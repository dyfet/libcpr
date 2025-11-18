// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#undef  NDEBUG
#include <assert.h>
#include "../src/clock.h"

#include <stdio.h>

int main(int argc, char **argv) {
    struct timespec ts, t1, t2;
    assert(cpr_gettime(&ts) == true);
    t2 = t1 = ts;
    cpr_addmsec(&ts, 1200);
    cpr_addmsec(&t1, 250);
    cpr_addmsec(&t2, 300);
    long remains = cpr_expires(&ts, NULL);
    assert(remains > 1000 && remains <= 1200);
    printf("%ld\n", remains);
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    cpr_monotonic(&cond);
    pthread_mutex_init(&mtx, NULL);
    cpr_timed(&cond, &mtx, &t1);
    remains = cpr_expires(&ts, NULL);
    assert(remains >= 500 && remains <= 950);
    printf("%ld\n", remains);
    cpr_until(&t2);
    cpr_timed(&cond, &mtx, &t1);
    long remains1 = cpr_expires(&ts, NULL);
    printf("%ld\n", remains1);
    assert(remains1 <= remains - 40);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mtx);
}

