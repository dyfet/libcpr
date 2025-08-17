// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#include "memory.h"

char *cpr_hexdup(const uint8_t *bin, size_t size) {
    static const char *hex = "0123456789abcdef";
    if (!bin) return NULL;
    char *out = malloc((size * 2) + 1);
    if (!out) return NULL;
    char *put = out;
    while (size--) {
        *(put++) = hex[*bin >> 4];
        *(put++) = hex[*bin & 0x0f];
        ++bin;
    }
    *put = 0;
    return out;
}

void *cpr_new(void **ptr, size_t size) {
    if (*ptr)
        free(*ptr);
    *ptr = malloc(size);
    return *ptr;
}

void cpr_free(void **ptr) {
    if (!*ptr)
        return;
    free(*ptr);
    *ptr = NULL;
}

void *cpr_memset(void *ptr, int value, size_t size) {
    volatile uint8_t *volatile p = (volatile uint8_t *volatile)ptr;
    if (!p) return NULL;
    while (size--) {
        *p++ = (uint8_t)value;
    }
    return ptr;
}

memshare_t cpr_makeref(size_t size) {
    memshare_t ptr = (memshare_t)malloc(sizeof(struct _memshare) + size);
    if (!ptr) return NULL;
#ifndef __STDC_NO_ATOMICS__
    atomic_init(&ptr->refcount, 1);
#else
    ptr->refcount = 1;
#endif
    return ptr;
}

void *cpr_ref(memshare_t ptr) {
    return ((uint8_t *)ptr) + sizeof(struct _memshare);
}

unsigned cpr_count(memshare_t ptr) {
#ifndef __STDC_NO_ATOMICS__
    return atomic_load(&ptr->refcount);
#else
    return ptr->refcount;
#endif
}

memshare_t cpr_retain(memshare_t ptr) {
#ifndef __STDC_NO_ATOMICS__
    atomic_fetch_add(&ptr->refcount, 1);
#else
    ++ptr->refcount;
#endif
    return ptr;
}

memshare_t cpr_release(memshare_t ptr) {
#ifndef __STDC_NO_ATOMICS__
    if (atomic_fetch_sub(&ptr->refcount, 1) == 1) {
        free(ptr);
        return NULL;
    }
#else
    if (!--ptr->refcount) {
        free(ptr);
        return NULL;
    }
#endif
    return ptr;
}
