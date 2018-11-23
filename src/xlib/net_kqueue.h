/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef XLIB_NET_KQUEUE_H_
#define XLIB_NET_KQUEUE_H_

#include "xlib/net_poll.h"

namespace xlib {

class Kqueue : public Poll {
 public:
    Kqueue();
    ~Kqueue();

    int32_t Init(uint32_t max_event);

    int32_t Wait(int32_t timeout_ms);

    int32_t AddFd(int32_t fd, uint32_t events, uint64_t data);

    int32_t DelFd(int32_t fd);

    int32_t ModFd(int32_t fd, uint32_t events, uint64_t data);

    int32_t GetEvent(uint32_t *events, uint64_t *data);


 private:
    int32_t m_kqueue_fd;
    int32_t m_max_event;
    int32_t m_event_num;
    struct kevent* m_events;
};

}  // namespace xlib

#endif  // XLIB_NET_KQUEUE_H_
