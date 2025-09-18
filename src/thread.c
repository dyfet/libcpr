// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#include "thread.h"

void cor_condlock_init(cpr_condlock_t *lock, unsigned max) {
    if (!lock) return;
    lock->pending = 0;
    lock->waiting = 0;
    lock->sharing = 0;
    mtx_init(&lock->mtx, mtx_plain);
    cnd_init(&lock->bcast);
}

void cor_condlock_free(cpr_condlock_t *lock) {
    if (!lock) return;
    cnd_destroy(&lock->bcast);
    mtx_destroy(&lock->mtx);
}

void cor_condlock_access(cpr_condlock_t *lock) {
    if (!lock) return;
    mtx_lock(&lock->mtx);
    while (lock->pending) {
        ++lock->waiting;
        cnd_wait(&lock->bcast, &lock->mtx);
        --lock->waiting;
    }
    ++lock->sharing;
    mtx_unlock(&lock->mtx);
}

void cpr_condlock_release(cpr_condlock_t *lock) {
    if (!lock) return;
    mtx_lock(&lock->mtx);
    if (lock->sharing) {
        --lock->sharing;
        if (lock->pending && !lock->sharing)
            cnd_signal(&lock->bcast);
        else if (lock->waiting && !lock->pending)
            cnd_broadcast(&lock->bcast);
    }
    mtx_unlock(&lock->mtx);
}

void cpr_condlock_modify(cpr_condlock_t *lock) {
    if (!lock) return;
    mtx_lock(&lock->mtx);
    while (lock->sharing) {
        ++lock->pending;
        cnd_wait(&lock->bcast, &lock->mtx);
        --lock->pending;
    }
}

void cpr_condlock_commit(cpr_condlock_t *lock) {
    if (!lock) return;
    if (lock->pending)
        cnd_signal(&lock->bcast);
    else if (lock->waiting)
        cnd_signal(&lock->bcast);
    mtx_unlock(&lock->mtx);
}
