// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#include "system.h"
#include "strchar.h"

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

int get_pass(char *buf, size_t size, const char *prompt) {
    if (prompt == NULL) prompt = "Password: ";
    if (!buf || size == 0) return -1;
    DWORD mode;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE || !GetConsoleMode(hStdin, &mode))
        return -2;

    fputs(prompt, stdout);
    fflush(stdout);
    if (!SetConsoleMode(hStdin, mode & ~(ENABLE_ECHO_INPUT))) return -3;
    if (!fgets(buf, (int)size, stdin)) {
        SetConsoleMode(hStdin, mode);
        return -4;
    }

    SetConsoleMode(hStdin, mode);
    fputs("\n", stdout);
    buf[strcspn(buf, "\r\n")] = '\0';
    return 0;
}

#else
#include <termios.h>

int get_pass(char *buf, size_t size, const char *prompt) {
    if (prompt == NULL) prompt = "Password: ";
    if (!buf || size == 0) return -1;
    struct termios old, new;
    if (tcgetattr(STDIN_FILENO, &old) != 0) return -1;
    new = old;
    new.c_lflag &= ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new) != 0) return -1;

    fputs(prompt, stdout);
    fflush(stdout);
    if (!fgets(buf, (int)size, stdin)) buf[0] = '\0';
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old);
    printf("\n");
    size_t len = cpr_strlen(buf, size);
    if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
    return 0;
}
#endif
