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
    size_t put;
    char buf[2]; // extra byte to zero end of buf in fetch
} bufio_t;

bufio_t *cpr_makebuf(int fd, size_t bufsize);
const char *cpr_lgetbuf(bufio_t *r, size_t *outlen, const char *delim);
const void *cpr_xgetbuf(bufio_t *r, size_t request);
void cpr_packbuf(bufio_t *r);
bool cpr_flushbuf(bufio_t *w);
bool cpr_xputbuf(bufio_t *w, const void *data, size_t request);
bool cpr_sputbuf(bufio_t *w, const char *text);
int cpr_waitbuf(const bufio_t *r, int timeout_ms);
void cpr_freebuf(bufio_t *r);

#ifdef __cplusplus
}
#endif
#endif
