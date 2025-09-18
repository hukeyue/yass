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

#include <gtest/gtest-message.h>
#include <gtest/gtest.h>

#include <absl/flags/flag.h>
#include <base/rand_util.h>
#include <gmock/gmock.h>
#include "net/cipher.hpp"

#include "test_util.hpp"

using namespace net;

namespace {
scoped_refptr<GrowableIOBuffer> GenerateRandContent(int size) {
  auto buf = gurl_base::MakeRefCounted<GrowableIOBuffer>();
  buf->SetCapacity(size);

  gurl_base::RandBytes(buf->data(), std::min(256, size));
  for (int i = 1; i < size / 256; ++i) {
    memcpy(buf->data() + 256 * i, buf->data(), 256);
  }
  if (size % 256) {
    memcpy(buf->data() + 256 * (size / 256), buf->data(), std::min(256, size % 256));
  }

  return buf;
}
}  // anonymous namespace

class CipherTest : public ::testing::TestWithParam<size_t>, public cipher_visitor_interface {
 public:
  void SetUp() override {}
  void TearDown() override {}

  bool on_received_data(GrowableIOBuffer* buf) override {
    if (recv_buf_) {
      const int recv_buf_size = recv_buf_->RemainingCapacity() + buf->size();
      auto previous_chunk = recv_buf_;
      recv_buf_ = gurl_base::MakeRefCounted<GrowableIOBuffer>();
      recv_buf_->SetCapacity(recv_buf_size);
      memcpy(recv_buf_->data(), previous_chunk->data(), previous_chunk->RemainingCapacity());
      memcpy(recv_buf_->data() + previous_chunk->RemainingCapacity(), buf->data(), buf->RemainingCapacity());
    } else {
      recv_buf_ = buf;
    }

    return true;
  }

  void on_protocol_error() override { ec_ = asio::error::connection_aborted; }

 protected:
  void EncodeAndDecode(const std::string& key, const std::string& password, cipher_method crypto_method, size_t size) {
    auto encoder = std::make_unique<cipher>(key, password, crypto_method, this, true);
    auto decoder = std::make_unique<cipher>(key, password, crypto_method, this, false);
    auto send_buf = GenerateRandContent(size);

    auto cipherbuf = gurl_base::MakeRefCounted<GrowableIOBuffer>();

    encoder->encrypt(send_buf->bytes(), send_buf->size(), cipherbuf.get());
    decoder->process_bytes(cipherbuf.get());
    ASSERT_EQ(ec_, asio::error_code());

    ASSERT_EQ(send_buf->capacity(), recv_buf_->capacity());
    ASSERT_EQ(::testing::Bytes(send_buf->everything()), ::testing::Bytes(recv_buf_->everything()));
  }

  asio::error_code ec_;
  scoped_refptr<GrowableIOBuffer> recv_buf_;
};

#define XX(num, name, string)                                           \
  TEST_P(CipherTest, name) {                                            \
    EncodeAndDecode("", "<dummy-password>", CRYPTO_##name, GetParam()); \
  }

CIPHER_METHOD_OLD_MAP(XX)
#undef XX

INSTANTIATE_TEST_SUITE_P(SizedCipherTest,
                         CipherTest,
                         ::testing::Values(16, 256, 512, 1024, 2048, 4096, 16 * 1024 - 1),
                         ::testing::PrintToStringParamName());
