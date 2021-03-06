/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef COMMON_SSH2_CLIENT_H_
#define COMMON_SSH2_CLIENT_H_

#include <termios.h>
#include <stdint.h>
#include <string>

#include "libssh2.h" // NOLINT

namespace common {

class SSH2Client {
 public:
    SSH2Client(std::string user, std::string password,
        std::string ip, uint16_t port, std::string alias);
    ~SSH2Client();
    void Exec(std::string cmd);
    void Open(bool noblock = true);
    void Login();
    void Run(std::string cmd);
    void Copy(std::string local_path, std::string remote_path);
    void Send(std::string local_path, std::string remote_path);
    void Shutdown();

 private:
    int RawMode();
    int NormalMode();
    std::string  m_user;
    std::string  m_password;
    std::string  m_ip;
    uint16_t     m_port;
    std::string  m_alias;

    int rc;
    int sock;
    LIBSSH2_SESSION *session = NULL;
    LIBSSH2_CHANNEL *channel = NULL;
    struct termios  saved_tio;
};

}  // namespace common

#endif  // COMMON_SSH2_CLIENT_H_
