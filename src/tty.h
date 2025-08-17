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
    int fd, out;
    int error;
    char echo;
} tty_ctx;

bool tty_console(tty_ctx *tty, char echo);
bool tty_reset(tty_ctx *tty);
bool tty_init(tty_ctx *tty, const char *path, char echo);
bool tty_free(tty_ctx *tty);
int tty_canon(tty_ctx *tty, const char *nl);
int tty_packet(tty_ctx *tty, size_t size, uint8_t timer);
bool tty_putch(tty_ctx *tty, char ch);
char tty_getch(tty_ctx *ctx);
ssize_t tty_putline(tty_ctx *ctx, const char *str);
ssize_t tty_getline(tty_ctx *ctx, char *buf, size_t max, char eol);

#ifdef __cplusplus
}
#endif
#endif
