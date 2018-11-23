/*
 * Copyright [2018] <Copyright u35s>
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include "xlib/log.h"

namespace xlib {

static const char*  g_priority_str[] = { "NULL", "TRACE", "DEBUG", "INFO", "ERROR", "FATAL" };

Log::Log() : m_log_priority(LOG_PRIORITY_INFO), m_log_fd(-1) {
}

Log::~Log() {
    Close();
}

void Log::Close() {
    if (m_log_fd > -1) {
        close(m_log_fd);
        m_log_fd = -1;
    }
}

int  Log::SetLogFile(std::string file) {
    Close();
    m_log_file = file;
    int fd = open(m_log_file.c_str(), O_CREAT | O_APPEND | O_RDWR);
    if (fd < 0) {
        ERR("set log file, open faild");
    } else {
        m_log_fd = fd;
    }
    return 0;
}

void Log::SetLogPriority(LOG_PRIORITY pri) {
    m_log_priority = pri;
}

void Log::Write(LOG_PRIORITY pri, const char* file, uint32_t line, const char* function,
    const char* fmt, ...) {
    if (pri < m_log_priority) {
        return;
    }
    static char buff[4096] = {0};
    int pre_len = 0;
    pre_len = snprintf(buff, ARRAYSIZE(buff), "[%s][%s][%d][%s:%d:%s] ",
        xlib::Time::Now().String().c_str(), g_priority_str[pri],
        getpid(), file, line, function);
    if (pre_len < 0) {
        pre_len = 0;
    }
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(buff + pre_len, ARRAYSIZE(buff) - pre_len, fmt, ap);
    va_end(ap);
    if (len < 0) {
        len = 0;
    }
    uint32_t tail = len + pre_len;
    if (tail > (ARRAYSIZE(buff) - 2)) {
        tail = ARRAYSIZE(buff) - 2;
    }
    buff[tail++] = '\n';
    buff[tail] = '\0';
    if (m_log_fd > -1) {
        write(m_log_fd, buff, tail);
    } else {
        fprintf(stdout, "%s", buff);
    }
}

}  // namespace xlib
