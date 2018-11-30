/*
 * Copyright [2018] <Copyright u35s>
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "xlib/conv.h"

namespace xlib {

char* Itoa(int num, int width, char* a) { sprintf(a, "% *d", width, num); return a; }

int Atoi(const char* a) { return std::atoi(a); }

int Stoi(const std::string a) { return Atoi(a.c_str()); }

uint32_t Stou(const std::string a) { return uint32_t(Stoi(a)); }

}  // namespace xlib
