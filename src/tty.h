// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_TTY_H
#define CPR_TTY_H

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <termios.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct termios saved, active;
    int fd;
} TTY;

static inline int tty_reset(TTY *tty) {
    if (!tty || !tty->fd)
        return -EBADF;

    tty->active.c_oflag = tty->active.c_lflag = 0;
    tty->active.c_cflag = CLOCAL | CREAD | HUPCL;
    tty->active.c_iflag = IGNBRK;

    memset(&tty->active.c_cc, 0, sizeof(tty->active.c_cc)); // NOLINT
    tty->active.c_cc[VMIN] = 1;

    tty->active.c_cflag |= tty->saved.c_cflag & (CRTSCTS | CSIZE | PARENB | PARODD | CSTOPB);
    tty->active.c_iflag |= tty->saved.c_iflag & (IXON | IXANY | IXOFF);

    return tcsetattr(tty->fd, TCSANOW, &tty->active);
}

static inline TTY *tty_open(const char *path) {
    TTY *tty = (TTY *)malloc(sizeof(TTY));
    if (!tty)
        return NULL;

    tty->fd = open(path, O_RDWR | O_NDELAY); // FlawFinder: ignore
    if (tty->fd < 0) {
        free(tty);
        return NULL;
    }

    long ioflags = fcntl(tty->fd, F_GETFL);
    tcgetattr(tty->fd, &tty->active);
    tcgetattr(tty->fd, &tty->saved);
    tty_reset(tty);
    fcntl(tty->fd, F_SETFL, ioflags & ~O_NDELAY);

#if defined(TIOCM_RTS) && defined(TIOCMODG)
    int mcs = 0;
    ioctl(tty->fd, TIOCMODG, &mcs);
    mcs |= TIOCM_RTS;
    ioctl(tty->fd, TIOCMODS, &mcs);
#endif
    return tty;
}

static inline int tty_close(TTY *tty) {
    if (tty->fd < 0)
        return -EBADF;

    tcsetattr(tty->fd, TCSANOW, &tty->saved);
    close(tty->fd);
    free(tty);
    return 0;
}

static inline int tty_canoninput(TTY *tty, const char *nl) {
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

static inline int tty_timedinput(TTY *tty, size_t size, uint8_t timer) {
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

static inline int tty_drain(TTY *tty) {
    if (!tty || tty->fd < 0)
        return -EBADF;

    return tcdrain(tty->fd);
}

static inline int tty_flush(TTY *tty) {
    if (!tty || tty->fd < 0)
        return -EBADF;

    return tcflush(tty->fd, TCOFLUSH);
}

static inline int tty_purge(TTY *tty) {
    if (!tty || tty->fd < 0)
        return -EBADF;

    return tcflush(tty->fd, TCIFLUSH);
}

static inline int tty_break(TTY *tty) {
    if (!tty || tty->fd < 0)
        return -EBADF;

    return tcsendbreak(tty->fd, 0);
}

static inline int tty_dtrtoggle(TTY *tty, long msec) {
    if (!tty || tty->fd < 0)
        return -EBADF;

    struct termios now, old;
    tcgetattr(tty->fd, &now);
    tcgetattr(tty->fd, &old);
    cfsetospeed(&now, B0);
    cfsetispeed(&now, B0);
    tcsetattr(tty->fd, TCSANOW, &now);

    if (msec) {
        usleep(msec * 1000L); // FlawFinder: ignore
        return tcsetattr(tty->fd, TCSANOW, &old);
    }
    return 0;
}

static inline int tty_parity(TTY *tty, bool odd, bool even) {
    if (!tty || tty->fd < 0)
        return -EBADF;

    if (odd && even)
        return -EINVAL;

    struct termios *attr = &tty->active;
    attr->c_cflag &= ~(PARENB | PARODD);
    if (odd)
        attr->c_cflag |= (PARENB | PARODD);
    else if (even)
        attr->c_cflag |= PARENB;

    return tcsetattr(tty->fd, TCSANOW, attr);
}

static inline int tty_bitsize(TTY *tty, int data, int stop) {
    if (!tty || tty->fd < 0)
        return -EBADF;

    if (stop < 1 || stop > 2 || data < 5 || data > 8)
        return -EINVAL;

    struct termios *attr = &tty->active;
    attr->c_cflag &= ~(CSTOPB | CSIZE);
    attr->c_cflag |= (stop == 2) ? CSTOPB : 0;
    switch (data) {
    case 5:
        attr->c_cflag |= CS5;
        break;
    case 6:
        attr->c_cflag |= CS6;
        break;
    case 7:
        attr->c_cflag |= CS7;
        break;
    case 8:
        attr->c_cflag |= CS8;
        break;
    default:
        return -EINVAL;
    }

    return tcsetattr(tty->fd, TCSANOW, attr);
}

static inline int tty_flowcontrol(TTY *tty, bool hard, bool soft) {
    if (!tty || tty->fd < 0)
        return -EBADF;

    struct termios *attr = &tty->active;
    attr->c_cflag &= ~CRTSCTS;
    attr->c_iflag &= ~(IXON | IXANY | IXOFF);

    if (hard)
        attr->c_cflag |= CRTSCTS;

    if (soft)
        attr->c_iflag |= (IXON | IXANY | IXOFF);

    return tcsetattr(tty->fd, TCSANOW, attr);
}

static inline int tty_speed(TTY *tty, unsigned long speed) {
    if (!tty || tty->fd < 0)
        return -EBADF;

    unsigned long rate;
    switch (speed) {
#ifdef B115200
    case 115200:
        rate = B115200;
        break;
#endif
#ifdef B57600
    case 57600:
        rate = B57600;
        break;
#endif
#ifdef B38400
    case 38400:
        rate = B38400;
        break;
#endif
    case 19200:
        rate = B19200;
        break;
    case 9600:
        rate = B9600;
        break;
    case 4800:
        rate = B4800;
        break;
    case 2400:
        rate = B2400;
        break;
    case 1200:
        rate = B1200;
        break;
    case 600:
        rate = B600;
        break;
    case 300:
        rate = B300;
        break;
    case 110:
        rate = B110;
        break;
#ifdef B0
    case 0:
        rate = B0;
        break;
#endif
    default:
        return -EINVAL;
    }

    struct termios *attr = &tty->active;
    cfsetispeed(attr, rate);
    cfsetospeed(attr, rate);
    tcsetattr(tty->fd, TCSANOW, attr);
    return 0;
}

static inline ssize_t tty_read(TTY *tty, void *data, size_t size) {
    if (!tty || tty->fd < 0)
        return -EBADF;

    return read(tty->fd, data, size); // FlawFinder: ignore
}

static inline ssize_t tty_write(TTY *tty, const void *data, size_t size) {
    if (!tty || tty->fd < 0)
        return -EBADF;

    return write(tty->fd, data, size);
}

#ifdef __cplusplus
}
#endif
#endif
