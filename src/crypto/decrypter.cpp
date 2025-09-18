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

#include "crypto/decrypter.hpp"

#include "crypto/aes_128_gcm_12_evp_decrypter.hpp"
#include "crypto/aes_128_gcm_evp_decrypter.hpp"
#include "crypto/aes_192_gcm_evp_decrypter.hpp"
#include "crypto/aes_256_gcm_evp_decrypter.hpp"
#include "crypto/aes_256_gcm_sodium_decrypter.hpp"
#include "crypto/chacha20_poly1305_evp_decrypter.hpp"
#include "crypto/chacha20_poly1305_sodium_decrypter.hpp"
#include "crypto/crypter_export.hpp"
#include "crypto/xchacha20_poly1305_evp_decrypter.hpp"
#include "crypto/xchacha20_poly1305_sodium_decrypter.hpp"

#include "crypto/aead_mbedtls_decrypter.hpp"
#include "crypto/mbedtls_common.hpp"

namespace crypto {

Decrypter::~Decrypter() = default;

std::unique_ptr<Decrypter> Decrypter::CreateFromCipherSuite(uint32_t cipher_suite) {
  switch (cipher_suite) {
    case CRYPTO_AES256GCMSHA256:
      return std::make_unique<Aes256GcmSodiumDecrypter>();
    case CRYPTO_CHACHA20POLY1305IETF:
      return std::make_unique<ChaCha20Poly1305SodiumDecrypter>();
    case CRYPTO_XCHACHA20POLY1305IETF:
      return std::make_unique<XChaCha20Poly1305SodiumDecrypter>();
    case CRYPTO_CHACHA20POLY1305IETF_EVP:
      return std::make_unique<ChaCha20Poly1305EvpDecrypter>();
    case CRYPTO_XCHACHA20POLY1305IETF_EVP:
      return std::make_unique<XChaCha20Poly1305EvpDecrypter>();
    case CRYPTO_AES128GCMSHA256_EVP:
      return std::make_unique<Aes128GcmEvpDecrypter>();
    case CRYPTO_AES128GCM12SHA256_EVP:
      return std::make_unique<Aes128Gcm12EvpDecrypter>();
    case CRYPTO_AES192GCMSHA256_EVP:
      return std::make_unique<Aes192GcmEvpDecrypter>();
    case CRYPTO_AES256GCMSHA256_EVP:
      return std::make_unique<Aes256GcmEvpDecrypter>();
#ifdef HAVE_MBEDTLS
#if 0
    case CRYPTO_RC4:
    case CRYPTO_RC4_MD5:
#endif
    case CRYPTO_AES_128_CFB:
    case CRYPTO_AES_192_CFB:
    case CRYPTO_AES_256_CFB:
    case CRYPTO_AES_128_CTR:
    case CRYPTO_AES_192_CTR:
    case CRYPTO_AES_256_CTR:
#if 0
    case CRYPTO_BF_CFB:
#endif
    case CRYPTO_CAMELLIA_128_CFB:
    case CRYPTO_CAMELLIA_192_CFB:
    case CRYPTO_CAMELLIA_256_CFB: {
      auto evp = mbedtls_create_evp(static_cast<cipher_method>(cipher_suite));
      auto key_len = mbedtls_get_key_size(static_cast<cipher_method>(cipher_suite));
      auto nonce_len = mbedtls_get_nonce_size(static_cast<cipher_method>(cipher_suite));
      return std::make_unique<AeadMbedtlsDecrypter>(static_cast<cipher_method>(cipher_suite), evp, key_len, 0,
                                                    nonce_len);
    }
#endif
    default:
      return nullptr;
  }
}

}  // namespace crypto
