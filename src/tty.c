// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef _WIN32
#include "tty.h"
#include "memory.h"

#include <errno.h>

static bool tty_setup(tty_ctx *tty, char echo) {
    if (!tty) return false;
    long ioflags = fcntl(tty->fd, F_GETFL);
    tcgetattr(tty->fd, &tty->active);
    tcgetattr(tty->fd, &tty->saved);
    tty_reset(tty);
    fcntl(tty->fd, F_SETFL, ioflags & ~O_NDELAY);
    tty->echo = echo;
    tty->error = 0;

#if defined(TIOCM_RTS) && defined(TIOCMODG)
    int mcs = 0;
    ioctl(tty->fd, TIOCMODG, &mcs);
    mcs |= TIOCM_RTS;
    ioctl(tty->fd, TIOCMODS, &mcs);
#endif
    return true;
}

bool tty_reset(tty_ctx *tty) {
    if (!tty || !tty->fd) return -EBADF;
    tty->active.c_oflag = tty->active.c_lflag = 0;
    tty->active.c_cflag = CLOCAL | CREAD | HUPCL;
    tty->active.c_iflag = IGNBRK;

    cpr_memset(&tty->active.c_cc, 0, sizeof(tty->active.c_cc));
    tty->active.c_cc[VMIN] = 1;

    tty->active.c_cflag |= tty->saved.c_cflag & (CRTSCTS | CSIZE | PARENB | PARODD | CSTOPB);
    tty->active.c_iflag |= tty->saved.c_iflag & (IXON | IXANY | IXOFF);
    return tcsetattr(tty->fd, TCSANOW, &tty->active) == 0;
}

bool tty_console(tty_ctx *tty, char echo) {
    if (!tty || !isatty(0)) return false;
    tty->fd = 0;
    if (isatty(1))
        tty->out = 1;
    else
        tty->out = -1;
    return tty_setup(tty, echo);
}

bool tty_init(tty_ctx *tty, const char *path, char echo) {
    if (!tty) return false;
    tty->fd = tty->out = open(path, O_RDWR | O_NDELAY); // FlawFinder: ignore
    if (tty->fd < 0) return false;
    return tty_setup(tty, echo);
}

bool tty_free(tty_ctx *tty) {
    if (!tty) return false;
    if (tty->fd < 0) return -EBADF;
    tcsetattr(tty->fd, TCSANOW, &tty->saved);
    if (tty->fd > 2)
        close(tty->fd);
    return true;
}

int tty_canon(tty_ctx *tty, const char *nl) {
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
    return (int)fpathconf(tty->fd, _PC_MAX_CANON);
#else
    return MAX_CANON
#endif
}

int tty_packet(tty_ctx *tty, size_t size, uint8_t timer) {
    if (!tty || !tty->fd)
        return -EBADF;

#ifdef _PC_MAX_INPUT
    long max = fpathconf(tty->fd, _PC_MAX_INPUT);
#else
    long max = MAX_INPUT;
#endif

    if (size > (size_t)max)
        return -EINVAL;

    struct termios *attr = &tty->active;
    attr->c_cc[VEOL] = attr->c_cc[VEOL2] = 0;
    attr->c_cc[VMIN] = (uint8_t)size;
    attr->c_cc[VTIME] = timer;
    attr->c_lflag &= ~ICANON;
    tcsetattr(tty->fd, TCSANOW, attr);
    return (int)size;
}

bool tty_putch(tty_ctx *ctx, char ch) {
    if (!ctx) return false;
    ctx->error = 0;
    ssize_t rtn = write(ctx->out, &ch, 1);
    if (rtn < 0) ctx->error = errno;
    return rtn == 1;
}

char tty_getch(tty_ctx *ctx) {
    if (!ctx) return 0;
    ctx->error = 0;
    char ch;
    ssize_t rtn = read(ctx->fd, &ch, 1); // FlawFinder: valid
    if (rtn < 0) ctx->error = errno;
    if (rtn != 1) return 0;
    if (ctx->echo > 1 && ch > 31 && ch != 127) {
        if (!tty_putch(ctx, ctx->echo))
            return 0;
    } else if (ctx->echo) {
        if (!tty_putch(ctx, ch))
            return 0;
    }
    return ch;
}

ssize_t tty_putline(tty_ctx *ctx, const char *str) {
    if (!str || !ctx) return -1;
    ctx->error = 0;
    ssize_t rtn = write(ctx->out, str, cpr_strlen(str, 256));
    if (rtn < 0) ctx->error = errno;
    return rtn;
}

ssize_t tty_getline(tty_ctx *ctx, char *buf, size_t max, char eol) {
    if (!buf || !ctx || max < 1) return -1;
    *buf = 0;
    --max;

    size_t count = 0;
    while (count < max) {
        char ch = tty_getch(ctx);
        if (ch == 0 || ch == eol) break;
        buf[count++] = ch;
    }
    return (ssize_t)count;
}
#endif
