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

#ifndef H_CRYPTO_MEBDTLS_COMMON
#define H_CRYPTO_MEBDTLS_COMMON

#ifdef HAVE_MBEDTLS
#include "crypto/crypter_export.hpp"

#include <mbedtls/cipher.h>
#include <mbedtls/version.h>

namespace crypto {
mbedtls_cipher_context_t* mbedtls_create_evp(enum cipher_method method);
void mbedtls_release_evp(mbedtls_cipher_context_t* evp);
const mbedtls_cipher_info_t* mbedtls_get_cipher(enum cipher_method method);
uint8_t mbedtls_get_nonce_size(enum cipher_method method);
uint8_t mbedtls_get_key_size(enum cipher_method method);
}  // namespace crypto

#endif  // HAVE_MBEDTLS
#endif  // H_CRYPTO_MEBDTLS_COMMON
