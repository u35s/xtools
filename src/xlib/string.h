/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef XLIB_STRING_H_
#define XLIB_STRING_H_

#include <vector>
#include <string>

namespace xlib {

void Split(const std::string& text, const std::string& sep, std::vector<std::string>* strs);

std::string& Ltrim(std::string& str); // NOLINT
std::string& Rtrim(std::string& str); // NOLINT
std::string& Trim(std::string& str);  // NOLINT

bool IsNumber(const std::string& a);

}  // namespace xlib


#endif  // XLIB_STRING_H_
