// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_MEMORY_H
#define CPR_MEMORY_H

#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef __STDC_NO_ATOMICS__
#include <stdatomic.h>
#endif

#include "strchar.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _memshare {
#ifndef __STDC_NO_ATOMICS__
    atomic_uint refcount;
#else
    unsigned refcount;
#endif
} *memshare_t;

char *cpr_hexdup(const uint8_t *bin, size_t size);
void *cpr_new(void **ptr, size_t size);
void cpr_free(void **ptr);
void *cpr_memset(void *ptr, int value, size_t size);
memshare_t cpr_makeref(size_t size);
void *cpr_ref(memshare_t ptr);
unsigned cpr_count(memshare_t ptr);
memshare_t cpr_retain(memshare_t ptr);
memshare_t cpr_release(memshare_t ptr);

#define NEWP(ptr, T) (T *)cpr_new((void **)&ptr, sizeof(T))
#define FREEP(ptr) cpr_free((void **)&ptr)
#define PAGER(mem, T) (T *)pager_alloc(mem, sizeof(T))
#define MAKE_PAGER(T, max) pager_create(sizeof(T), max)
#define FREE_PAGER(ptr) pager_free(ptr)
#define REF(ptr, T) (T *)cpr_ref(ptr)
#define MAKE_REF(ptr, T) (*ptr == NULL ? *ptr = cpr_makeref(sizeof(T)) : cpr_retain(*ptr))
#define RELEASE_REF(ptr) (*ptr = cpr_release(*ptr))

#ifdef __cplusplus
}
#endif
#endif
