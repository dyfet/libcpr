// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#include "events.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <poll.h>

// NOTE: assuming FreeBSD >= 13
#if defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__)
#include <sys/eventfd.h>
#endif

bool cpr_initevt(event_t *evt) {
    if (!evt) return false;
#ifdef EFD_NONBLOCK
    evt->fds[0] = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evt->fds[0] == -1) return false;
    evt->fds[1] = evt->fds[0];
#else
    if (pipe(evt->fds) == -1) return false;
    fcntl(evt->fds[0], F_SETFL, O_NONBLOCK);
    fcntl(evt->fds[1], F_SETFL, O_NONBLOCK);
#endif
    return true;
}

void cpr_freeevt(event_t *evt) {
    if (!evt || evt->fds[0] == -1) return;
    if (evt->fds[0] != evt->fds[1]) close(evt->fds[1]);
    close(evt->fds[0]);
    evt->fds[0] = evt->fds[1] = -1;
}

bool cpr_setevt(event_t *evt) {
    if (!evt || evt->fds[0] == -1) return false;
#ifdef EFD_NONBLOCK
    uint64_t one = 1;
    int rtn = write(evt->fds[1], &one, sizeof(one));
#else
    int rtn = write(evt->fds[1], "x", 1);
#endif
    if (rtn < 0) {
        cpr_freeevt(evt);
        return false;
    }
    return rtn > 0;
}

bool cpr_clearevt(event_t *evt) {
    if (!evt || evt->fds[0] == -1) return false;
#ifdef EFD_NONBLOCK
    uint64_t count;
    int rtn = read(evt->fds[0], &count, sizeof(count));
#else
    char buf[64];
    int rtn = read(evt->fds[0], buf, sizeof(buf)); // drain pending signals
#endif
    if (rtn < 0) {
        cpr_freeevt(evt);
        return false;
    }
    return rtn > 0;
}

bool cpr_waitevt(event_t *evt, int timeout) {
    if (!evt || evt->fds[0] == -1) return false;
    struct pollfd pfd = {.fd = evt->fds[0], .events = POLLIN};
    int rtn = poll(&pfd, 1, timeout);
    if (rtn < 0) {
        cpr_freeevt(evt);
        return false;
    }
    return rtn > 0;
}
