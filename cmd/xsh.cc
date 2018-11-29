/*
 * Copyright [2018] <Copyright u35s>
 */

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

    std::vector<common::Host> hosts;
    config.GetHostsByOptions(options, &hosts);

    for (int i = 0; i < hosts.size(); i++) {
        common::Host& host = hosts[i];
        common::SSH2Client client(
            host.user, host.password, host.ip, host.port, host.alias);
        if (options.cmd == "l") {
            client.Open(false);
            client.Login();
        } else if (options.cmd == "r") {
            client.Open();
            client.Run(options.params);
        }
    }

    return 0;
}
