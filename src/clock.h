// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_CLOCK_H
#define CPR_CLOCK_H

#include "thread.h"

#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISO_DATETIME_FORMAT "%Y-%m-%dT%H:%M:%S%z"
#define LOG_DATETIME_FORMAT "%Y-%m-%d %H:%M:%S"

typedef struct timespec *cpr_timepoint_t;
typedef struct timespec *cpr_duration_t;
typedef pthread_cond_t cpr_monocond_t;

bool cpr_systime(cpr_timepoint_t tp, time_t when);
bool cpr_gettime(cpr_timepoint_t ts);
void cpr_addmsec(cpr_timepoint_t ts, long ms);
bool cpr_remains(const cpr_timepoint_t mono, cpr_duration_t rel);
long cpr_expires(const cpr_timepoint_t deadline, struct timeval *tv);
void cpr_elapsed(const cpr_timepoint_t from, cpr_duration_t since);
time_t cpr_time(const cpr_timepoint_t mono);
void cpr_monoinit(cpr_monocond_t *cond);
int cpr_monotimed(cpr_monocond_t *cond, mtx_t *mtx, cpr_timepoint_t tp);
int cpr_monosleep(pthread_cond_t *cond, mtx_t *mtx, cpr_timepoint_t tp);

#ifdef __cplusplus
}
#endif
#endif
