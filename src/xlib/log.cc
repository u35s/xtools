/*
 * Copyright [2018] <Copyright u35s>
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <memory.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include "xlib/log.h"
#include "xlib/trace.h"

namespace xlib {

static const struct {
        int number;
        const char* name;
    } g_failure_signals[] = {
        { SIGSEGV, "SIGSEGV" },
        { SIGILL,  "SIGILL"  },
        { SIGFPE,  "SIGFPE"  },
        { SIGABRT, "SIGABRT" },
        { SIGBUS,  "SIGBUS"  },
        { SIGTERM, "SIGTERM" }
    };

static struct sigaction g_sigaction_bak[ARRAYSIZE(g_failure_signals)];

void on_error_signal(int signum, siginfo_t* siginfo, void* ucontext) {
    // 获取信号名
    const char* signame = "";
    uint32_t i = 0;
    for (; i < ARRAYSIZE(g_failure_signals); i++) {
        if (g_failure_signals[i].number == signum) {
            signame = g_failure_signals[i].name;
            break;
        }
    }
    ERR("recive signal %d, %s", signum, signame);
    xlib::PrintStack();

    // 恢复默认处理
    sigaction(signum, &g_sigaction_bak[i], NULL);
    kill(getpid(), signum);
}

// 错误信号处理
void init_error_signal() {
    struct sigaction sig_action;
    memset(&sig_action, 0, sizeof(sig_action));
    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_flags |= SA_SIGINFO;
    sig_action.sa_sigaction = &on_error_signal;

    for (uint32_t i = 0; i < ARRAYSIZE(g_failure_signals); i++) {
        sigaction(g_failure_signals[i].number, &sig_action, &g_sigaction_bak[i]);
    }
}

static const char*  g_priority_str[] = { "NULL", "TRACE", "DEBUG", "INFO", "ERROR", "FATAL" };

Log::Log() : m_log_priority(LOG_PRIORITY_INFO), m_log_fd(-1) {
    init_error_signal();
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
    pre_len = 0;
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
