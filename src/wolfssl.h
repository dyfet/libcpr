// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_WOLFSSL_H
#define CPR_WOLFSSL_H

#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/sha512.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CPR_SHA256_DIGEST_SIZE WC_SHA256_DIGEST_SIZE
#define CPR_SHA512_DIGEST_SIZE WC_SHA512_DIGEST_SIZE

static inline size_t cpr_sha256(const uint8_t *input, size_t size, uint8_t *out) {
    wc_Sha256 ctx;
    if (wc_InitSha256(&ctx) != 0) return 0;
    if (wc_Sha256Update(&ctx, input, size) != 0) return 0;
    if (wc_Sha256Final(&ctx, out) != 0) return 0;
    return WC_SHA256_DIGEST_SIZE;
}

static inline size_t cpr_sha512(const uint8_t *input, size_t size, uint8_t *out) {
    wc_Sha512 ctx;
    if (wc_InitSha512(&ctx) != 0) return 0;
    if (wc_Sha512Update(&ctx, input, size) != 0) return 0;
    if (wc_Sha512Final(&ctx, out) != 0) return 0;
    return WC_SHA512_DIGEST_SIZE;
}

#ifdef __cplusplus
}
#endif
#endif
