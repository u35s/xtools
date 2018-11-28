/*
 * Copyright [2018] <Copyright u35s>
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <fstream>
#include "libssh2.h" // NOLINT
#include "xlib/conv.h"
#include "xlib/log.h"
#include "common/ssh2_client.h"

namespace common {

SSH2Client::SSH2Client(std::string user, std::string password,
    std::string ip, uint16_t port, std::string alias) :
    m_user(user), m_password(password), m_ip(ip), m_port(port), m_alias(alias) {
}

SSH2Client::~SSH2Client() {
    Shutdown();
}

static int waitsocket(int socket_fd, LIBSSH2_SESSION *session) {
    struct timeval timeout;
    int rc;
    fd_set fd;
    fd_set *writefd = NULL;
    fd_set *readfd = NULL;
    int dir;

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fd);

    FD_SET(socket_fd, &fd);

    /* now make sure we wait in the correct direction */
    dir = libssh2_session_block_directions(session);

    if (dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        readfd = &fd;

    if (dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        writefd = &fd;

    rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);

    return rc;
}

void SSH2Client::Open() {
    rc = libssh2_init(0);
    IF_XFATAL(rc != 0, "libssh2 initialization failed (%v)", rc);

    uint64_t hostaddr;
    hostaddr = inet_addr(m_ip.c_str());
    sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(m_port);
    sin.sin_addr.s_addr = hostaddr;
    rc = connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in));
    IF_XFATAL(rc != 0, "failed to connect %v:%v", m_ip, m_port);

    session = libssh2_session_init();
    IF_XFATAL(!session, "session init error");

    /* tell libssh2 we want it all done non-blocking */
    libssh2_session_set_blocking(session, 0);

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    while ((rc = libssh2_session_handshake(session, sock)) ==
           LIBSSH2_ERROR_EAGAIN) {}
    IF_XFATAL(rc, "Failure establishing SSH session: %v", rc);

    while ((rc = libssh2_userauth_password(session, m_user.c_str(), m_password.c_str())) ==
           LIBSSH2_ERROR_EAGAIN) {}
    IF_XFATAL(rc, "Authentication by password failed.");
}

void SSH2Client::Shutdown() {
    if (channel != NULL) {
        libssh2_channel_free(channel);
        channel = NULL;
    }
    if (session != NULL) {
        libssh2_session_disconnect(session,
                                   "Normal Shutdown, Thank you for playing");
        libssh2_session_free(session);
        session = NULL;
    }
    if (sock > 0) {
        close(sock);
    }
    libssh2_exit();
    XDBG("all done");
}

void SSH2Client::Send(std::string local_path, std::string remote_path) {
    FILE *local;
    struct stat fileinfo;
    local = fopen(local_path.c_str(), "rb");
    IF_XFATAL(!local, "Can't local file %v", local_path);
    stat(local_path.c_str(), &fileinfo);

    /* Send a file via scp. The mode parameter must only have permissions! */
    do {
        channel = libssh2_scp_send(session, remote_path.c_str(), fileinfo.st_mode & 0777,
                                   (unsigned long)fileinfo.st_size);

        if ((!channel) && (libssh2_session_last_errno(session) !=
                           LIBSSH2_ERROR_EAGAIN)) {
            char *err_msg;

            libssh2_session_last_error(session, &err_msg, NULL, 0);
            XFATAL("%v", err_msg);
        }
    } while (!channel);

    XDBG("%v", "SCP session waiting to send file");
    char mem[1024*100];
    char *ptr;
    long total = 0;
    size_t prev;
    size_t nread;
    do {
        nread = fread(mem, 1, sizeof(mem), local);
        if (nread <= 0) {
            /* end of file */
            break;
        }
        ptr = mem;

        total += nread;

        prev = 0;
        do {
            while ((rc = libssh2_channel_write(channel, ptr, nread)) ==
                   LIBSSH2_ERROR_EAGAIN) {
                waitsocket(sock, session);
                prev = 0;
            }
            if (rc < 0) {
                XERR("ERROR %v total %lv / %v prev %v", rc,
                        total, (int)nread, (int)prev);
                break;
            }
            else {
                prev = nread;

                /* rc indicates how many bytes were written this time */
                nread -= rc;
                ptr += rc;
            }
        } while (nread);
    } while (!nread); /* only continue if nread was drained */
    XDBG("%v bytes ", total);

    XDBG("Sending EOF");
    while (libssh2_channel_send_eof(channel) == LIBSSH2_ERROR_EAGAIN);

    XDBG("Waiting for EOF");
    while (libssh2_channel_wait_eof(channel) == LIBSSH2_ERROR_EAGAIN);

    XDBG("Waiting for channel to close");
    while (libssh2_channel_wait_closed(channel) == LIBSSH2_ERROR_EAGAIN);
}

void SSH2Client::Copy(std::string local_path, std::string remote_path) {
    XDBG("libssh2_scp_recv2()");
    libssh2_struct_stat fileinfo;
    do {
        channel = libssh2_scp_recv2(session, remote_path.c_str(), &fileinfo);
        if (!channel) {
            if (libssh2_session_last_errno(session) != LIBSSH2_ERROR_EAGAIN) {
                char *err_msg;
                libssh2_session_last_error(session, &err_msg, NULL, 0);
                XFATAL("%v", err_msg);
            } else {
                XDBG("libssh2_scp_recv() spin");
                waitsocket(sock, session);
            }
        }
    } while (!channel);
    XDBG("libssh2_scp_recv() is done, now receive data!");

    int rc;
    int spin = 0;
    libssh2_struct_stat_size got = 0;
    libssh2_struct_stat_size total = 0;
    std::string copy_file = local_path + "-" + m_ip;
    std::ofstream out(copy_file, std::ofstream::out);
    IF_XFATAL(!out, "%v open fail", copy_file);
    while (got < fileinfo.st_size) {
        char mem[1024*24];
        do {
            int amount = sizeof(mem);

            if ((fileinfo.st_size -got) < amount) {
                amount = static_cast<int>(fileinfo.st_size - got);
            }

            /* loop until we block */
            rc = libssh2_channel_read(channel, mem, amount);
            if (rc > 0) {
                out.write(mem, rc);
                got += rc;
                total += rc;
            }
        } while (rc > 0);

        if ((rc == LIBSSH2_ERROR_EAGAIN) && (got < fileinfo.st_size)) {
            /* this is due to blocking that would occur otherwise
            so we loop on this condition */

            spin++;
            waitsocket(sock, session); /* now we wait */
            continue;
        }
        break;
    }
}

void SSH2Client::Run(std::string cmd) {
    XLOG("[%v@%v:%v %v] %v\n", m_user,  m_ip, m_port, m_alias, cmd);
    /* Exec non-blocking on the remove host */
    while ( (channel = libssh2_channel_open_session(session)) == NULL &&
           libssh2_session_last_error(session, NULL, NULL, 0) ==
           LIBSSH2_ERROR_EAGAIN ) {
        waitsocket(sock, session);
    }
    IF_XFATAL(channel == NULL, "channel null");

    while ( (rc = libssh2_channel_exec(channel, cmd.c_str())) ==
          LIBSSH2_ERROR_EAGAIN ) {
        waitsocket(sock, session);
    }
    IF_XFATAL(rc != 0, "exec error");
    int bytecount = 0;
    for ( ;; ) {
        /* loop until we block */
        do {
            char buffer[0x4000];
            rc = libssh2_channel_read(channel, buffer, sizeof(buffer));
            if ( rc > 0 ) {
                int i;
                bytecount += rc;
                XLOG("%v", std::string(buffer, rc));
            }
        } while ( rc > 0 );

        /* this is due to blocking that would occur otherwise so we loop on
           this condition */
        if ( rc == LIBSSH2_ERROR_EAGAIN ) {
            waitsocket(sock, session);
        } else {
            break;
        }
    }
    int exitcode = 127;
    while ( (rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN )
        waitsocket(sock, session);

    char *exitsignal = const_cast<char *>("none");
    if ( rc == 0 ) {
        exitcode = libssh2_channel_get_exit_status(channel);
        libssh2_channel_get_exit_signal(channel, &exitsignal,
                                        NULL, NULL, NULL, NULL, NULL);
    }

    if (exitsignal) {
        XDBG("got signal: %v\n", exitsignal);
    } else {
        XDBG("exit: %v bytecount: %v\n", exitcode, bytecount);
    }
}

}  // namespace common
