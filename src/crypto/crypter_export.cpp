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

/* Copyright (c) 2019-2025 Chilledheart  */

#include "crypto/crypter_export.hpp"

enum cipher_method to_cipher_method(const std::string_view& method) {
#define XX(num, name, string) \
  if (method == string) {     \
    return CRYPTO_##name;     \
  }
  CIPHER_METHOD_MAP(XX)
#undef XX
  return CRYPTO_INVALID;
}

std::string_view to_cipher_method_name(enum cipher_method method) {
  switch (method) {
#define XX(num, name, string)                \
  case num: {                                \
    constexpr std::string_view _ret = #name; \
    return _ret;                             \
  }
    CIPHER_METHOD_MAP(XX)
#undef XX
    default:
      return CRYPTO_INVALID_NAME;
  }
}

std::string_view to_cipher_method_str(enum cipher_method method) {
  switch (method) {
#define XX(num, name, string)                 \
  case num: {                                 \
    constexpr std::string_view _ret = string; \
    return _ret;                              \
  }
    CIPHER_METHOD_MAP(XX)
#undef XX
    default:
      return CRYPTO_INVALID_STR;
  }
}

bool is_valid_cipher_method(enum cipher_method method) {
  switch (method) {
#define XX(num, name, string) \
  case num:                   \
    return true;
    CIPHER_METHOD_MAP(XX)
#undef XX
    default:
      return false;
  }
}
