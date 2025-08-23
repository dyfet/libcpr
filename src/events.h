// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_EVENTS_H
#define CPR_EVENTS_H

#ifndef _WIN32
#include <stdbool.h>

typedef struct {
    int fds[2];
} event_t;

bool cpr_initevt(event_t *evt);
void cpr_freeevt(event_t *evt);
bool cpr_setevt(event_t *evt);
bool cpr_clearevt(event_t *evt);
bool cpr_waitevt(event_t *evt, int timeout);

#endif
#endif
