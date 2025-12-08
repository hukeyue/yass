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

/* Copyright (c) 2024-2025 Chilledheart  */

#include "net/x509_util.hpp"

namespace net::x509_util {

namespace {

class BufferPoolSingleton {
 public:
  BufferPoolSingleton() : pool_(CRYPTO_BUFFER_POOL_new()) {}
  CRYPTO_BUFFER_POOL* pool() { return pool_; }

 private:
  // The singleton is leaky, so there is no need to use a smart pointer.
  CRYPTO_BUFFER_POOL* pool_;
};

}  // namespace

CRYPTO_BUFFER_POOL* GetBufferPool() {
  static BufferPoolSingleton g_buffer_pool_singleton;
  return g_buffer_pool_singleton.pool();
}

bssl::UniquePtr<CRYPTO_BUFFER> CreateCryptoBuffer(std::string_view data) {
  return bssl::UniquePtr<CRYPTO_BUFFER>(
      CRYPTO_BUFFER_new(reinterpret_cast<const uint8_t*>(data.data()), data.size(), GetBufferPool()));
}

}  // namespace net::x509_util
