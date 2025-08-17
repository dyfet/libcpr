// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#include "waitgroup.h"

#include <stdlib.h>

bool cpr_initwg(waitgroup_t *wg) {
    if (!wg) return false;
    wg->count = 0;
    mtx_init(&wg->lock, mtx_plain);
    cnd_init(&wg->cond);
    return true;
}

void cpr_freewg(waitgroup_t *wg) {
    if (!wg) return;
    mtx_destroy(&wg->lock);
    cnd_destroy(&wg->cond);
    wg->count = 0;
}

void cpr_waitwg(waitgroup_t *wg) {
    if (!wg) return;
    mtx_lock(&wg->lock);
    while (wg->count > 0)
        cnd_wait(&wg->cond, &wg->lock);
    mtx_unlock(&wg->lock);
}

void cpr_addwg(waitgroup_t *wg, size_t add) {
    if (!wg) return;
    mtx_lock(&wg->lock);
    wg->count += add;
    mtx_unlock(&wg->lock);
}

bool cpr_donewg(waitgroup_t *wg) {
    if (!wg) return false;
    mtx_lock(&wg->lock);
    if (wg->count == 0) {
        mtx_unlock(&wg->lock);
        return false;
    }
    if (--wg->count == 0) {
        cnd_broadcast(&wg->cond);
        mtx_unlock(&wg->lock);
        return true;
    }
    mtx_unlock(&wg->lock);
    return false;
}
