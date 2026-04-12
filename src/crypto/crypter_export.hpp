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

#ifndef H_CRYPTO_CRYPTER_EXPORT
#define H_CRYPTO_CRYPTER_EXPORT

#include <stdint.h>
#include <string_view>

#include "yass/crypto.h"

#define MAX_KEY_LENGTH 64
#define MAX_NONCE_LENGTH 32

enum cipher_method : uint32_t {
#define XX(num, name, string) CRYPTO_##name = num,
  YASS_CIPHER_METHOD_MAP(XX)
#undef XX
};

#ifdef HAVE_QUICHE
#define CIPHER_METHOD_IS_HTTP2(m) ((m) == CRYPTO_HTTP2_PLAINTEXT || (m) == CRYPTO_HTTP2)
#define CIPHER_METHOD_IS_TLS(m) ((m) == CRYPTO_HTTPS || (m) == CRYPTO_HTTP2)
#define CIPHER_METHOD_IS_HTTPS(m) ((m) == CRYPTO_HTTPS)
#define CIPHER_METHOD_IS_HTTP(m) ((m) == CRYPTO_HTTPS || (m) == CRYPTO_HTTP2_PLAINTEXT || (m) == CRYPTO_HTTP2)
#else
#define CIPHER_METHOD_IS_HTTP2(m) false
#define CIPHER_METHOD_IS_TLS(m) ((m) == CRYPTO_HTTPS)
#define CIPHER_METHOD_IS_HTTPS(m) ((m) == CRYPTO_HTTPS)
#define CIPHER_METHOD_IS_HTTP(m) ((m) == CRYPTO_HTTPS)
#endif

#define CIPHER_METHOD_IS_SOCKS(m) \
  ((m) == CRYPTO_SOCKS4 || (m) == CRYPTO_SOCKS4A || (m) == CRYPTO_SOCKS5 || (m) == CRYPTO_SOCKS5H)
#define CIPHER_METHOD_IS_SOCKS_NON_DOMAIN_NAME(m) ((m) == CRYPTO_SOCKS4 || (m) == CRYPTO_SOCKS5)
#define CIPHER_METHOD_IS_SOCKS_DOMAIN_NAME(m) ((m) == CRYPTO_SOCKS4A || (m) == CRYPTO_SOCKS5H)
#define CIPHER_METHOD_IS_SOCKS5(m) ((m) == CRYPTO_SOCKS5 || (m) == CRYPTO_SOCKS5H)

#ifdef HAVE_QUICHE
#define CRYPTO_DEFAULT CRYPTO_HTTP2
#define CRYPTO_DEFAULT_STR CRYPTO_HTTP2_STR
#define CRYPTO_DEFAULT_CSTR CRYPTO_HTTP2_CSTR
#elif defined(HAVE_MBEDTLS)
#define CRYPTO_DEFAULT CHACHA20POLY1305IETF_EVP
#define CRYPTO_DEFAULT_STR CHACHA20POLY1305IETF_EVP_STR
#define CRYPTO_DEFAULT_CSTR CHACHA20POLY1305IETF_EVP_CSTR
#else
#define CRYPTO_DEFAULT CRYPTO_AES256GCMSHA256
#define CRYPTO_DEFAULT_STR CRYPTO_AES256GCMSHA256_STR
#define CRYPTO_DEFAULT_CSTR CRYPTO_AES256GCMSHA256_CSTR
#endif

enum cipher_method to_cipher_method(const std::string_view& method);
std::string_view to_cipher_method_name(enum cipher_method method);
std::string_view to_cipher_method_str(enum cipher_method method);
bool is_valid_cipher_method(enum cipher_method method);

#define XX(num, name, string) constexpr const std::string_view CRYPTO_##name##_STR = string;
YASS_CIPHER_METHOD_MAP(XX)
#undef XX

#define XX(num, name, string) constexpr const std::string_view CRYPTO_##name##_NAME = #name;
YASS_CIPHER_METHOD_MAP(XX)
#undef XX

#define XX(num, name, string) constexpr const char CRYPTO_##name##_CSTR[] = string;
YASS_CIPHER_METHOD_MAP(XX)
#undef XX

#define XX(num, name, string) static_cast<enum cipher_method>(num),
constexpr const enum cipher_method kCipherMethods[] = {YASS_CIPHER_METHOD_VALID_MAP(XX)};
#undef XX

#define XX(num, name, string) string,
constexpr const std::string_view kCipherMethodStrs[] = {YASS_CIPHER_METHOD_VALID_MAP(XX)};
#undef XX

#define XX(num, name, string) string,
constexpr const char* kCipherMethodCStrs[] = {YASS_CIPHER_METHOD_VALID_MAP(XX)};
#undef XX

#define XX(num, name, string) L##string,
constexpr const wchar_t* kCipherMethodLCStrs[] = {YASS_CIPHER_METHOD_VALID_MAP(XX)};
#undef XX

#endif  // H_CRYPTO_CRYPTER_EXPORT
