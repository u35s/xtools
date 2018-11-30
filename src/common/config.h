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
    int  Init(const std::string& config_file);
    bool GetHost(const std::string& group, int index, Host* host);
    int  GetHosts(const std::string& group, const std::string& tag, std::vector<Host>* hosts);
    int  GetHostsByOptions(const Options& options, std::vector<Host>* hosts);
    const std::vector<std::string> GetGroupNames();

 private:
    std::vector< cxx::shared_ptr<Host> > m_hosts;
    std::vector< std::string > m_group_names;
};

}  // namespace common

#endif  // COMMON_CONFIG_H_
