/*
 * Copyright [2018] <Copyright u35s>
 */

#ifndef XLIB_DAEMON_H_
#define XLIB_DAEMON_H_

namespace xlib {

const int DAEMON_ERR_NONE   = 0;
const int DAEMON_ERR_FORK   = 1;
const int DAEMON_ERR_SETSID = 2;
const int DAEMON_ERR_REDIRECT = 3;
const int DAEMON_ERR_DUP2     = 4;

int Daemon();

}  // namespace xlib

#endif  // XLIB_DAEMON_H_
