/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef XLIB_CONV_H_
#define XLIB_CONV_H_

#include <string>

namespace xlib {

char* Itoa(int num, int width, char* a);

int Atoi(const char* a);

int Stoi(const std::string a);

uint32_t Stou(const std::string a);

}  // namespace xlib

#endif  // XLIB_CONV_H_
