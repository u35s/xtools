/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef XLIB_STRING_H_
#define XLIB_STRING_H_

#include <vector>
#include <string>

namespace xlib {

void Split(const std::string& text, const std::string& sep, std::vector<std::string>* strs);

}  // namespace xlib

#endif  // XLIB_STRING_H_
