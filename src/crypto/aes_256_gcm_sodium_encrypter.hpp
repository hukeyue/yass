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

/* Copyright (c) 2022-2025 Chilledheart  */

#ifndef H_CRYPTO_AES256_GCM_SODIUM_ENCRYPTER
#define H_CRYPTO_AES256_GCM_SODIUM_ENCRYPTER

#include <stddef.h>
#include <stdint.h>
#include <string>

#include "crypto/aead_sodium_encrypter.hpp"

namespace crypto {

class Aes256GcmSodiumEncrypter : public SodiumAeadEncrypter {
 public:
  enum : size_t {
    kAuthTagSize = 16,
  };

  Aes256GcmSodiumEncrypter();
  ~Aes256GcmSodiumEncrypter() override;

  uint32_t cipher_id() const override;
};

}  // namespace crypto

#endif  // H_CRYPTO_AES256_GCM_SODIUM_ENCRYPTER
