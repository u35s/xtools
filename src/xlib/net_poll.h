/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef XLIB_NET_POLL_H_
#define XLIB_NET_POLL_H_

#define POLLIN   1
#define POLLOUT  4
#define POLLERR  8
#define POLLEOF  8

#include "xlib/net_util.h"

namespace xlib {

class Poll {
 public:
    Poll() : m_bind_net_io(NULL) {  }
    virtual ~Poll() {  }

    virtual int32_t Init(uint32_t max_event) { return 0; }

    virtual int32_t Wait(int32_t timeout_ms) { return 0; }

    virtual int32_t AddFd(int32_t fd, uint32_t events, uint64_t data) { return 0; }

    virtual int32_t DelFd(int32_t fd) { return 0; }

    virtual int32_t ModFd(int32_t fd, uint32_t events, uint64_t data) { return 0; }

    virtual int32_t GetEvent(uint32_t *events, uint64_t *data) { return 0; }


    NetIO*  m_bind_net_io;
 protected:
};

}  // namespace xlib

#endif  // XLIB_NET_POLL_H_
