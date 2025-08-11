// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#include "bufio.h"
#include "socket.h"

#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

void cpr_freebuf(bufio_t *r) {
    if (r->fd > -1 && isatty(r->fd)) {
        tcdrain(r->fd);
        tcsetattr(r->fd, TCSANOW, &r->tty);
    } else if (r->fd > -1) {
        struct stat ino;
        if (!fstat(r->fd, &ino) && S_ISSOCK(ino.st_mode))
            shutdown(r->fd, SHUT_RDWR);
        close(r->fd);
    }
    free(r);
}

bufio_t *cpr_makebuf(int fd, size_t bufsize) {
    if (fd < 0 || !bufsize) return NULL;
    bufio_t *r = malloc(sizeof(bufio_t) + bufsize);
    if (!r) return NULL;
    if (isatty(fd)) {
        struct termios t;
        if (tcgetattr(fd, &r->tty) < 0) {
            free(r);
            return NULL;
        }
        tcgetattr(fd, &t);
        t.c_lflag &= ~(ICANON | ECHO | ISIG);
        t.c_iflag &= ~(IXON | ICRNL);
        t.c_oflag &= ~(OPOST);
        t.c_cc[VMIN] = 1;  // read returns after 1 byte
        t.c_cc[VTIME] = 0; // no timeout
        if (tcsetattr(fd, TCSANOW, &t) < 0) {
            free(r);
            return NULL;
        }
    }

    r->fd = fd;
    r->bufsize = bufsize;
    r->start = r->end = 0;
    return r;
}

const char *cpr_fetchbuf(bufio_t *r, size_t *outlen, const char *delim) {
    if (!r) return NULL;
    if (delim == NULL) delim = "\n";
    size_t delim_len = strlen(delim);
    while (1) {
        for (size_t i = r->start; i + delim_len <= r->end; ++i) {
            if (memcmp(&r->buf[i], delim, delim_len) == 0) {
                const char *result = &r->buf[r->start];
                size_t len = i - r->start;

                if (outlen) {
                    *outlen = len;
                } else {
                    if (i + delim_len < r->bufsize + 1) {
                        r->buf[i + delim_len] = '\0';
                    } else {
                        return NULL;
                    }
                }

                r->start = i + delim_len;
                return result;
            }
        }

        if (r->start > 0 && r->start < r->end) {
            memmove(r->buf, &r->buf[r->start], r->end - r->start);
            r->end -= r->start;
            r->start = 0;
        } else if (r->start == r->end) {
            r->start = r->end = 0;
        }

        if (r->end >= r->bufsize) return NULL;
        ssize_t n = read(r->fd, &r->buf[r->end], r->bufsize - r->end);
        if (n <= 0) return NULL;
        r->end += n;
    }
}

ssize_t cpr_readbuf(bufio_t *r, char *out, size_t maxlen, const char *delim) {
    if (!r || !out || maxlen == 0) return -1;
    if (delim == NULL) delim = "\n";
    size_t delim_len = strlen(delim);
    size_t i = r->start;
    while (1) {
        // Search for delimiter in buffer
        for (; i + delim_len <= r->end; ++i) {
            if (memcmp(&r->buf[i], delim, delim_len) == 0) {
                size_t len = i - r->start;
                if (len >= maxlen) return -2; // output buffer too small

                memcpy(out, &r->buf[r->start], len);
                out[len] = '\0';

                r->start = i + delim_len;
                return len;
            }
        }

        // Shift unread data to front
        if (r->start > 0 && r->start < r->end) {
            memmove(r->buf, &r->buf[r->start], r->end - r->start);
            r->end -= r->start;
            r->start = 0;
        } else if (r->start == r->end) {
            r->start = r->end = 0;
        }

        // Check for buffer overflow
        if (r->end == r->bufsize) return -3; // buffer full, no delimiter

        // Read more data
        ssize_t n = read(r->fd, &r->buf[r->end], r->bufsize - r->end);
        if (n < 0) return -4; // read error
        if (n == 0) return 0; // EOF

        r->end += n;
        i = r->start;
    }
}

int cpr_waitbuf(const bufio_t *r, int timeout_ms) {
    if (!r || r->fd < 0) return -1;
    struct pollfd pfd = {
    .fd = r->fd,
    .events = POLLIN};

    while (1) {
        int rv = poll(&pfd, 1, timeout_ms);
        if (rv > 0) {
            if (pfd.revents & POLLIN) return 1; // ready to read
            if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) return -2;
            return -3; // unexpected state
        } else if (rv == 0) {
            return 0; // timeout
        } else if (errno == EINTR) {
            continue; // retry
        } else {
            return -4; // poll error
        }
    }
}
