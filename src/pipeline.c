// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#include "pipeline.h"

#include <stdlib.h>

pipeline_t *make_pipeline(size_t size, int policy) {
    pipeline_t *pl = malloc(sizeof(pipeline_t) + (size * sizeof(void *)));
    if (!pl) return NULL;
    mtx_init(&pl->lock, mtx_plain);
    cnd_init(&pl->input);
    cnd_init(&pl->output);
    pl->head = pl->tail = pl->count = 0;
    pl->size = size;
    pl->policy = policy;
    atomic_init(&pl->closed, false);
    return pl;
}

void free_pipeline(pipeline_t *pl) {
    if (!pl) return;
    close_pipeline(pl);
    mtx_destroy(&pl->lock);
    cnd_destroy(&pl->input);
    cnd_destroy(&pl->output);
    free(pl);
}

void close_pipeline(pipeline_t *pl) {
    if (!pl) return;
    if(atomic_exchange(&pl->closed, true)) return;
    cnd_broadcast(&pl->input);
    cnd_broadcast(&pl->output);
    thrd_yield();
    mtx_lock(&pl->lock);
    while (pl->count) {
        void *out = pl->buf[pl->head];
        if(out) free(out);
        pl->head = (pl->head + 1) % pl->size;
        --pl->count;
    }
    mtx_unlock(&pl->lock);
}

void *get_pipeline(pipeline_t *pl) {
    if (!pl) return NULL;
    mtx_lock(&pl->lock);
    while (!atomic_load(&pl->closed)) {
        if (pl->count > 0) {
            void *out = pl->buf[pl->head];
            pl->head = (pl->head + 1) % pl->size;
            if (pl->count-- == pl->size)
                cnd_signal(&pl->input);
            mtx_unlock(&pl->lock);
            return out;
        }
        cnd_wait(&pl->output, &pl->lock);
    }
    mtx_unlock(&pl->lock);
    return NULL;
}

bool put_pipeline(pipeline_t *pl, void *ptr) {
    if (!pl) return false;
    mtx_lock(&pl->lock);
    while (!atomic_load(&pl->closed)) {
        if (pl->count < pl->size) {
            pl->buf[pl->tail] = ptr;
            pl->tail = (pl->tail + 1) % pl->size;
            if (pl->count++ == 0)
                cnd_signal(&pl->output);
            mtx_unlock(&pl->lock);
            return true;
        }
        if (pl->policy == WAIT)
            cnd_wait(&pl->input, &pl->lock);
        else {
            void *drop = pl->buf[pl->head];
            if (drop)
                free(drop);
            pl->head = (pl->head + 1) % pl->size;
            --pl->count;
        }
    }
    mtx_unlock(&pl->lock);
    return false;
}
