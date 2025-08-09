// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_SOCKET_H
#define CPR_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <poll.h>
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

// socket casting types

int cpr_join(int so, const struct sockaddr *member, unsigned ifindex);
int cpr_drop(int so, const struct sockaddr *member, unsigned ifindex);
socklen_t cpr_socklen(const struct sockaddr *addr);

inline static struct sockaddr *to_sockaddr(struct sockaddr_storage *storage) {
    return (struct sockaddr *)storage;
}

inline static const struct sockaddr_un *to_unix(const struct sockaddr *addr) {
    return (struct sockaddr_un *)addr;
}

inline static const struct sockaddr_in *to_in4(const struct sockaddr *addr) {
    return (struct sockaddr_in *)addr;
}

inline static const struct sockaddr_in6 *to_in6(const struct sockaddr *addr) {
    return (struct sockaddr_in6 *)addr;
}

#ifdef __cplusplus
}
#endif
#endif
