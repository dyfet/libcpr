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
} tty_ctx;

bool cpr_ttyreset(tty_ctx *tty);
bool cpr_inittty(tty_ctx *tty, const char *path);
bool cpr_freetty(tty_ctx *tty);
int cpr_ttycanon(tty_ctx *tty, const char *nl);
int cpr_ttypacket(tty_ctx *tty, size_t size, uint8_t timer);

#ifdef __cplusplus
}
#endif
#endif
