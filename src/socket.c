// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef _WIN32
#include "socket.h"

#include <string.h>
#include <errno.h>

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

int cpr_join(int so, const struct sockaddr *member, unsigned ifindex) {
    int res = 0;
    multicast_t multicast;

    if (so < 0) return EBADF;
    memset(&multicast, 0, sizeof(multicast));
    switch (member->sa_family) {
    case AF_INET:
        multicast.ipv4.imr_interface.s_addr = INADDR_ANY;
        multicast.ipv4.imr_multiaddr = to_in4(member)->sin_addr;
        if (setsockopt(so, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast, sizeof(multicast.ipv4)) == -1)
            res = errno;
        break;
    case AF_INET6:
        multicast.ipv6.ipv6mr_interface = ifindex;
        multicast.ipv6.ipv6mr_multiaddr = to_in6(member)->sin6_addr;
        if (setsockopt(so, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &multicast, sizeof(multicast.ipv6)) == -1)
            res = errno;
        break;
    default:
        res = EAI_FAMILY;
    }
    return res;
}

int cpr_drop(int so, const struct sockaddr *member, unsigned ifindex) {
    int res = 0;
    multicast_t multicast;

    if (so < 0) return EBADF;
    memset(&multicast, 0, sizeof(multicast));
    switch (member->sa_family) {
    case AF_INET:
        multicast.ipv4.imr_interface.s_addr = INADDR_ANY;
        multicast.ipv4.imr_multiaddr = to_in4(member)->sin_addr;
        if (setsockopt(so, IPPROTO_IP, IP_DROP_MEMBERSHIP, &multicast, sizeof(multicast.ipv4)) == -1)
            res = errno;
        break;
    case AF_INET6:
        multicast.ipv6.ipv6mr_interface = ifindex;
        multicast.ipv6.ipv6mr_multiaddr = to_in6(member)->sin6_addr;
        if (setsockopt(so, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP, &multicast, sizeof(multicast.ipv6)) == -1)
            res = errno;
        break;
    default:
        res = EAI_FAMILY;
    }
    return res;
}
#endif
