/*
 * Copyright [2018] <Copyright u35s>
 */

#include <stdio.h>
#include <unistd.h>

#include "xlib/conv.h"
#include "common/options.h"

namespace common {

Options::Options() {
    log_level       = 0;
    log_file        = "";
}

void Options::Init(int argc, char **argv) {
    int opt  = 0;
    while ( -1 != (opt = getopt(argc, argv, "l:f:")) ) {
        switch (opt) {
            case 'l':
                log_level = xlib::Atoi(optarg);
                break;
            case 'f':
                log_file = optarg;
                break;
        }
    }
}

}  // namespace common
