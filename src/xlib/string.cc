/*
 * Copyright [2018] <Copyright u35s>
 */

#include "xlib/string.h"

namespace xlib {

void Split(const std::string& str,
    const std::string& delim,
    std::vector<std::string>* result) {
    if (str.empty()) {
        return;
    }
    if (delim[0] == '\0') {
        result->push_back(str);
        return;
    }

    size_t delim_length = delim.length();

    for (std::string::size_type begin_index = 0; begin_index < str.size();) {
        std::string::size_type end_index = str.find(delim, begin_index);
        if (end_index == std::string::npos) {
            result->push_back(str.substr(begin_index));
            return;
        }
        if (end_index > begin_index) {
            result->push_back(str.substr(begin_index, (end_index - begin_index)));
        }

        begin_index = end_index + delim_length;
    }
}

}  // namespace xlib
