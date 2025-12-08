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

#include "crypto/aead_mbedtls_encrypter.hpp"

#include "core/logging.hpp"
#include "net/protocol.hpp"

#ifdef HAVE_MBEDTLS

#include "crypto/mbedtls_common.hpp"

namespace crypto {

AeadMbedtlsEncrypter::AeadMbedtlsEncrypter(cipher_method method,
                                           mbedtls_cipher_context_t* evp,
                                           size_t key_size,
                                           size_t auth_tag_size,
                                           size_t nonce_size)
    : AeadBaseEncrypter(key_size, auth_tag_size, nonce_size), method_(method), evp_(evp) {
#if 0
  DCHECK_EQ(EVP_AEAD_key_length(aead_alg_), key_size);
  DCHECK_EQ(EVP_AEAD_nonce_length(aead_alg_), nonce_size);
  DCHECK_GE(EVP_AEAD_max_tag_len(aead_alg_), auth_tag_size);
#endif
}

AeadMbedtlsEncrypter::~AeadMbedtlsEncrypter() {
  mbedtls_release_evp(evp_);
}

bool AeadMbedtlsEncrypter::SetKey(const char* key, size_t key_len) {
  if (!AeadBaseEncrypter::SetKey(key, key_len)) {
    return false;
  }
  if (mbedtls_cipher_setkey(evp_, (const uint8_t*)key, key_len * 8, MBEDTLS_ENCRYPT) != 0) {
    return false;
  }
  if (mbedtls_cipher_set_iv(evp_, iv_, nonce_size_) != 0) {
    return false;
  }
  if (mbedtls_cipher_reset(evp_) != 0) {
    return false;
  }

  return true;
}

bool AeadMbedtlsEncrypter::Encrypt(const uint8_t* nonce,
                                   size_t nonce_len,
                                   const char* associated_data,
                                   size_t associated_data_len,
                                   const char* plaintext,
                                   size_t plaintext_len,
                                   uint8_t* output,
                                   size_t* output_length,
                                   size_t max_output_length) {
  DCHECK_EQ(nonce_len, nonce_size_);

  if (mbedtls_cipher_update(evp_, reinterpret_cast<const uint8_t*>(plaintext), plaintext_len, output, output_length) !=
      0) {
    return false;
  }

  return true;
}

bool AeadMbedtlsEncrypter::EncryptPacket(uint64_t packet_number,
                                         const char* associated_data,
                                         size_t associated_data_len,
                                         const char* plaintext,
                                         size_t plaintext_len,
                                         char* output,
                                         size_t* output_length,
                                         size_t max_output_length) {
  size_t ciphertext_size = GetCiphertextSize(plaintext_len);
  if (max_output_length < ciphertext_size) {
    return false;
  }
  uint8_t nonce[kMaxNonceSize] = {};
  memcpy(nonce, iv_, nonce_size_);

  *output_length = max_output_length;

  if (!Encrypt(nonce, nonce_size_, associated_data, associated_data_len, plaintext, plaintext_len,
               reinterpret_cast<uint8_t*>(output), output_length, max_output_length)) {
    return false;
  }

  DCHECK_EQ(*output_length, ciphertext_size);
  return true;
}

}  // namespace crypto

#endif  // HAVE_MBEDTLS
