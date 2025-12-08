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

#ifndef H_CRYPTO_AEAD_BASE_DECRYPTER
#define H_CRYPTO_AEAD_BASE_DECRYPTER

#include "crypto/decrypter.hpp"

#include <stddef.h>
#include <stdint.h>
#include <string>

namespace crypto {

class AeadBaseDecrypter : public Decrypter {
 public:
  AeadBaseDecrypter(size_t key_size, size_t auth_tag_size, size_t nonce_size);
  virtual ~AeadBaseDecrypter();

  bool SetKey(const char* key, size_t key_len) override;
  bool SetNoncePrefix(const char* nonce_prefix, size_t nonce_prefix_len) override;
  bool SetIV(const char* iv, size_t iv_len) override;
  bool SetPreliminaryKey(const char* key, size_t key_len) override;

  size_t GetKeySize() const override;
  size_t GetNoncePrefixSize() const override;
  size_t GetIVSize() const override;
  size_t GetTagSize() const override;

  const uint8_t* GetKey() const override;
  const uint8_t* GetIV() const override;
  const uint8_t* GetNoncePrefix() const override;

 protected:
  static const size_t kMaxKeySize = 64;
  enum : size_t { kMaxNonceSize = 32 };

  const size_t key_size_;
  const size_t auth_tag_size_;
  const size_t nonce_size_;
  bool have_preliminary_key_;

  // The Key.
  uint8_t key_[kMaxKeySize];
  // The IV used to construct the nonce.
  uint8_t iv_[kMaxNonceSize];
};

}  // namespace crypto

#endif  // H_CRYPTO_AEAD_BASE_DECRYPTER
