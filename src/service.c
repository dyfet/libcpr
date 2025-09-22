// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#include "service.h"
#include "strchar.h"
#include "thread.h"
#include "clock.h"

#ifdef _WIN32
bool is_service() {
    bool isService = false;
    DWORD pid = GetCurrentProcessId();
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return false;
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnapshot, &pe)) {
        do {
            if (pe.th32ProcessID == pid) {
                DWORD parentPid = pe.th32ParentProcessID;
                if (Process32First(hSnapshot, &pe)) {
                    do {
                        if (pe.th32ProcessID == parentPid) {
                            isService = eq(pe.szExeFile, "services.exe");
                            break;
                        }
                    } while (Process32Next(hSnapshot, &pe));
                }
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    return isService;
}

__attribute__((format(printf, 2, 3))) void cpr_syslog(int priority, const char *fmt, ...) {}
void cpr_closelog() {}
void cpr_openlog(const char *id, int facility, int flags) {}
#else
static bool logger = false;

bool is_service() {
    return getuid() == 0 || getpid() == 1 || getppid() == 1;
}

__attribute__((format(printf, 2, 3))) void cpr_syslog(int priority, const char *fmt, ...) {
    if (!logger) return;
    va_list args;
    va_start(args, fmt);
    vsyslog(priority, fmt, args);
    va_end(args);
}

void cpr_closelog() {
    logger = false;
    closelog();
}

void cpr_openlog(const char *id, int facility, int flags) {
    if (!id) return;
    int level = LOG_INFO;
    if (!flags) flags = LOG_NDELAY;
    switch (cpr_verbose) {
    case 0:
        level = LOG_ERR;
        break;
    case 1:
        level = LOG_INFO;
        break;
    case 2:
        level = LOG_NOTICE;
        break;
    default:
        level = LOG_DEBUG;
        break;
    }
    logger = true;
    openlog(id, flags, facility);
    setlogmask(LOG_UPTO(level));
}
#endif

__attribute__((format(printf, 3, 4))) void cpr_logger(FILE *out, int level, const char *fmt, ...) {
    static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    va_list args;
    va_start(args, fmt);
    const char *type = "note";
    switch (level) {
    case 0:
        type = "fail";
        cpr_syslog(LOG_ERR, fmt, args);
        break;
    case 1:
        type = "warn";
        cpr_syslog(LOG_WARNING, fmt, args);
        break;
    case 2:
        type = "info";
        cpr_syslog(LOG_INFO, fmt, args);
        break;
    case 3:
        cpr_syslog(LOG_NOTICE, fmt, args);
        break;
    default:
#ifndef NDEBUG
        cpr_syslog(LOG_DEBUG, fmt, args);
#endif
        break;
    }
    if (cpr_verbose <= level) {
        pthread_mutex_lock(&mtx);
        time_t now;
        struct tm ts = {0};
        char buf[80];
        time(&now);
#ifdef _WIN32
        localtime_s(&ts, &now);
#else
        localtime_r(&now, &ts);
#endif
        strftime(buf, sizeof(buf), LOG_DATETIME_FORMAT, &ts);
        fprintf(out, "%s %s: ", buf, type);
        vfprintf(out, fmt, args); // FlawFinder: ignore
        fputc('\n', out);
        fflush(out);
        pthread_mutex_unlock(&mtx);
    }
    va_end(args);
}
