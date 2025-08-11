// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_BUFIO_H
#define CPR_BUFIO_H

#include "string.h"

#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int fd;
    struct termios tty;
    size_t bufsize;
    size_t start;
    size_t end;
    char buf[2]; // extra byte to zero end of buf in fetch
} bufio_t;

bufio_t *cpr_makebuf(int fd, size_t bufsize);
ssize_t cpr_readbuf(bufio_t *r, char *out, size_t maxlen, const char *delim);
const char *cpr_fetchbuf(bufio_t *r, size_t *outlen, const char *delim);
int cpr_waitbuf(const bufio_t *r, int timeout_ms);
void cpr_freebuf(bufio_t *r);

#ifdef __cplusplus
}
#endif
#endif
