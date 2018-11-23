/*
 * Copyright [2018] <Copyright u35s>
 */

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (__NetBSD__)

#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include "xlib/net_kqueue.h"
#include "xlib/log.h"

namespace xlib {

Kqueue::Kqueue() : Poll(),
    m_kqueue_fd(-1), m_max_event(1000), m_event_num(0),
    m_events(NULL) {
}

Kqueue::~Kqueue() {
    if (m_kqueue_fd >= 0) {
        close(m_kqueue_fd);
    }
    if (NULL != m_events) {
        m_event_num = 0;
        delete []m_events;
    }
}

int32_t Kqueue::Init(uint32_t max_event) {
    m_max_event = static_cast<int32_t>(max_event);
    m_kqueue_fd = kqueue();
    m_events = new struct kevent[max_event];
    if (m_kqueue_fd < 0) {
        return -1;
    }
    return 0;
}

int32_t Kqueue::Wait(int32_t timeout) {
    m_event_num = kevent(m_kqueue_fd, NULL, 0, m_events, m_max_event, NULL);
    return m_event_num;
}

int32_t Kqueue::AddFd(int32_t fd, uint32_t events, uint64_t data) {
    return ModFd(fd, events, data);
}

int32_t Kqueue::DelFd(int32_t fd) {
    struct kevent ev;
    EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(m_kqueue_fd, &ev, 1, NULL, 0, NULL);
    EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    kevent(m_kqueue_fd, &ev, 1, NULL, 0, NULL);
    return 0;
}

int32_t Kqueue::ModFd(int32_t fd, uint32_t events, uint64_t data) {
    struct kevent ev[2];
    int n = 0;
    if (events & POLLIN) {
        EV_SET(&ev[n++], fd, EVFILT_READ, EV_ADD|EV_ENABLE, 0, 0, (void*)(data));
    } else {
        EV_SET(&ev[n++], fd, EVFILT_READ, EV_DELETE, 0, 0, (void*)(data));
    }
    if (events & POLLOUT) {
        EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_ADD|EV_ENABLE, 0, 0, (void*)(data));
    } else {
        EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, (void*)(data));
    }
    if (kevent(m_kqueue_fd, ev, n, NULL, 0, NULL) == -1 ||  ev[0].flags & EV_ERROR) {
        return 1;
    }
    return 0;
}

int32_t Kqueue::GetEvent(uint32_t *event, uint64_t *data) {
    if (m_event_num > 0) {
        --m_event_num;
        *data = uint64_t(m_events[m_event_num].udata);

        bool eof = (m_events[m_event_num].flags & EV_EOF) != 0;
        uint64_t filter = m_events[m_event_num].filter;

        uint32_t new_event = 0;
        new_event = (filter == EVFILT_WRITE) && (!eof) ? new_event | POLLOUT : new_event;
        new_event = (filter == EVFILT_READ)  && (!eof) ? new_event | POLLIN  : new_event;
        new_event = (filter == EV_ERROR)     && (!eof) ? new_event | POLLERR : new_event;
        new_event = eof                                ? new_event | POLLERR : new_event;

        *event = new_event;

        if (NULL != m_bind_net_io) {
            m_bind_net_io->OnEvent(*data, *event);
        }
        return 0;
    }
    return -1;
}

}  // namespace xlib

#endif
