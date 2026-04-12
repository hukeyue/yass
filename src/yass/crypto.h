// SPDX-License-Identifier: GPL-2.0 OR LGPL-2.1 OR CDDL-1.0
/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or https://opensource.org/licenses/CDDL-1.0.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/* Copyright (c) 2019-2026 Chilledheart  */

#ifndef YASS_CRYPTO_H
#define YASS_CRYPTO_H

#include <stddef.h>

#include "yass/config.h"
#include "yass/feature.h"

#ifdef __cplusplus
extern "C" {
#endif

#define YASS_CIPHER_METHOD_MAP_SODIUM(XX)                  \
  XX(0x3U, AES256GCMSHA256, "aes-256-gcm")                 \
  XX(0x4U, CHACHA20POLY1305IETF, "chacha20-ietf-poly1305") \
  XX(0x5U, XCHACHA20POLY1305IETF, "xchacha20-ietf-poly1305")

#define YASS_CIPHER_METHOD_MAP_BORINGSSL(XX)                          \
  XX(0x14U, CHACHA20POLY1305IETF_EVP, "chacha20-ietf-poly1305-evp")   \
  XX(0x15U, XCHACHA20POLY1305IETF_EVP, "xchacha20-ietf-poly1305-evp") \
  XX(0x16U, AES128GCMSHA256_EVP, "aes-128-gcm-evp")                   \
  XX(0x17U, AES128GCM12SHA256_EVP, "aes-128-gcm12-evp")               \
  XX(0x18U, AES192GCMSHA256_EVP, "aes-192-gcm-evp")                   \
  XX(0x19U, AES256GCMSHA256_EVP, "aes-256-gcm-evp")

#ifdef HAVE_MBEDTLS
#define YASS_CIPHER_METHOD_MAP_MBEDTLS(XX)        \
  XX(0x22U, AES_128_CFB, "aes-128-cfb")           \
  XX(0x23U, AES_192_CFB, "aes-192-cfb")           \
  XX(0x24U, AES_256_CFB, "aes-256-cfb")           \
  XX(0x25U, AES_128_CTR, "aes-128-ctr")           \
  XX(0x26U, AES_192_CTR, "aes-192-ctr")           \
  XX(0x27U, AES_256_CTR, "aes-256-ctr")           \
  XX(0x29U, CAMELLIA_128_CFB, "camellia-128-cfb") \
  XX(0x30U, CAMELLIA_192_CFB, "camellia-192-cfb") \
  XX(0x31U, CAMELLIA_256_CFB, "camellia-256-cfb")
#else
#define YASS_CIPHER_METHOD_MAP_MBEDTLS(XX)
#endif

#define YASS_CIPHER_METHOD_MAP_HTTP(XX) XX(0x110U, HTTPS, "https")

#ifdef HAVE_QUICHE
#define YASS_CIPHER_METHOD_MAP_HTTPS(XX)         \
  YASS_CIPHER_METHOD_MAP_HTTP(XX)                \
  XX(0x121U, HTTP2, "http2")
#define YASS_CIPHER_METHOD_MAP_HTTP2(XX)         \
  XX(0x120U, HTTP2_PLAINTEXT, "http2-plaintext") \
  XX(0x121U, HTTP2, "http2")
#define YASS_CIPHER_METHOD_MAP_FULL_HTTP2(XX)     \
  XX(0x120U, HTTP2_PLAINTEXT, "http2-plaintext")  \
  XX(0x121U, HTTP2, "http2")                      \
  XX(0x122U, HTTP2_INPLACE_2, "http2-2-protocol") \
  XX(0x123U, HTTP2_INPLACE_3, "http2-3-protocol") \
  XX(0x124U, HTTP2_INPLACE_4, "http2-4-protocol") \
  XX(0x125U, HTTP2_INPLACE_5, "http2-5-protocol")
#else
#define YASS_CIPHER_METHOD_MAP_HTTPS(XX) YASS_CIPHER_METHOD_MAP_HTTP(XX)
#define YASS_CIPHER_METHOD_MAP_HTTP2(XX)
#endif

#define YASS_CIPHER_METHOD_MAP_SOCKS(XX) \
  XX(0x150U, SOCKS4, "socks4")           \
  XX(0x151U, SOCKS4A, "socks4a")         \
  XX(0x152U, SOCKS5, "socks5")           \
  XX(0x153U, SOCKS5H, "socks5h")

#define YASS_CIPHER_METHOD_OLD_MAP(XX) \
  YASS_CIPHER_METHOD_MAP_SODIUM(XX)    \
  YASS_CIPHER_METHOD_MAP_BORINGSSL(XX) \
  YASS_CIPHER_METHOD_MAP_MBEDTLS(XX)

#define YASS_CIPHER_METHOD_VALID_MAP(XX) \
  YASS_CIPHER_METHOD_MAP_SODIUM(XX)      \
  YASS_CIPHER_METHOD_MAP_BORINGSSL(XX)   \
  YASS_CIPHER_METHOD_MAP_MBEDTLS(XX)     \
  YASS_CIPHER_METHOD_MAP_HTTP(XX)        \
  YASS_CIPHER_METHOD_MAP_HTTP2(XX)       \
  YASS_CIPHER_METHOD_MAP_SOCKS(XX)

#define YASS_CIPHER_METHOD_MAP(XX)  \
  XX(0x0U, INVALID, "invalid") \
  YASS_CIPHER_METHOD_VALID_MAP(XX)

typedef enum _yass_cipher_method {
#define XX(num, name, string) YASS_CRYPTO_##name = num,
  YASS_CIPHER_METHOD_MAP(XX)
#undef XX
} yass_cipher_method;

YASS_DLL yass_cipher_method yass_to_cipher_method(const char* method);
YASS_DLL const char* yass_to_cipher_method_name(yass_cipher_method method);
YASS_DLL const char* yass_to_cipher_method_str(yass_cipher_method method);
YASS_DLL int yass_is_valid_cipher_method(yass_cipher_method method);

#define XX(num, name, string) YASS_DLL extern const char YASS_CRYPTO_##name##_CSTR[];
YASS_CIPHER_METHOD_MAP(XX)
#undef XX

#define XX(num, name, string) YASS_DLL extern const char YASS_CRYPTO_##name##_CNAME[];
YASS_CIPHER_METHOD_MAP(XX)
#undef XX

YASS_DLL extern const yass_cipher_method kYassCipherMethods[];
YASS_DLL extern const char* const kYassCipherMethodCStrs[];
YASS_DLL extern const wchar_t* const kYassCipherMethodLCStrs[];

#ifdef __cplusplus
} // extern "C"
#endif

#endif  // YASS_CRYPTO_H
