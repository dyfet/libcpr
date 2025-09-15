// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_SYSTEM_H
#define CPR_SYSTEM_H

#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#if defined(__MINGW32__) || defined(__MINGW64__) || defined(WIN32)
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#define WINDOWS_SYSTEM
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <tlhelp32.h>
#include <io.h>
#include <direct.h>
#include <winioctl.h>
#include <fileapi.h>
#include <stdio.h>
#include <string.h>
#endif

#if defined(__OpenBSD__)
#define stat64 stat
#define fstat64 fstat
#endif

#ifndef R_OK
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
typedef struct {
    WIN32_FIND_DATA find;
    HINSTANCE fd;
} DIR;

// statically allocated in readdir, not threadsafe...
typedef struct {
    char d_name[256];
} dirent;

static inline int last_error();
static inline char *optarg = NULL;
static inline optind = 0, optopt = 0, opterr = 1;

static inline int cpr_getopt(int argc, char *argv[], const char *optstring) {
    char c;
    const char *place;
    static char *scan = NULL;

    for (optarg = NULL; scan == NULL || !*scan; scan++, optind++) {
        if (optind >= argc) {
            scan = NULL;
            return (EOF);
        }
        if (*(scan = argv[optind]) != '-') {
            optarg = scan;
            scan = NULL;
            optind++;
            return (0);
        }
    }

    if ((place = strchr(optstring, c = *(scan++))) == NULL || c == ':') {
        if (opterr)
            fprintf(stderr, "%s: unknown command option '%c'\n", argv[0], c);
        return '?';
    }

    optarg = NULL;
    if (place[1] == ':') {
        if (*scan) {
            optarg = scan;
            scan = NULL;
        } else if (optind < argc) {
            optarg = argv[optind++];
        } else {
            if (opterr)
                fprintf(stderr, "%s: command option '%c' missing its argument\n", argv[0], c);
            optopt = c;
            return '?';
        }
    }

    return c;
}

static inline DIR *opendir(const char *path) {
    DWORD attr = GetFileAttributes(path);
    if ((attr == (DWORD)~0l) || !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
        return NULL;
    }

    DIR *dir = (DIR *)malloc(sizeof(DIR));
    if (!dir)
        return NULL;

    char tpath[256];
    snprintf(tpath, sizeof(tpath), "%s%s", path, "\\*"); // NOLINT
    dir->fd = FindFirstFile(tpath, &dir->find);
    if (dir->fd == INVALID_HANDLE_VALUE) {
        free(dir);
        return NULL;
    }
    return dir;
}

static inline int closedir(DIR *dir) {
    if (!dir)
        return -EBADF;

    if (dir->fd != INVALID_HANDLE_VALUE)
        FindClose(dir->fd);

    free(dir);
    return 0;
}

static inline dirent *readdir(DIR *dir) {
    static dirent entry;

    if (!dir || dir->fd != INVALID_HANDLE_VALUE)
        return NULL;

    snprintf(entry.d_name, sizeof(entry.d_name), "%s", dir->find.cFileName); // NOLINT
    if (!FindNextFile(dir->fd, &dir->find)) {
        FindClose(dir->fd);
        dir->fd = INVALID_HANDLE_VALUE;
    }
    return &entry;
}

static inline int makedir(const char *path, int perms) {
    if (!CreateDirectory(path, NULL))
        return -last_error();

    if (!perms)
        return 0;

    if (perms & 06)
        perms |= 01;
    if (perms & 060)
        perms |= 010;
    if (perms & 0600)
        perms |= 0100;

    return _chmod(path, perms);
}

static inline int removedir(const char *path) {
    if (RemoveDirectory(path))
        return 0;

    return -last_error();
}
#else
#ifdef WINDOWS_SYSTEM
static inline int makedir(const char *path, int perms) {
    int err = mkdir(path);
    if (!err && perms)
        err = chmod(path, perms); // FlawFinder: ignore
    return err;
}
#else
static inline int makedir(const char *path, int perms) {
    if (!perms)
        perms = 0755;

    return mkdir(path, perms);
}
#endif

static inline int removedir(const char *path) {
    return rmdir(path);
}
#endif

static inline bool is_dir(const char *path) {
#ifdef WINDOWS_SYSTEM
    DWORD attr = GetFileAttributes(path);
    if (attr == (DWORD)~0l)
        return false;

    if (attr & FILE_ATTRIBUTE_DIRECTORY)
        return true;

    return false;
#else
    struct stat ino;

    if (stat(path, &ino))
        return false;

    if (S_ISDIR(ino.st_mode))
        return true;

    return false;
#endif
}

static inline bool is_file(const char *path) {
#ifdef WINDOWS_SYSTEM
    DWORD attr = GetFileAttributes(path);
    if (attr == (DWORD)~0l)
        return false;

    if (attr & FILE_ATTRIBUTE_DIRECTORY)
        return false;

    return true;

#else
    struct stat ino;

    if (stat(path, &ino))
        return false;

    if (S_ISREG(ino.st_mode))
        return true;

    return false;
#endif
}

static inline int last_error() {
#ifdef WINDOWS_SYSTEM
    DWORD err = GetLastError();
    switch (err) {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_INVALID_NAME:
    case ERROR_BAD_PATHNAME:
        return ENOENT;
    case ERROR_TOO_MANY_OPEN_FILES:
        return EMFILE;
    case ERROR_ACCESS_DENIED:
    case ERROR_WRITE_PROTECT:
    case ERROR_SHARING_VIOLATION:
    case ERROR_LOCK_VIOLATION:
        return EACCES;
    case ERROR_INVALID_HANDLE:
        return EBADF;
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
        return ENOMEM;
    case ERROR_INVALID_DRIVE:
    case ERROR_BAD_UNIT:
    case ERROR_BAD_DEVICE:
        return ENODEV;
    case ERROR_NOT_SAME_DEVICE:
        return EXDEV;
    case ERROR_NOT_SUPPORTED:
    case ERROR_CALL_NOT_IMPLEMENTED:
        return ENOSYS;
    case ERROR_END_OF_MEDIA:
    case ERROR_EOM_OVERFLOW:
    case ERROR_HANDLE_DISK_FULL:
    case ERROR_DISK_FULL:
        return ENOSPC;
    case ERROR_BAD_NETPATH:
    case ERROR_BAD_NET_NAME:
        return EACCES;
    case ERROR_FILE_EXISTS:
    case ERROR_ALREADY_EXISTS:
        return EEXIST;
    case ERROR_CANNOT_MAKE:
    case ERROR_NOT_OWNER:
        return EPERM;
    case ERROR_NO_PROC_SLOTS:
        return EAGAIN;
    case ERROR_BROKEN_PIPE:
    case ERROR_NO_DATA:
        return EPIPE;
    case ERROR_OPEN_FAILED:
        return EIO;
    case ERROR_NOACCESS:
        return EFAULT;
    case ERROR_IO_DEVICE:
    case ERROR_CRC:
    case ERROR_NO_SIGNAL_SENT:
        return EIO;
    case ERROR_CHILD_NOT_COMPLETE:
    case ERROR_SIGNAL_PENDING:
    case ERROR_BUSY:
        return EBUSY;
    case ERROR_DIR_NOT_EMPTY:
        return ENOTEMPTY;
    case ERROR_DIRECTORY:
        return ENOTDIR;
    default:
        return EINVAL;
    }
#else
    return errno;
#endif
}

#ifdef __cplusplus
}
#endif
#endif
