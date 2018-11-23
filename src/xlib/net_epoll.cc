/*
 * Copyright [2018] <Copyright u35s>
 */

#if defined(__linux__)

#include <errno.h>
#include "xlib/net_epoll.h"
#include "xlib/log.h"

namespace xlib {

Epoll::Epoll() : Poll(),
    m_epoll_fd(-1), m_max_event(1000), m_event_num(0),
    m_events(NULL) {
}

Epoll::~Epoll() {
    if (m_epoll_fd >= 0) {
        close(m_epoll_fd);
    }
    if (NULL != m_events) {
        m_event_num = 0;
        delete []m_events;
    }
}

int32_t Epoll::Init(uint32_t max_event) {
    m_max_event = static_cast<int32_t>(max_event);
    m_epoll_fd = epoll_create(m_max_event);
    m_events = new struct epoll_event[max_event];
    if (m_epoll_fd < 0) {
        ERR("epoll_create failed in %d", errno);
        return -1;
    }
    return 0;
}

int32_t Epoll::Wait(int32_t timeout) {
    m_event_num = epoll_wait(m_epoll_fd, m_events, m_max_event, timeout);
    return m_event_num;
}

int32_t Epoll::AddFd(int32_t fd, uint32_t events, uint64_t data) {
    struct epoll_event eve;
    eve.events = events;
    eve.data.u64 = data;
    return epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &eve);
}

int32_t Epoll::DelFd(int32_t fd) {
    return epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

int32_t Epoll::ModFd(int32_t fd, uint32_t events, uint64_t data) {
    struct epoll_event eve;
    eve.events = events;
    eve.data.u64 = data;
    return epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &eve);
}

int32_t Epoll::GetEvent(uint32_t *events, uint64_t *data) {
    if (m_event_num > 0) {
        --m_event_num;
        *events = m_events[m_event_num].events;
        *data = m_events[m_event_num].data.u64;
        if (NULL != m_bind_net_io) {
            m_bind_net_io->OnEvent(*data, *events);
        }
        return 0;
    }
    return -1;
}

}  // namespace xlib

#endif
