/*
 * Copyright [2018] <Copyright u35s>
 */

#include <cstdlib>

#include "xlib/conv.h"

namespace xlib {

int Atoi(const char* a) { return std::atoi(a); }

int Stoi(const std::string a) { return Atoi(a.c_str()); }

}  // namespace xlib
