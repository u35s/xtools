/*
 * Copyright [2018] <Copyright u35s>
 */

#include <algorithm>
#include <functional>
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

std::string& Ltrim(std::string& str) { // NOLINT
    std::string::iterator it = find_if(str.begin(), str.end(), std::not1(std::ptr_fun(::isspace)));
    str.erase(str.begin(), it);
    return str;
}

std::string& Rtrim(std::string& str) { // NOLINT
    std::string::reverse_iterator it = find_if(str.rbegin(),
        str.rend(), std::not1(std::ptr_fun(::isspace)));

    str.erase(it.base(), str.end());
    return str;
}

std::string& Trim(std::string& str) { // NOLINT
    return Rtrim(Ltrim(str));
}

bool IsNumber(const std::string& a) {
    for (int i = 0; i < a.size(); i++) {
        if (!isdigit(a[i])) {
            return false;
        }
    }
    return a.size() > 0;
}

std::string& Color(std::string& str,      // NOLINT
    const std::string color, int width) { // NOLINT
    char s[300] = {0};
    snprintf(s, sizeof(s), "%-*s", width, str.c_str());
    str = s;
    if (color == "red") {
        str.insert(0, "\x1b[31m");
        str.append("\x1b[0m");
    }
    if (color == "green") {
        str.insert(0, "\x1b[32m");
        str.append("\x1b[0m");
    }
    return str;
}

}  // namespace xlib
