/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef XLIB_BUFFER_H_
#define XLIB_BUFFER_H_

namespace xlib {

class Buffer {
 public:
    explicit Buffer(int cap);
    ~Buffer();

    // 恢复初始状态
    void  Reset();

    // 待写指针
    char* Next();
    // 剩余多少容量可写
    int   Cap();
    // 写入多少数据
    int   WriteN(int n);
    int   Write(const char* p);

    // 待读指针
    char* Bytes();
    // 剩余多少数据可读
    int   Size();
    // 读取多少数据
    int   ReadN(int n);

 private:
    char * m_data;
    int  m_write_pos;
    int  m_read_pos;
    int  m_cap;
};

}  // namespace xlib

#endif  // XLIB_BUFFER_H_
