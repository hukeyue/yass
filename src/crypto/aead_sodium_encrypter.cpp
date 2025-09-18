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

#include "crypto/aead_sodium_encrypter.hpp"

#include "core/logging.hpp"
#include "net/protocol.hpp"

#include "third_party/boringssl/src/include/openssl/crypto.h"
#include "third_party/boringssl/src/include/openssl/err.h"

// In debug builds only, log OpenSSL error stack. Then clear OpenSSL error
// stack.
static void DLogOpenSslErrors() {
#if !DCHECK_IS_ON()
  ERR_clear_error();
#else
  while (uint32_t error = ERR_get_error()) {
    char buf[120];
    ERR_error_string_n(error, buf, sizeof(buf));
    DLOG(ERROR) << "OpenSSL error: " << buf;
  }
#endif
}

namespace crypto {

SodiumAeadEncrypter::SodiumAeadEncrypter(const EVP_AEAD* (*aead_getter)(),
                                         size_t key_size,
                                         size_t auth_tag_size,
                                         size_t nonce_size)
    : AeadBaseEncrypter(key_size, auth_tag_size, nonce_size), aead_alg_(aead_getter()) {
  DCHECK_EQ(EVP_AEAD_key_length(aead_alg_), key_size);
  DCHECK_EQ(EVP_AEAD_nonce_length(aead_alg_), nonce_size);
  DCHECK_GE(EVP_AEAD_max_tag_len(aead_alg_), auth_tag_size);
}

SodiumAeadEncrypter::~SodiumAeadEncrypter() = default;

bool SodiumAeadEncrypter::SetKey(const char* key, size_t key_len) {
  if (!AeadBaseEncrypter::SetKey(key, key_len)) {
    return false;
  }
  EVP_AEAD_CTX_cleanup(ctx_.get());
  if (!EVP_AEAD_CTX_init(ctx_.get(), aead_alg_, key_, key_size_, auth_tag_size_, nullptr)) {
    DLogOpenSslErrors();
    return false;
  }
  return true;
}

bool SodiumAeadEncrypter::Encrypt(const uint8_t* nonce,
                                  size_t nonce_len,
                                  const char* associated_data,
                                  size_t associated_data_len,
                                  const char* plaintext,
                                  size_t plaintext_len,
                                  uint8_t* output,
                                  size_t* output_length,
                                  size_t max_output_length) {
  DCHECK_EQ(nonce_len, nonce_size_);

  if (!EVP_AEAD_CTX_seal(ctx_.get(), output, output_length, max_output_length, nonce, nonce_len,
                         reinterpret_cast<const uint8_t*>(plaintext), plaintext_len,
                         reinterpret_cast<const uint8_t*>(associated_data), associated_data_len)) {
    DLogOpenSslErrors();
    return false;
  }

  return true;
}

bool SodiumAeadEncrypter::EncryptPacket(uint64_t packet_number,
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

  // for libsodium, packet number is written ahead
  PacketNumberToNonceSodium(nonce, nonce_size_, packet_number);

  DumpHex("EN-NONCE", nonce, nonce_size_);

  *output_length = max_output_length;

  if (!Encrypt(nonce, nonce_size_, associated_data, associated_data_len, plaintext, plaintext_len,
               reinterpret_cast<uint8_t*>(output), output_length, max_output_length)) {
    return false;
  }

  DCHECK_EQ(*output_length, ciphertext_size);
  return true;
}

}  // namespace crypto
