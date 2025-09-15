// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_MULTICAST_H
#define CPR_MULTICAST_H

#ifndef _WIN32
#ifdef __cplusplus
extern "C" {
#endif

#include "socket.h"

#include <cpr/socket.h>
#include <stdint.h>
#include <ifaddrs.h>

#ifndef IPV6_ADD_MEMBERSHIP
#define IPV6_ADD_MEMBERSHIP IP_ADD_MEMBERSHIP
#endif

#ifndef IPV6_DROP_MEMBERSHIP
#define IPV6_DROP_MEMBERSHIP IP_DROP_MEMBERSHIP
#endif

typedef union {
    struct ip_mreq ipv4;
    struct ipv6_mreq ipv6;
} multicast_t;

#define MULTICAST_IPV4_DEFAULT_GROUP "239.0.0.1"
#define MULTICAST_IPV6_DEFAULT_GROUP "FF35::1234"

typedef struct ifaddrs *iface_t;

extern unsigned int if_index;
iface_t find_multicast(iface_t list, const char *iface, int family);
int make_multicast(const char *mcast, int family, uint16_t port);
int join_multicast(int so, const struct sockaddr *member);
int drop_multicast(int so, const struct sockaddr *member);

#ifdef __cplusplus
}
#endif
#endif
#endif
