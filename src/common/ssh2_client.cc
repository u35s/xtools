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

    LIBSSH2_KNOWNHOSTS *nh;
    nh = libssh2_knownhost_init(session);
    IF_XFATAL(!nh, "eeek, do cleanup here");

    /* read all hosts from here */
    libssh2_knownhost_readfile(nh, "known_hosts",
                               LIBSSH2_KNOWNHOST_FILE_OPENSSH);

    /* store all known hosts to here */
    libssh2_knownhost_writefile(nh, "dumpfile",
                                LIBSSH2_KNOWNHOST_FILE_OPENSSH);

    const char *fingerprint;
    size_t len;
    int type;
    fingerprint = libssh2_session_hostkey(session, &len, &type);
    if (fingerprint) {
        struct libssh2_knownhost *host;
        int check = libssh2_knownhost_checkp(nh, m_ip.c_str(), m_port,
                                             fingerprint, len,
                                             LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                             LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                             &host);

        XDBG("Host check: %v, key: %v", check,
                (check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH)?
                host->key:"<none>");

        /*****
         * At this point, we could verify that 'check' tells us the key is
         * fine or bail out.
         *****/
    } else {
        XFATAL("eeek, do cleanup here");
    }
    libssh2_knownhost_free(nh);

    while ((rc = libssh2_userauth_password(session, m_user.c_str(), m_password.c_str())) ==
           LIBSSH2_ERROR_EAGAIN) {}
    IF_XFATAL(rc, "Authentication by password failed.");

    /* Exec non-blocking on the remove host */
    while ( (channel = libssh2_channel_open_session(session)) == NULL &&
           libssh2_session_last_error(session, NULL, NULL, 0) ==
           LIBSSH2_ERROR_EAGAIN ) {
        waitsocket(sock, session);
    }
    IF_XFATAL(channel == NULL, "channel null");
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
    }
    if (sock > 0) {
        close(sock);
    }
    libssh2_exit();
    XDBG("all done");
}

void SSH2Client::Run(std::string cmd) {
    XLOG("[%v@%v:%v %v] %v\n", m_user,  m_ip, m_port, m_alias, cmd);
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
