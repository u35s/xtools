/*
 * Copyright [2018] <Copyright u35s>
 */

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "xlib/daemon.h"

namespace xlib {

int Daemon() {
    // fork 返回两次
    // 父进程返回子进程ID
    // 子进程返回0
    switch (fork()) {
    case -1 :
        return DAEMON_ERR_FORK;
    case 0 :
        break;
    default :
        exit(0);
    }

    // 调用setsid()函数脱离控制终端和进程组，使该进程成为会话组长，并与原来的登录会话和进程组脱离。
    // 此时进程已经成为无终端的会话组长，但它可以重新申请打开一个控制终端。
    // 为了避免这种情况，可以通过使进程不再成为会话组长来禁止进程重新打开控制终端，
    // 这就需要第二次调用fork()函数。父进程（会话组长）退出，子进程继续执行，
    // 并不再拥有打开控制终端的能力。这一步在许多项目中省略
    if (setsid() == -1) {
        return DAEMON_ERR_SETSID;
    }

    // 文件权限掩码是指屏蔽掉文件权限中的对应位。比如，有个文件权限掩码是050，它就屏蔽了文件组拥有者的可读与可执行权限。
    // 由于使用fork函数新建的子进程继承了父进程的文件权限掩码，这就给该子进程使用文件带来了诸多的麻烦。
    // 因此，把文件权限掩码设置为0，可以大大增强该守护进程的灵活性。设置文件权限掩码的函数是umask。在这里，通常的使用方法为umask(0)。
    umask(0);

    // 这一步也是必要的步骤。使用fork创建的子进程继承了父进程的当前工作目录。由于在进程运行中，
    // 当前目录所在的文件系统（如"/mnt/usb"）是不能卸载的，这对以后的使用会造成诸多的麻烦（比如系统由于某种原因要进入单用户模式）。
    // 因此，通常的做法是让"/"作为守护进程的当前工作目录，这样就可以避免上述的问题，
    // 当然，如有特殊需要，也可以把当前工作目录换成其他的路径，如/tmp。改变工作目录的常见函数是chdir。
    chdir("/");


    // 同文件权限码一样，用fork函数新建的子进程会从父进程那里继承一些已经打开了的文件。这些被打开的文件可能永远不会被守护进程读写，
    // 但它们一样消耗系统资源，而且可能导致所在的文件系统无法卸下。
    // 在上面的第二步之后，守护进程已经与所属的控制终端失去了联系。因此从终端输入的字符不可能达到守护进程，
    // 守护进程中用常规方法（如printf）输出的字符也不可能在终端上显示出来。
    // 所以，文件描述符为0、1和2 的3个文件（常说的输入、输出和报错）已经失去了存在的价值，也应被关闭。
    // 用重定向关闭可以防止0,1,2被重新利用而程序又调用fprintf(1,**)来写数据
    int redirect_fd = open("/dev/null", O_RDWR);
    if (redirect_fd == -1) {
         return DAEMON_ERR_REDIRECT;
    }

    if (dup2(redirect_fd, STDIN_FILENO)  == -1) {
        return DAEMON_ERR_DUP2;
    }
    if (dup2(redirect_fd, STDOUT_FILENO) == -1) {
        return DAEMON_ERR_DUP2;
    }
    if (dup2(redirect_fd, STDERR_FILENO) == -1) {
        return DAEMON_ERR_DUP2;
    }
    return DAEMON_ERR_NONE;
}

}  // namespace xlib
