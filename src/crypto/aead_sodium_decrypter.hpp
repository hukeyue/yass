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

#ifndef H_CRYPTO_AEAD_SODIUM_DECRYPTER
#define H_CRYPTO_AEAD_SODIUM_DECRYPTER

#include "aead_base_decrypter.hpp"

#include "third_party/boringssl/src/include/openssl/aead.h"

namespace crypto {

class AeadSodiumDecrypter : public AeadBaseDecrypter {
 public:
  AeadSodiumDecrypter(const EVP_AEAD* (*aead_getter)(), size_t key_size, size_t auth_tag_size, size_t nonce_size);
  ~AeadSodiumDecrypter() override;

  bool SetKey(const char* key, size_t key_len) override;

  bool DecryptPacket(uint64_t packet_number,
                     const char* associated_data,
                     size_t associated_data_len,
                     const char* ciphertext,
                     size_t ciphertext_len,
                     char* output,
                     size_t* output_length,
                     size_t max_output_length) override;

 private:
  const EVP_AEAD* const aead_alg_;
  bssl::ScopedEVP_AEAD_CTX ctx_;
};

}  // namespace crypto

#endif  // H_CRYPTO_AEAD_SODIUM_DECRYPTER
