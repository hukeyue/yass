// SPDX-License-Identifier: GPL-2.0
/* Copyright (c) 2023-2024 Chilledheart  */

#include <gtest/gtest-message.h>
#include <gtest/gtest.h>

#include <gmock/gmock.h>
#include "net/padding.hpp"

#include "test_util.hpp"

using namespace net;

TEST(NetworkTest, AddAndRemovePadding) {
  uint8_t buffer[256];
  for (int i = 0; i < (int)sizeof(buffer); ++i) {
    buffer[i] = i & 255;
  }

  auto origin_buf = gurl_base::MakeRefCounted<WrappedIOBuffer>(reinterpret_cast<char*>(buffer), sizeof(buffer));
  auto send_buf = AddPadding(origin_buf.get());

  asio::error_code ec;
  auto recv_buf = RemovePadding(send_buf.get(), ec);
  EXPECT_FALSE(ec) << ec;
  EXPECT_EQ(send_buf->size(), 0);
  EXPECT_EQ((int)sizeof(buffer), recv_buf->size());

  ASSERT_EQ(::testing::Bytes(origin_buf->span()), ::testing::Bytes(recv_buf->span()));
}
