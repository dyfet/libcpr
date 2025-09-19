// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_STRING_H
#define CPR_STRING_H

#include "strchar.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _string {
    uint16_t size, used;
} *string_t;

void free_string(string_t *str);
string_t new_string(size_t size);
size_t string_size(string_t str);
char *string_get(const string_t str);
size_t string_commit(string_t str);
size_t string_count(const string_t str);
void string_clear(string_t str);
string_t string_clone(const string_t str);
string_t string_copy(string_t str, const char *val);
string_t string_append(string_t str, const char *val);
string_t make_string(const char *val, size_t max);
string_t string_upper(string_t str);
string_t string_lower(string_t str);
string_t string_trim(string_t str, const char *trim);
string_t string_chop(string_t str, const char *chop);
string_t string_tail(string_t str, size_t size);
string_t string_head(string_t str, size_t size);
int string_scanf(string_t str, const char *format, ...);
int string_printf(string_t str, const char *format, ...);

#define STR(str) string_get(str)

#ifdef __cplusplus
}
#endif
#endif
