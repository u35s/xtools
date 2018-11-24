/*
 * Copyright [2018] <Copyright u35s>
 */

#include "xlib/log.h"

#include "common/options.h"
#include "common/config.h"

int main(int argc, char **argv) {
    common::Options options;
    options.Init(argc, argv);

    if (options.log_file.empty() == false) {
        xlib::Log::Instance().SetLogFile(options.log_file);
    }
    if (options.log_level != 0) {
        xlib::Log::Instance().SetLogPriority(xlib::LOG_PRIORITY(options.log_level));
    }
    XINF("xsh 1.0")

    common::Config config;
    config.Init("./conf/example.conf");
    
    common::Host host;
    int index = 1;
    config.GetHost("a", index, &host);
    XINF("get a-%v, %v ,tag num %v", index, host.alias, host.tags.size());

    std::string tag("tag1");
    std::vector<common::Host> hosts;
    config.GetHostsByTag(tag, &hosts);
    XINF("get %v, num %v", tag, hosts.size());
    return 0;
}
