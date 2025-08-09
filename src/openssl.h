// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#ifndef CPR_OPENSSL_H
#define CPR_OPENSSL_H

#include <stdbool.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CPR_MD5_DIGEST_SIZE 16
#define CPR_SHA256_DIGEST_SIZE 32
#define CPR_SHA512_DIGEST_SIZE 64

static inline EVP_MD_CTX *md5_digest() {
    EVP_MD_CTX *ctx = EVP_MD_CTX_create();
    if (ctx && EVP_DigestInit_ex(ctx, EVP_md5(), NULL) != 1) {
        EVP_MD_CTX_destroy(ctx);
        ctx = NULL;
    }
    return ctx;
}

static inline EVP_MD_CTX *sha256_digest() {
    EVP_MD_CTX *ctx = EVP_MD_CTX_create();
    if (ctx && EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1) {
        EVP_MD_CTX_destroy(ctx);
        ctx = NULL;
    }
    return ctx;
}

static inline EVP_MD_CTX *sha512_digest() {
    EVP_MD_CTX *ctx = EVP_MD_CTX_create();
    if (ctx && EVP_DigestInit_ex(ctx, EVP_sha512(), NULL) != 1) {
        EVP_MD_CTX_destroy(ctx);
        ctx = NULL;
    }
    return ctx;
}

static inline bool update_digest(EVP_MD_CTX *ctx, const uint8_t *cp, size_t size) {
    if (!ctx)
        return false;
    return EVP_DigestUpdate(ctx, (uint8_t *)cp, size) == 1;
}

static inline unsigned finish_digest(EVP_MD_CTX *ctx, uint8_t *digest) {
    unsigned size = 0;
    if (!ctx)
        return 0;

    EVP_DigestFinal_ex(ctx, digest, &size);
    EVP_MD_CTX_destroy(ctx);
    return size;
}

static inline void reuse_digest(EVP_MD_CTX *ctx) {
    EVP_DigestInit_ex(ctx, NULL, NULL);
}

static inline size_t cpr_sha256(const uint8_t *input, size_t size, uint8_t *out) {
    EVP_MD_CTX *ctx = sha256_digest();
    if (!ctx) return 0;
    update_digest(ctx, input, size);
    return finish_digest(ctx, out);
}

static inline size_t cpr_sha512(const uint8_t *input, size_t size, uint8_t *out) {
    EVP_MD_CTX *ctx = sha512_digest();
    if (!ctx) return 0;
    update_digest(ctx, input, size);
    return finish_digest(ctx, out);
}
#ifdef __cplusplus
}
#endif
#endif
