/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef XLIB_PLATFORM_H_
#define XLIB_PLATFORM_H_

#if __cplusplus < 201103L

#include <tr1/unordered_map>  //  NOLINT
#include <tr1/memory>         //  NOLINT


#else  // c++ 11

#include <unordered_map>
#include <memory>

#endif

namespace xlib {
namespace stdcxx {

#if __cplusplus < 201103L

using ::std::tr1::unordered_map;
using ::std::tr1::shared_ptr;

#else  // c++ 11

using ::std::unordered_map;
using ::std::shared_ptr;

#endif

}  // namespace stdcxx
}  // namespace xlib

namespace cxx = xlib::stdcxx;

#endif  // XLIB_PLATFORM_H_
