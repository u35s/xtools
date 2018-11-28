/*
 * Copyright [2018] <Copyright u35s>
 */

#include <stdio.h>
#include <unistd.h>
#include <vector>

#include "xlib/conv.h"
#include "xlib/log.h"
#include "xlib/string.h"
#include "common/options.h"

namespace common {

Options::Options() : index(-1), log_level(-1) {
    config_file = "/etc/xtools.conf";
}

void Options::ParseHosts(const std::string& s) {
    std::vector<std::string> vec;
    xlib::Split(s, "-", &vec);
    if (vec.size() == 2) {
        group  = vec[0];
        if (xlib::IsNumber(vec[1])) {
            run_type = common::RunType_group_index;
            index = xlib::Stou(vec[1]);
        } else {
            tag = vec[1];
            run_type = common::RunType_group_tag;
        }
    } else if (vec.size() == 1) {
        if (xlib::IsNumber(vec[0])) {
            index = xlib::Stou(vec[0]);
            run_type = common::RunType_index;
        } else {
            group = vec[0];
            run_type = common::RunType_group;
        }
    }
}

void Options::Init(int argc, char **argv) {
    int opt  = 0;
    while ( -1 != (opt = getopt(argc, argv, "l:f:c:")) ) {
        switch (opt) {
            case 'l':
                log_level = xlib::Atoi(optarg);
                break;
            case 'f':
                log_file = optarg;
                break;
            case 'c':
                config_file = optarg;
                break;
        }
    }
    XDBG("optind %v", optind);
    opt_cur = optind;
}

}  // namespace common
