/*
 * Copyright [2018] <Copyright u35s>
 */

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <string>

#include "xlib/time.h"

namespace xlib {

Time::Time() : m_microsecond(0) {}

Time::Time(uint64_t micro) : m_microsecond(micro) {}

Time::~Time() {}

Time Time::Now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t micro = tv.tv_sec * DURATION_PER_SEC + tv.tv_usec;
    return Time(micro);
}

std::string Time::String() {
    time_t now = m_microsecond / DURATION_PER_SEC;
    uint64_t micro = m_microsecond % DURATION_PER_SEC;
    struct tm tm_now;
    struct tm* p_tm_now;

    p_tm_now = localtime_r(&now, &tm_now);

    char buff[256] = {0};
    snprintf(buff, sizeof(buff), "%04d-%02d-%02d% 02d:%02d:%02d:%06" PRIu64,
        1900 + p_tm_now->tm_year,
        p_tm_now->tm_mon + 1,
        p_tm_now->tm_mday,
        p_tm_now->tm_hour,
        p_tm_now->tm_min,
        p_tm_now->tm_sec,
        micro);

    return std::string(buff);
}

uint64_t Time::Reset() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    m_microsecond = tv.tv_sec * DURATION_PER_SEC + tv.tv_usec;
    return m_microsecond;
}

uint64_t Time::Elapse() {
    return Now().Micro() - m_microsecond;
}

uint64_t Time::Micro() {
    return m_microsecond;
}

uint64_t Time::Unix() {
    return m_microsecond / DURATION_PER_SEC;
}

}  // namespace xlib
