/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef XLIB_TIME_H_
#define XLIB_TIME_H_

#include <stdint.h>
#include <string>

#define DURATION_PER_SEC 1000000

namespace xlib {

class Time {
 public:
    Time();
    ~Time();

    explicit Time(uint64_t second);  // 构造函数单个参数声明explicit
    uint64_t Elapse();
    uint64_t Micro();
    uint64_t Unix();
    uint64_t Reset();

    std::string String();

    static Time Now();
 private:
    uint64_t m_microsecond;
};

}  // namespace xlib

#endif  // XLIB_TIME_H_
