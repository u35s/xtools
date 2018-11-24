/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef XLIB_LOG_H_
#define XLIB_LOG_H_

#include <string>
#include <sstream>
#include "xlib/time.h"

namespace xlib {

/*
 * printf 格式 [标志][输出最小宽度][.精度][长度]类型
 *
 * 标志          - + # 空格
 *
 *               -         结果左对齐，右边填空格
 *               +        输出符号(正号或负号)
 *               空格     输出值为正时冠以空格，为负时冠以负号
 *               #
 *                        对c、s、d、u类无影响；
 *                        对o类，在输出时加前缀o；
 *                        对x类，在输出时加前缀0x；
 *                        对e、g、f 类当结果有小数时才给出小数点。
 *
 * 输出最小宽度  用十进制整数来表示输出的最少位数。
 *               若实际位数多于定义的宽度，则按实际位数输出，
 *               若实际位数少于定义的宽度则补以空格或0。
 *
 * 精度          精度格式符以“.”开头，后跟十进制整数。
 *               本项的意义是：如果输出数字，则表示小数的位数；
 *               如果输出的是字符，则表示输出字符的个数；
 *               若实际位数大于所定义的精度数，则截去超过的部分。
 *
 * 长度          长度格式符为h、l两种，h表示按短整型量输出，l表示按长整型量输出。
 *
 * 类型          类型字符用以表示输出数据的类型，其格式符和意义如下表所示：
 *
 *               d    以十进制形式输出带符号整数(正数不输出符号)
 *               o    以八进制形式输出无符号整数(不输出前缀0)
 *               x,X  以十六进制形式输出无符号整数(不输出前缀Ox)
 *               u    以十进制形式输出无符号整数
 *               f    以小数形式输出单、双精度实数
 *               e,E  以指数形式输出单、双精度实数
 *               g,G  以%f或%e中较短的输出宽度输出单、双精度实数
 *               c    输出单个字符
 *               s    输出字符串
 * */

#define ARRAYSIZE(a) (sizeof(a) / sizeof(*(a)))

#if __cplusplus >= 201103L
#define XTRACE(fmt, ...) xlib::Log::Instance().SWrite(xlib::LOG_PRIORITY_TRACE, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);  // NOLINT
#define XDBG(fmt, ...)   xlib::Log::Instance().SWrite(xlib::LOG_PRIORITY_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);  // NOLINT
#define XINF(fmt, ...)   xlib::Log::Instance().SWrite(xlib::LOG_PRIORITY_INFO,  __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);  // NOLINT
#define XERR(fmt, ...)   xlib::Log::Instance().SWrite(xlib::LOG_PRIORITY_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);  // NOLINT
#define XFATAL(fmt, ...) xlib::Log::Instance().SWrite(xlib::LOG_PRIORITY_FATAL, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);  // NOLINT
#endif

#define TRACE(fmt, ...) xlib::Log::Instance().Write(xlib::LOG_PRIORITY_TRACE, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);  // NOLINT
#define DBG(fmt, ...)   xlib::Log::Instance().Write(xlib::LOG_PRIORITY_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);  // NOLINT
#define INF(fmt, ...)   xlib::Log::Instance().Write(xlib::LOG_PRIORITY_INFO,  __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);  // NOLINT
#define ERR(fmt, ...)   xlib::Log::Instance().Write(xlib::LOG_PRIORITY_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);  // NOLINT
#define FATAL(fmt, ...) xlib::Log::Instance().Write(xlib::LOG_PRIORITY_FATAL, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);  // NOLINT

typedef enum {
    LOG_PRIORITY_NULL = 0,
    LOG_PRIORITY_TRACE,
    LOG_PRIORITY_DEBUG,
    LOG_PRIORITY_INFO,
    LOG_PRIORITY_ERROR,
    LOG_PRIORITY_FATAL,
} LOG_PRIORITY;

class Log {
 protected:
    Log();

 public:
    ~Log();

    static Log& Instance() {
        static Log s_log_instance;
        return s_log_instance;
    }

#if __cplusplus >= 201103L
    template <typename ...Args>
    void SWrite(LOG_PRIORITY pri, const char* file, uint32_t line, const char* function,
        const std::string& format, const Args&... args) {
        std::string temp(format);
        Format(&temp, args...);
        Write(pri, file, line, function, temp.c_str());
    }

    void Format(std::string* format) {}

    template <typename First, typename... Rest>
    void Format(std::string* format,  const First& first, const Rest&... rest) {
        size_t index = format->find_first_of("##");
        if (index == std::string::npos) {
            return;
        }
        std::ostringstream oss;
        oss << first;
        format->replace(index, 2, oss.str());
        Format(format, rest...);
    }
#endif

    void Write(LOG_PRIORITY pri, const char* file, uint32_t line, const char* function,
        const char* fmt, ...);

    void SetLogPriority(LOG_PRIORITY pri);
    int  SetLogFile(std::string file);
    void Close();

 private:
    LOG_PRIORITY m_log_priority;
    int          m_log_fd;
    std::string  m_log_file;
};

}  // namespace xlib

#endif  // XLIB_LOG_H_
