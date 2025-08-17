// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_WAITGROUP_H
#define CPR_WAITGROUP_H

#include "clock.h"

typedef struct {
    size_t count;
    mtx_t lock;
    cnd_t cond;
} waitgroup_t;

bool cpr_initwg(waitgroup_t *wg);
void cpr_freewg(waitgroup_t *wg);
void cpr_addwg(waitgroup_t *wg, size_t add);
bool cpr_donewg(waitgroup_t *wg);
void cpr_waitwg(waitgroup_t *wg);
#endif
