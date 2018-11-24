/*
 * Copyright [2018] <Copyright u35s>
 */

#include <string>
#include <fstream>
#include <algorithm>
#include "xlib/log.h"
#include "xlib/string.h"
#include "xlib/conv.h"
#include "common/config.h"

namespace common {

int Config::Init(std::string config_file) {
    std::ifstream infile(config_file.data());

    std::string group("default");
    std::string s;
    while (getline(infile, s)) {
        xlib::Trim(s);
        if (s.size() == 0 || s.find('#') == 0) {
            continue;
        }

        if (s.size() > 2 && s.find('[') == 0 && s.find(']') > 0) {
            group = s.substr(s.find('[')+1, s.find(']')-1);
            XDBG("set group %v", group);
            continue;
        }
        std::vector<std::string> vec;
        xlib::Split(s, " ",  &vec);
        if (vec.size() == 6) {
            cxx::shared_ptr<Host> host(new Host);
            host->alias = vec[0];
            host->ip    = vec[1];
            host->user  = vec[2];
            host->password = vec[3];
            host->port     = static_cast<uint32_t>(xlib::Stoi(vec[4]));

            std::vector<std::string> tags;
            xlib::Split(vec[5], "|", &tags);
            host->tags = tags;
            host->group = group;
            m_hosts.push_back(host);
            XDBG("add host %v in group %v", host->alias, group);
        } else {
            XDBG("ignore %v", s);
        }
    }
    XDBG("read host num %v ", m_hosts.size());
    infile.close();
}

int Config::GetHost(std::string group, int index, Host* host) {
    int num = 0;
    for (int i = 0; i < m_hosts.size(); i++) {
        if (m_hosts[i]->group == group) {
            if (num == index) {
                *host = *m_hosts[i];
                break;
            }
            num++;
        }
    }
}

int Config::GetHostsByTag(std::string tag, std::vector<Host>* hosts) {
    int num = 0;
    for (int i = 0; i < m_hosts.size(); i++) {
        std::vector<std::string>& v = m_hosts[i]->tags;
        if (std::count(v.begin(), v.end(), tag) > 0) {
            hosts->push_back(*m_hosts[i]);
        }
    }
}

}  // namespace common
