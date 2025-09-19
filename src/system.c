// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#include "system.h"

#ifdef _WIN32
int make_dir(const char *path, int perms) {
    int err = mkdir(path);
    if (!err && perms)
        err = chmod(path, perms); // FlawFinder: ignore
    return err;
}
#else
int make_dir(const char *path, int perms) {
    if (!perms)
        perms = 0755;

    return mkdir(path, perms);
}
#endif

bool is_dir(const char *path) {
    struct stat ino;
    if (stat(path, &ino))
        return false;

    if (S_ISDIR(ino.st_mode))
        return true;

    return false;
}

bool is_file(const char *path) {
    struct stat ino;
    if (stat(path, &ino))
        return false;

    if (S_ISREG(ino.st_mode))
        return true;

    return false;
}

#ifdef _WIN32
ssize_t getline(char **lp, size_t *size, FILE *fp) {
    size_t pos = 0;
    int c = EOF;

    if (lp == NULL || fp == NULL || size == NULL) return -1;
    c = getc(fp); // FlawFinder: ignore
    if (c == EOF) return -1;
    if (*lp == NULL) {
        *lp = malloc(128);
        if (*lp == NULL) return -1;
        *size = 128;
    }

    pos = 0;
    while (c != EOF) {
        if (pos + 1 >= *size) {
            size_t new_size = *size + (*size >> 2);
            if (new_size < 128) {
                new_size = 128;
            }
            char *new_ptr = realloc(*lp, new_size);
            if (new_ptr == NULL) return -1;
            *size = new_size;
            *lp = new_ptr;
        }

        ((unsigned char *)(*lp))[pos++] = (unsigned char)c;
        if (c == '\n') break;
        c = getc(fp); // FlawFinder: ignore
    }
    (*lp)[pos] = '\0';
    return (ssize_t)(pos);
}

char *getpass(const char *prompt) { // FlawFinder: ignore for now
    static char buffer[128];
    DWORD mode;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE) return NULL;
    if (!GetConsoleMode(hStdin, &mode)) return NULL;
    fputs(prompt, stdout);
    fflush(stdout);
    SetConsoleMode(hStdin, mode & ~(ENABLE_ECHO_INPUT));
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        SetConsoleMode(hStdin, mode);
        return NULL;
    }

    SetConsoleMode(hStdin, mode);
    fputs("\n", stdout);
    buffer[strcspn(buffer, "\r\n")] = '\0';
    return buffer;
}
#endif
