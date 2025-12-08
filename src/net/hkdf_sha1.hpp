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

#ifndef H_NET_HKDF_SHA1
#define H_NET_HKDF_SHA1

namespace net {

/*
 * Key Derivation
 *
 * HKDF_SHA1 is a function that takes a secret key, a non-secret salt, an info
 * string, and produces a subkey that is cryptographically strong even if the
 * input secret key is weak.
 *
 *      HKDF_SHA1(key, salt, info) => subkey
 *
 * The info string binds the generated subkey to a specific application context.
 * In our case, it must be the string "ss-subkey" without quotes.
 *
 * We derive a per-session subkey from a pre-shared master key using HKDF_SHA1.
 * Salt must be unique through the entire life of the pre-shared master key.
 *
 */

#define SUBKEY_INFO "ss-subkey"

int crypto_hkdf_extract(const unsigned char* salt,
                        int salt_len,
                        const unsigned char* ikm,
                        int ikm_len,
                        unsigned char* prk);

int crypto_hkdf_expand(const unsigned char* prk,
                       int prk_len,
                       const unsigned char* info,
                       int info_len,
                       unsigned char* okm,
                       int okm_len);

/* HKDF-Extract + HKDF-Expand */
int crypto_hkdf(const unsigned char* salt,
                int salt_len,
                const unsigned char* ikm,
                int ikm_len,
                const unsigned char* info,
                int info_len,
                unsigned char* okm,
                int okm_len);

}  // namespace net

#endif  // H_NET_HKDF_SHA1
