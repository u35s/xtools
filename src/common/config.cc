/*
 * Copyright [2018] <Copyright u35s>
 */

#include <string>
#include <fstream>
#include <algorithm>
#include "xlib/log.h"
#include "xlib/string.h"
#include "xlib/conv.h"
#include "common/options.h"
#include "common/config.h"

namespace common {

int Config::Init(const std::string& config_file) {
    std::ifstream infile(config_file.data());

    std::string group("");
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
    return 0;
}

bool Config::GetHost(const std::string& group, int index, Host* host) {
    int num = 0;
    for (int i = 0; i < m_hosts.size(); i++) {
        if (m_hosts[i]->group == group) {
            if (num == index) {
                *host = *m_hosts[i];
                return true;
            }
            num++;
        }
    }
    return false;
}

int Config::GetHosts(const std::string& group, const std::string& tag, std::vector<Host>* hosts) {
    int num = 0;
    for (int i = 0; i < m_hosts.size(); i++) {
        if (m_hosts[i]->group == group) {
            std::vector<std::string>& v = m_hosts[i]->tags;
            if (tag.empty() || std::count(v.begin(), v.end(), tag) > 0) {
                hosts->push_back(*m_hosts[i]);
                num++;
            }
        }
    }
    return num;
}

int Config::GetHostsByOptions(const Options& options, std::vector<Host>* hosts) {
    switch (options.run_type) {
    case RunType_index :
    case RunType_group_index : {
        Host host;
        if (GetHost(options.group, options.index, &host)) {
            hosts->push_back(host);
        }
        break;
    }
    case RunType_group :
    case RunType_group_tag :
        GetHosts(options.group, options.tag, hosts);
        break;
    }
    return 0;
}

}  // namespace common
