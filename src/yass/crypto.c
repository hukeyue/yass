// SPDX-License-Identifier: GPL-2.0 OR CDDL-1.0
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

#include "yass/crypto.h"

#include <string.h>

yass_cipher_method yass_to_cipher_method(const char* method) {
#define XX(num, name, string)                \
  if (0 == strncmp(method, string, 80)) {    \
    return YASS_CRYPTO_##name;               \
  }
  YASS_CIPHER_METHOD_MAP(XX)
#undef XX
  return YASS_CRYPTO_INVALID;
}

const char* yass_to_cipher_method_name(yass_cipher_method method) {
  switch (method) {
#define XX(num, name, string)                \
  case num: {                                \
    return #name;                            \
  }
    YASS_CIPHER_METHOD_MAP(XX)
#undef XX
    default:
      return YASS_CRYPTO_INVALID_CNAME;
  }
}

const char* yass_to_cipher_method_str(yass_cipher_method method) {
  switch (method) {
#define XX(num, name, string)                \
  case num: {                                \
    return string;                           \
  }
    YASS_CIPHER_METHOD_MAP(XX)
#undef XX
    default:
      return YASS_CRYPTO_INVALID_CSTR;
  }
}

int yass_is_valid_cipher_method(yass_cipher_method method) {
  switch (method) {
#define XX(num, name, string) \
  case num:                   \
    return 0;
    YASS_CIPHER_METHOD_MAP(XX)
#undef XX
    default:
      return -1;
  }
}

#define XX(num, name, string) const char YASS_CRYPTO_##name##_CSTR[] = string;
YASS_CIPHER_METHOD_MAP(XX)
#undef XX

#define XX(num, name, string) const char YASS_CRYPTO_##name##_CNAME[] = #name;
YASS_CIPHER_METHOD_MAP(XX)
#undef XX

#define XX(num, name, string) (yass_cipher_method)num,
const yass_cipher_method kYassCipherMethods[] = {YASS_CIPHER_METHOD_VALID_MAP(XX)};
#undef XX

#define XX(num, name, string) string,
const char* const kYassCipherMethodCStrs[] = {YASS_CIPHER_METHOD_VALID_MAP(XX)};
#undef XX

#define XX(num, name, string) L##string,
const wchar_t* const kYassCipherMethodLCStrs[] = {YASS_CIPHER_METHOD_VALID_MAP(XX)};
#undef XX
