// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#include "tty.h"

bool cpr_ttyreset(tty_ctx *tty) {
    if (!tty || !tty->fd) return -EBADF;
    tty->active.c_oflag = tty->active.c_lflag = 0;
    tty->active.c_cflag = CLOCAL | CREAD | HUPCL;
    tty->active.c_iflag = IGNBRK;

    memset(&tty->active.c_cc, 0, sizeof(tty->active.c_cc)); // NOLINT
    tty->active.c_cc[VMIN] = 1;

    tty->active.c_cflag |= tty->saved.c_cflag & (CRTSCTS | CSIZE | PARENB | PARODD | CSTOPB);
    tty->active.c_iflag |= tty->saved.c_iflag & (IXON | IXANY | IXOFF);
    return tcsetattr(tty->fd, TCSANOW, &tty->active) == 0;
}

bool cpr_inittty(tty_ctx *tty, const char *path) {
    if (!tty) return false;
    tty->fd = open(path, O_RDWR | O_NDELAY); // FlawFinder: ignore
    if (tty->fd < 0) return false;

    long ioflags = fcntl(tty->fd, F_GETFL);
    tcgetattr(tty->fd, &tty->active);
    tcgetattr(tty->fd, &tty->saved);
    cpr_ttyreset(tty);
    fcntl(tty->fd, F_SETFL, ioflags & ~O_NDELAY);

#if defined(TIOCM_RTS) && defined(TIOCMODG)
    int mcs = 0;
    ioctl(tty->fd, TIOCMODG, &mcs);
    mcs |= TIOCM_RTS;
    ioctl(tty->fd, TIOCMODS, &mcs);
#endif
    return true;
}

bool cpr_freetty(tty_ctx *tty) {
    if (!tty) return false;
    if (tty->fd < 0) return -EBADF;
    tcsetattr(tty->fd, TCSANOW, &tty->saved);
    close(tty->fd);
    return true;
}

int cpr_ttycanon(tty_ctx *tty, const char *nl) {
    char nl1 = 0, nl2 = 0;

    if (!tty || !tty->fd)
        return -EBADF;

    if (!nl)
        return -EINVAL;

    if (*nl) {
        nl1 = nl[0];
        nl2 = nl[1];
    }

    struct termios *attr = &tty->active;
    attr->c_cc[VMIN] = nl1 ? 0 : 1;
    attr->c_cc[VTIME] = 0;
    attr->c_cc[VEOL] = nl1;
    attr->c_cc[VEOL2] = nl2;
    attr->c_lflag |= ICANON;
    tcsetattr(tty->fd, TCSANOW, attr);

#ifdef _PC_MAX_CANON
    return fpathconf(tty->fd, _PC_MAX_CANON);
#else
    return MAX_CANON
#endif
}

int cpr_ttypacket(tty_ctx *tty, size_t size, uint8_t timer) {
    if (!tty || !tty->fd)
        return -EBADF;

#ifdef _PC_MAX_INPUT
    int max = fpathconf(tty->fd, _PC_MAX_INPUT);
#else
    int max = MAX_INPUT;
#endif

    if (size > (size_t)max)
        return -EINVAL;

    struct termios *attr = &tty->active;
    attr->c_cc[VEOL] = attr->c_cc[VEOL2] = 0;
    attr->c_cc[VMIN] = (uint8_t)size;
    attr->c_cc[VTIME] = timer;
    attr->c_lflag &= ~ICANON;
    tcsetattr(tty->fd, TCSANOW, attr);
    return size;
}

