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

/* Copyright (c) 2024-2025 Chilledheart  */

#ifndef H_CONFIG_CONFIG_EXPORT
#define H_CONFIG_CONFIG_EXPORT

#include <absl/strings/str_cat.h>
#include <stdint.h>
#include <string>
#include <string_view>
#include "crypto/crypter_export.hpp"

struct PortFlag {
  explicit PortFlag(uint16_t p = 0) : port(p) {}
  operator uint16_t() const { return port; }
  uint16_t port;
};

struct CipherMethodFlag {
  explicit CipherMethodFlag(cipher_method m = CRYPTO_INVALID) : method(m) {}
  operator std::string_view() const { return to_cipher_method_str(method); }
  operator cipher_method() const { return method; }
  cipher_method method;
};

struct RateFlag {
  explicit RateFlag(uint64_t r = 0u) : rate(r) {}
  operator std::string() const {
    if (rate % (1L << 20) == 0) {
      return absl::StrCat(rate / (1L << 20), "m");
    } else if (rate % (1L << 10) == 0) {
      return absl::StrCat(rate / (1L << 10), "k");
    }
    return absl::StrCat(rate);
  }
  operator uint64_t() const { return rate; }
  uint64_t rate;
};

#endif  // H_CONFIG_CONFIG_EXPORT
