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

/* Copyright (c) 2023-2025 Chilledheart  */

#ifndef H_NET_OPENSSL_UTIL
#define H_NET_OPENSSL_UTIL

#include <cstdint>

namespace net {
// Utility to construct the appropriate set & clear masks for use the OpenSSL
// options and mode configuration functions. (SSL_set_options etc)
struct SslSetClearMask {
  SslSetClearMask();
  void ConfigureFlag(long flag, bool state);

  long set_mask = 0;
  long clear_mask = 0;
};
int OpenSSLNetErrorLib();
int MapOpenSSLErrorSSL(uint32_t error_code);
int MapOpenSSLErrorWithDetails(int err);
void OpenSSLPutNetError(const char* file, int line, int err);

inline int MapOpenSSLError(int err) {
  return MapOpenSSLErrorWithDetails(err);
}

}  // namespace net

#define FROM_HERE __FILE__, __LINE__

#endif  // H_NET_OPENSSL_UTIL
