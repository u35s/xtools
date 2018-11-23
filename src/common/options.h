/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef COMMON_OPTIONS_H_
#define COMMON_OPTIONS_H_

#include <stdint.h>
#include <string>

namespace common {

struct Options {
    Options();

    void Init(int argc, char **argv);

    uint32_t     log_level;
    std::string  log_file;
};

}  // namespace common

#endif  // COMMON_OPTIONS_H_
