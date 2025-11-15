// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#include "clock.h"
#include "thread.h"

#include <errno.h>

bool cpr_gettime(cpr_timepoint_t ts) {
    return (clock_gettime(CLOCK_MONOTONIC, ts) != 0) ? false : true;
}

bool cpr_systime(cpr_timepoint_t tp, time_t when) {
    time_t now;
    time(&now);
    when -= now;
    if (!cpr_gettime(tp)) return false;
    if (when >= 0) {
        tp->tv_sec += when;
        return true;
    }
    return false;
}

// timeval can be NULL if we just want to test expiration...
long cpr_expires(const cpr_timepoint_t deadline, struct timeval *tv) {
    if (!deadline) return true;
    struct timespec now, delta;
    clock_gettime(CLOCK_MONOTONIC, &now);
    delta.tv_sec = deadline->tv_sec - now.tv_sec;
    delta.tv_nsec = deadline->tv_nsec - now.tv_nsec;
    if (delta.tv_nsec < 0) {
        delta.tv_sec -= 1;
        delta.tv_nsec += 1000000000;
    }

    if (delta.tv_sec < 0 || (delta.tv_sec == 0 && delta.tv_nsec <= 0)) {
        if (tv) {
            tv->tv_sec = 0;
            tv->tv_usec = 0;
        }
        return 0;
    }

    if (tv) {
        tv->tv_sec = delta.tv_sec;
        tv->tv_usec = delta.tv_nsec / 1000;
    }
    return (delta.tv_sec * 1000) + (delta.tv_nsec / 1000000);
}

void cpr_addmsec(cpr_timepoint_t ts, long ms) {
    if (!ts) return;
    ts->tv_sec += ms / 1000;
    ts->tv_nsec += (ms % 1000) * 1000000;
    if (ts->tv_nsec >= 1000000000) {
        ts->tv_sec += 1;
        ts->tv_nsec -= 1000000000;
    } else if (ts->tv_nsec < 0) {
        ts->tv_sec -= 1;
        ts->tv_nsec += 1000000000L;
    }
}

time_t cpr_time(const cpr_timepoint_t mono) {
    struct timespec real_now, mono_now;
    clock_gettime(CLOCK_REALTIME, &real_now);
    clock_gettime(CLOCK_MONOTONIC, &mono_now);

    time_t offset = real_now.tv_sec - mono_now.tv_sec;
    return mono->tv_sec + offset;
}

void cpr_elapsed(const cpr_timepoint_t from, cpr_duration_t since) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    since->tv_sec = now.tv_sec - from->tv_sec;
    since->tv_nsec = now.tv_nsec - from->tv_nsec;

    if (since->tv_nsec < 0) {
        since->tv_nsec += 1000000000;
        since->tv_sec -= 1;
    }

    if (since->tv_sec < 0 || (since->tv_sec == 0 && since->tv_nsec <= 0)) {
        since->tv_sec = 0;
        since->tv_nsec = 0;
        return;
    }
}

bool cpr_remains(const cpr_timepoint_t mono, cpr_duration_t rel) {
    if (!mono || !rel) return false;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    rel->tv_sec = mono->tv_sec - now.tv_sec;
    rel->tv_nsec = mono->tv_nsec - now.tv_nsec;

    if (rel->tv_nsec < 0) {
        rel->tv_nsec += 1000000000;
        rel->tv_sec -= 1;
    }

    // Clamp to zero if already expired
    if (rel->tv_sec < 0 || (rel->tv_sec == 0 && rel->tv_nsec <= 0)) {
        rel->tv_sec = 0;
        rel->tv_nsec = 0;
        return false;
    }
    return true;
}

void cpr_monotonic(pthread_cond_t *cond) {
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    pthread_cond_init(cond, &attr);
    pthread_condattr_destroy(&attr);
}

int cpr_timed(pthread_cond_t *cond, pthread_mutex_t *mtx, cpr_timepoint_t tp) {
    return pthread_cond_timedwait(cond, mtx, tp);
}

int cpr_until(cpr_timepoint_t tp) {
    return clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, tp, NULL);
}
