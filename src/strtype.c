// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#include "strtype.h"
#include "strchar.h"
#include "memory.h"

void free_string(string_t *str) {
    if (str && *str)
        free(*str);

    if (str)
        *str = NULL;
}

string_t new_string(size_t size) {
    string_t str = (string_t)malloc(size + sizeof(struct _string));
    if (!str)
        return NULL;

    str->size = (uint16_t)size;
    str->used = 0;
    return str;
}

size_t string_size(string_t str) {
    if (!str)
        return 0;

    return str->size;
}

char *string_get(const string_t str) {
    if (!str)
        return NULL;

    return ((char *)str) + sizeof(struct _string);
}

size_t string_commit(string_t str) {
    if (!str)
        return 0;

    return (str->used = (uint16_t)cpr_strlen(string_get(str), str->size - 1));
}

size_t string_count(const string_t str) {
    if (!str)
        return 0;

    return str->used;
}

void string_clear(string_t str) {
    if (!str)
        return;

    str->used = 0;
    *string_get(str) = 0;
}

string_t string_clone(const string_t str) {
    if (!str)
        return NULL;

    size_t size = str->size + sizeof(struct _string);
    string_t dup = (string_t)malloc(size);
    if (!dup) return NULL;
    cpr_memcpy(dup, size, str, size);
    return dup;
}

string_t string_copy(string_t str, const char *val) {
    if (!str)
        return NULL;

    size_t len = cpr_strlen(val, str->size - 1);
    char *mem = string_get(str);
    if (mem == val) {
        str->used = (uint16_t)len;
        mem[len] = 0;
        return str;
    }

    str->used = (uint16_t)len;
    while (len--)
        *(mem++) = *(val++);
    *mem = 0;
    return str;
}

string_t string_append(string_t str, const char *val) {
    if (str->used >= (str->size - 1))
        return str;

    char *mem = string_get(str) + str->used;
    size_t size = string_size(str) - str->used - 1;
    size_t len = cpr_strlen(val, size);
    if (len)
        cpr_memcpy(mem, size, val, len);

    mem[len] = 0;
    str->used += (uint16_t)len;
    return str;
}

string_t make_string(const char *val, size_t max) {
    size_t size = cpr_strlen(val, max) + 1;
    string_t str = new_string(size);
    if (!str)
        return NULL;

    return string_copy(str, val);
}

string_t string_upper(string_t str) {
    char *mem = string_get(str);
    while (*mem) {
        *mem = (char)toupper(*mem);
        ++mem;
    }
    return str;
}

string_t string_lower(string_t str) {
    char *mem = string_get(str);
    while (*mem) {
        *mem = (char)tolower(*mem);
        ++mem;
    }
    return str;
}

string_t string_trim(string_t str, const char *trim) {
    if (!str)
        return NULL;

    return string_copy(str, cpr_strtrim(string_get(str), trim, str->size));
}

string_t string_chop(string_t str, const char *chop) {
    if (!str)
        return NULL;

    return string_copy(str, cpr_strchop(string_get(str), chop, str->size));
}

string_t string_tail(string_t str, size_t size) {
    if (!str)
        return NULL;

    if (size > str->used)
        size = str->used;

    *(string_get(str) + size) = 0;
    str->used = (uint16_t)size;
    return str;
}

string_t string_head(string_t str, size_t size) {
    if (!str)
        return NULL;

    if (size > str->used)
        size = str->used;

    return string_copy(str, string_get(str) + (str->used - size));
}

int string_scanf(string_t str, const char *format, ...) {
    if (!str)
        return 0;

    va_list args;
    va_start(args, format);

    // FlawFinder: ignore
    int out = vsscanf(string_get(str), format, args); // NOLINT
    va_end(args);
    return out;
}

int string_printf(string_t str, const char *format, ...) {
    if (!str)
        return 0;

    va_list args;
    char *mem = string_get(str);

    va_start(args, format);

    // FlawFinder: ignore
    int out = vsnprintf(mem, str->size, format, args); // NOLINT
    va_end(args);
    if (out < 1)
        string_clear(str);
    else {
        str->used = (uint16_t)out;
        mem[out] = 0;
    }
    return out;
}
