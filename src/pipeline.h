// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_BUFIO_H
#define CPR_BUFIO_H

#include "clock.h"

typedef struct pipeline {
    enum { WAIT,
        DROP } policy;
    mtx_t lock;
    cnd_t input, output;
    size_t head, tail, count, size;
    void *buf[];
} pipeline_t;

pipeline_t *make_pipeline(size_t size, int policy);
void free_pipeline(pipeline_t *pl);
void *get_pipeline(pipeline_t *pl);
void put_pipeline(pipeline_t *pl, void *ptr);
#endif
