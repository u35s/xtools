/*
 * Copyright [2018] <Copyright u35s>
 */

#include <stdio.h>
#include "xlib/buffer.h"

namespace xlib {

Buffer::Buffer(int cap):
    m_data(new char[cap]),
    m_write_pos(0), m_read_pos(0), m_cap(cap) {
}

Buffer::~Buffer()  { delete [] m_data; }

void Buffer::Reset()  {
    m_write_pos = 0;
    m_read_pos = 0;
}

char* Buffer::Next()  {
    return m_data + m_write_pos;
}

int Buffer::Cap()  {
    return m_cap - m_write_pos;
}

int Buffer::WriteN(int n)  {
    return m_write_pos += n;
}

int Buffer::Write(const char* p)  {
    int n = snprintf(Next(), Cap(), "%s", p);
    if (n < 0) {
        return n;
    }
    return m_write_pos += n;
}

char* Buffer::Bytes()  {
    return m_data + m_read_pos;
}

int Buffer::Size()  {
    return m_write_pos - m_read_pos;
}

int Buffer::ReadN(int n)  {
    m_read_pos += n;
    if (Size() == 0) {
       Reset();
    }
    return m_read_pos;
}

}  // namespace xlib
