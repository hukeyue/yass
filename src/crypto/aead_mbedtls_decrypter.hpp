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

#ifndef H_CRYPTO_AEAD_MBEDTLS_DECRYPTER
#define H_CRYPTO_AEAD_MBEDTLS_DECRYPTER

#include "crypto/aead_base_decrypter.hpp"
#include "crypto/crypter_export.hpp"

#ifdef HAVE_MBEDTLS

#include <mbedtls/cipher.h>

namespace crypto {

/// actually, it is stream cipher, not ahead cipher
class AeadMbedtlsDecrypter : public AeadBaseDecrypter {
 public:
  AeadMbedtlsDecrypter(cipher_method method,
                       mbedtls_cipher_context_t* evp,
                       size_t key_size,
                       size_t auth_tag_size,
                       size_t nonce_size);
  ~AeadMbedtlsDecrypter() override;

  uint32_t cipher_id() const override { return method_; }

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
  cipher_method method_;
  mbedtls_cipher_context_t* evp_;
};

}  // namespace crypto

#endif

#endif  // H_CRYPTO_AEAD_MBEDTLS_DECRYPTER
