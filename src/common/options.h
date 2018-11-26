/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef COMMON_OPTIONS_H_
#define COMMON_OPTIONS_H_

#include <stdint.h>
#include <string>

namespace common {

enum RunType {
    RunType_index,
    RunType_group,
    RunType_group_index,
    RunType_group_tag,
};

struct Options {
    Options();

    void Init(int argc, char **argv);

    std::string  cmd;
    std::string  group;
    std::string  tag;
    std::string  params;
    int32_t      index;
    RunType      run_type;

    int32_t      log_level;
    std::string  log_file;

    std::string  config_file;
};

}  // namespace common

#endif  // COMMON_OPTIONS_H_
