// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_THREAD_H
#define CPR_THREAD_H

#if defined(_WIN32) || defined(__FreeBSD__)
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <sched.h>

typedef pthread_t thrd_t;
typedef int (*thrd_start_t)(void *);
typedef pthread_mutex_t mtx_t;
typedef pthread_cond_t cnd_t;

enum {
    thrd_success = 0,
    thrd_error = 1,
    thrd_nomem = 2,
    thrd_timedout = 3,
    thrd_busy = 4
};

typedef enum {
    mtx_plain = 0,
    mtx_timed = 1, // Not supported in this shim
    mtx_recursive = 2
} mtx_type;

static inline int thrd_create(thrd_t *thr, thrd_start_t func, void *arg) {
    return pthread_create(thr, NULL, (void *(*)(void *))func, arg) == 0 ? thrd_success : thrd_error;
}

static inline int thrd_join(thrd_t thr, int *res) {
    void *retval;
    if (pthread_join(thr, &retval) != 0)
        return thrd_error;
    if (res) *res = (int)(intptr_t)retval;
    return thrd_success;
}

static inline void thrd_yield(void) {
    sched_yield();
}

static inline int mtx_init(mtx_t *mtx, mtx_type type) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    if (type == mtx_recursive)
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    int ret = pthread_mutex_init(mtx, &attr);
    pthread_mutexattr_destroy(&attr);
    return ret == 0 ? thrd_success : thrd_error;
}

static inline void mtx_destroy(mtx_t *mtx) {
    pthread_mutex_destroy(mtx);
}

static inline int mtx_lock(mtx_t *mtx) {
    return pthread_mutex_lock(mtx) == 0 ? thrd_success : thrd_error;
}

static inline int mtx_unlock(mtx_t *mtx) {
    return pthread_mutex_unlock(mtx) == 0 ? thrd_success : thrd_error;
}

static inline int cnd_init(cnd_t *cond) {
    return pthread_cond_init(cond, NULL) == 0 ? thrd_success : thrd_error;
}

static inline void cnd_destroy(cnd_t *cond) {
    pthread_cond_destroy(cond);
}

static inline int cnd_wait(cnd_t *cond, mtx_t *mtx) {
    return pthread_cond_wait(cond, mtx) == 0 ? thrd_success : thrd_error;
}

static inline int cnd_signal(cnd_t *cond) {
    return pthread_cond_signal(cond) == 0 ? thrd_success : thrd_error;
}

static inline int cnd_broadcast(cnd_t *cond) {
    return pthread_cond_broadcast(cond) == 0 ? thrd_success : thrd_error;
}

#else
#include <threads.h>
#endif

typedef struct {
    mtx_t mtx;
    cnd_t bcast;
    unsigned pending, waiting, sharing;
} cpr_condlock_t;

void cor_condlock_init(cpr_condlock_t *lock, unsigned max);
void cor_condlock_free(cpr_condlock_t *lock);
void cor_condlock_access(cpr_condlock_t *lock);
void cpr_condlock_release(cpr_condlock_t *lock);
void cpr_condlock_modify(cpr_condlock_t *lock);
void cpr_condlock_commit(cpr_condlock_t *lock);

#endif
