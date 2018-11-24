/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef COMMON_CONFIG_H_
#define COMMON_CONFIG_H_

#include <stdint.h>
#include <string>
#include <vector>
#include "xlib/platform.h"

namespace common {

struct Host {
    std::string  alias;
    std::string  user;
    std::string  password;
    std::string  ip;
    uint16_t     port;

    std::vector<std::string> tags;
    std::string              group;
};

class Config {
 public:
    Config()  {}
    ~Config() {}
    int Init(std::string config_file);
    int GetHost(std::string group, int index, Host* host);
    int GetHostsByTag(std::string tag, std::vector<Host>* hosts);

 private:
    std::vector< cxx::shared_ptr<Host> > m_hosts;
};

}  // namespace common

#endif  // COMMON_CONFIG_H_
