/*
 * Copyright [2018] <Copyright u35s>
 */

#include "xlib/log.h"

#include "common/options.h"

int main(int argc, char **argv) {
    common::Options options;
    options.Init(argc, argv);

    if (options.log_file.empty() == false) {
        xlib::Log::Instance().SetLogFile(options.log_file);
    }
    if (options.log_level != 0) {
        xlib::Log::Instance().SetLogPriority(xlib::LOG_PRIORITY(options.log_level));
    }

    INF("xsh 1.0")
    return 0;
}
