/*
 * Copyright [2018] <Copyright u35s>
 */

#include "xlib/log.h"

#include "xlib/conv.h"
#include "xlib/string.h"
#include "common/options.h"
#include "common/config.h"
#include "common/ssh2_client.h"

struct XcpOptions : public common::Options {
    std::string remote_path;
    std::string local_path;

    bool remote_to_local;
    bool local_to_remote;

    void Init(int argc, char **argv) {
        common::Options::Init(argc, argv);
        int pre = opt_cur;
        while ( opt_cur < argc && (pre + 2)>opt_cur ) {
            std::vector<std::string> vec;
            xlib::Split(argv[opt_cur], ":", &vec);
            if (vec.size() == 2) {
                pre == opt_cur ? remote_to_local = true : local_to_remote = true;
                common::Options::ParseHosts(vec[0]);
                remote_path = vec[1];
            } else {
                local_path = vec[0];
            }
            opt_cur++;
        }
    }
};

int main(int argc, char **argv) {
    XcpOptions options;
    options.Init(argc, argv);

    if (options.log_file.empty() == false) {
        xlib::Log::Instance().SetLogFile(options.log_file);
    }
    if (options.log_level > 0) {
        xlib::Log::Instance().SetLogPriority(xlib::LOG_PRIORITY(options.log_level));
    }

    common::Config config;
    config.Init(options.config_file);

    std::vector<common::Host> hosts;
    config.GetHostsByOptions(options, &hosts);
    for (int i = 0; i < hosts.size(); i++) {
        common::Host& host = hosts[i];
        common::SSH2Client client(
            host.user, host.password, host.ip, host.port, host.alias);
        client.Open();
        if (options.remote_to_local) {
            client.Copy(options.local_path, options.remote_path);
        }
        if (options.local_to_remote) {
            client.Send(options.local_path, options.remote_path);
        }
    }
    return 0;
}
