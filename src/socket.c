// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#include "socket.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

#ifdef _WIN32
void cpr_sockclose(int so) {
    shutdown(so, SD_BOTH);
    closesocket(so);
}

int cpr_sockinit() {
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 2), &wsa);
}
#else
void cpr_sockclose(int so) {
    shutdown(so, SHUT_RDWR);
    close(so);
}

int cpr_sockinit() {
    return 0;
}
#endif

socklen_t cpr_socklen(const struct sockaddr *addr) {
    if (!addr) return 0;
    switch (addr->sa_family) {
    case AF_INET:
        return sizeof(struct sockaddr_in);
    case AF_INET6:
        return sizeof(struct sockaddr_in6);
    default:
        return 0;
    }
}
