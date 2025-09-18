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

#ifndef H_NET_HMAC_SHA1
#define H_NET_HMAC_SHA1

#include "third_party/boringssl/src/include/openssl/sha.h"

#define HASH_BLOCK_SIZE 128
#define HASH_BLOCK_SIZE_256 64

#define OUTPUT_SIZE_SHA1 SHA_DIGEST_LENGTH

namespace net {

int hmac_sha1_starts(SHA_CTX* ctx, unsigned char* ipad, unsigned char* opad, const unsigned char* key, size_t keylen);

int hmac_sha1_update(SHA_CTX* ctx, unsigned char* ipad, unsigned char* opad, const unsigned char* input, size_t ilen);

int hmac_sha1_finish(SHA_CTX* ctx, unsigned char* ipad, unsigned char* opad, unsigned char* output);

int hmac_sha1_reset(SHA_CTX* ctx, unsigned char* ipad, unsigned char* opad);

int hmac_sha1(const unsigned char* key, size_t keylen, const unsigned char* input, size_t ilen, unsigned char* output);

}  // namespace net

#endif  // H_NET_HMAC_SHA1
