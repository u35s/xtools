/*
 * Copyright [2018] <Copyright u35s>
 */

#include <algorithm>
#include "xlib/log.h"

#include "xlib/conv.h"
#include "xlib/string.h"
#include "common/options.h"
#include "common/config.h"
#include "common/ssh2_client.h"

struct XshOptions : public common::Options {
    void Init(int argc, char **argv) {
        common::Options::Init(argc, argv);
        if (opt_cur < argc) { cmd = argv[opt_cur]; opt_cur++; }
        if (opt_cur < argc) {
            std::string hosts = argv[opt_cur];
            common::Options::ParseHosts(hosts);
            opt_cur++;
        }
        while (opt_cur < argc) {
            params.append(argv[opt_cur]);
            params.append(" ");
            opt_cur++;
        }
    }
};

int main(int argc, char **argv) {
    XshOptions options;
    options.Init(argc, argv);

    if (options.log_file.empty() == false) {
        xlib::Log::Instance().SetLogFile(options.log_file);
    }
    if (options.log_level > 0) {
        xlib::Log::Instance().SetLogPriority(xlib::LOG_PRIORITY(options.log_level));
    }
    XDBG("xsh 1.0")
    XDBG("cmd %v, group %v, tag %v, index %v, params %v",
        options.cmd, options.group, options.tag, options.index, options.params)

    common::Config config;
    config.Init(options.config_file);

    if (options.cmd != "l" && options.cmd != "r" && options.cmd != "i") {
        const std::vector<std::string> vec = config.GetGroupNames();
        auto it = std::find(vec.begin(), vec.end(), options.cmd);
        bool one = (it != vec.end() && options.cmd != "");

        for (size_t i = 0; i < vec.size(); i++) {
            if (one && vec[i] != options.cmd) {
                continue;
            }
            std::vector<common::Host> hosts;
            std::string tag;
            config.GetHosts(vec[i], tag, &hosts);

            XLOG("[%v]\n", vec[i]);
            for (size_t i = 0; i < hosts.size(); i++) {
                common::Host& host = hosts[i];
                std::string indexs(std::to_string(i));
                XLOG("%v\t%v服\t%v\t",
                    xlib::Color(host.ip, "", 15),
                    xlib::Color(indexs, "green", 3),
                    xlib::Color(host.alias, "red", 15));
                if (i != 0 && (i+1)%3 == 0) {
                    XLOG("\n");
                }
            }
            XLOG("\n\n");
        }
        return 0;
    }

    std::vector<common::Host> hosts;
    config.GetHostsByOptions(options, &hosts);
    for (size_t i = 0; i < hosts.size(); i++) {
        common::Host& host = hosts[i];
        common::SSH2Client client(
            host.user, host.password, host.ip, host.port, host.alias);
        if (options.cmd == "l") {
            client.Open(false);
            client.Login();
        } else if (options.cmd == "r") {
            client.Open();
            client.Run(options.params);
        } else if (options.cmd == "i") {
            client.Exec(options.params);
        }
    }
    return 0;
}
