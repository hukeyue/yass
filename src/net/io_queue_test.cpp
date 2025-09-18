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

#include <build/build_config.h>
#include <gtest/gtest.h>

#include <vector>

#include "core/utils.hpp"
#include "net/io_buffer.hpp"
#include "net/io_queue.hpp"

using namespace net;

static constexpr unsigned int kDefaultDepth = 8u;
static constexpr unsigned int kBufferSize = 4096u;
static constexpr const char kBuffer[kBufferSize * 10] = {};

TEST(IoQueueTest, Construct) {
  IoQueue<GrowableIOBuffer, kDefaultDepth> queue;
  ASSERT_TRUE(queue.empty());
}

TEST(IoQueueTest, PushBackAndPopFrontVariant0) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> queue;
  for (unsigned int i = 0u; i < kDefaultDepth; ++i) {
    queue.push_back(kBuffer, kBufferSize);
  }
  ASSERT_EQ(kDefaultDepth, queue.length());
  ASSERT_EQ(kDefaultDepth * kBufferSize, queue.byte_length());
  for (unsigned int i = 0u; i < kDefaultDepth / 2; ++i) {
    queue.pop_front();
  }
  ASSERT_EQ(kDefaultDepth / 2, queue.length());
  ASSERT_EQ(kDefaultDepth / 2 * kBufferSize, queue.byte_length());
  for (unsigned int i = 0u; i < kDefaultDepth / 2; ++i) {
    queue.pop_front();
  }
  ASSERT_TRUE(queue.empty());
}

TEST(IoQueueTest, PushBackAndPopFrontVariant1) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> queue;
  for (unsigned int i = 0u; i < kDefaultDepth; ++i) {
    queue.push_back(GrowableIOBuffer::copyBuffer(kBuffer, kBufferSize));
  }
  ASSERT_EQ(kDefaultDepth, queue.length());
  ASSERT_EQ(kDefaultDepth * kBufferSize, queue.byte_length());
  for (unsigned int i = 0u; i < kDefaultDepth / 2; ++i) {
    queue.pop_front();
  }
  ASSERT_EQ(kDefaultDepth / 2, queue.length());
  ASSERT_EQ(kDefaultDepth / 2 * kBufferSize, queue.byte_length());
  for (unsigned int i = 0u; i < kDefaultDepth / 2; ++i) {
    queue.pop_front();
  }
  ASSERT_TRUE(queue.empty());
}

TEST(IoQueueTest, MoveConstruct) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> pending_data;
  for (unsigned int i = 0u; i < kDefaultDepth; ++i) {
    pending_data.push_back(kBuffer, kBufferSize);
  }
  auto queue = std::move(pending_data);
  ASSERT_TRUE(pending_data.empty());
  ASSERT_EQ(kDefaultDepth, queue.length());
}

TEST(IoQueueTest, MoveConstructOverInlinedStorage) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> pending_data;
  for (unsigned int i = 0u; i < kDefaultDepth * 2; ++i) {
    pending_data.push_back(kBuffer, kBufferSize);
  }
  auto queue = std::move(pending_data);
  ASSERT_TRUE(pending_data.empty());
  ASSERT_EQ(kDefaultDepth * 2, queue.length());
}

TEST(IoQueueTest, MoveAssignment) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> queue, pending_data;
  for (unsigned int i = 0u; i < kDefaultDepth; ++i) {
    queue.push_back(kBuffer, kBufferSize);
  }
  for (unsigned int i = 0u; i < kDefaultDepth; ++i) {
    pending_data.push_back(kBuffer, kBufferSize);
  }
  queue = std::move(pending_data);
  ASSERT_TRUE(pending_data.empty());
  ASSERT_EQ(kDefaultDepth, queue.length());
}

TEST(IoQueueTest, MoveAssignmentLhsOverInlinedStorage) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> pending_data, queue;
  for (unsigned int i = 0u; i < kDefaultDepth; ++i) {
    pending_data.push_back(kBuffer, kBufferSize);
  }
  for (unsigned int i = 0u; i < kDefaultDepth * 2; ++i) {
    queue.push_back(kBuffer, kBufferSize);
  }
  queue = std::move(pending_data);
  ASSERT_TRUE(pending_data.empty());
  ASSERT_EQ(kDefaultDepth, queue.length());
}

TEST(IoQueueTest, MoveAssignmentRhsOverInlinedStorage) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> pending_data, queue;
  for (unsigned int i = 0u; i < kDefaultDepth * 2; ++i) {
    pending_data.push_back(kBuffer, kBufferSize);
  }
  for (unsigned int i = 0u; i < kDefaultDepth; ++i) {
    queue.push_back(kBuffer, kBufferSize);
  }
  queue = std::move(pending_data);
  ASSERT_TRUE(pending_data.empty());
  ASSERT_EQ(kDefaultDepth * 2, queue.length());
}

TEST(IoQueueTest, MoveAssignmentBothOverInlinedStorage) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> pending_data, queue;
  for (unsigned int i = 0u; i < kDefaultDepth * 2; ++i) {
    pending_data.push_back(kBuffer, kBufferSize);
  }
  for (unsigned int i = 0u; i < kDefaultDepth * 2; ++i) {
    queue.push_back(kBuffer, kBufferSize);
  }
  queue = std::move(pending_data);
  ASSERT_TRUE(pending_data.empty());
  ASSERT_EQ(kDefaultDepth * 2, queue.length());
}

TEST(IoQueueTest, Clear) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> queue;
  for (unsigned int i = 0u; i < kDefaultDepth; ++i) {
    queue.push_back(kBuffer, kBufferSize);
  }
  queue.clear();
  ASSERT_TRUE(queue.empty());
  ASSERT_EQ(0u, queue.length());
  ASSERT_EQ(0u, queue.byte_length());
}

TEST(IoQueueTest, ClearOverInlinedStorage) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> queue;
  for (unsigned int i = 0u; i < kDefaultDepth * 2; ++i) {
    queue.push_back(kBuffer, kBufferSize);
  }
  queue.clear();
  ASSERT_TRUE(queue.empty());
  ASSERT_EQ(0u, queue.length());
}

TEST(IoQueueTest, SwapEmptyWith) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> queue, empty_queue;

  ASSERT_TRUE(queue.empty());
  for (unsigned int i = 0u; i < kDefaultDepth; ++i) {
    empty_queue.push_back(kBuffer, kBufferSize);
  }

  std::swap(queue, empty_queue);

  ASSERT_TRUE(empty_queue.empty());
  ASSERT_EQ(0u, empty_queue.length());

  ASSERT_EQ(kDefaultDepth, queue.length());
  ASSERT_EQ(kDefaultDepth * kBufferSize, queue.byte_length());
}

TEST(IoQueueTest, SwapWithEmpty) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> queue, empty_queue;

  for (unsigned int i = 0u; i < kDefaultDepth; ++i) {
    empty_queue.push_back(kBuffer, kBufferSize);
  }

  std::swap(empty_queue, queue);

  ASSERT_TRUE(empty_queue.empty());
  ASSERT_EQ(0u, empty_queue.length());

  ASSERT_EQ(kDefaultDepth, queue.length());
  ASSERT_EQ(kDefaultDepth * kBufferSize, queue.byte_length());
}

TEST(IoQueueTest, SwapNonEmpty) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 10> lhs, rhs;

  for (unsigned int i = 0u; i < kDefaultDepth * 3; ++i) {
    lhs.push_back(kBuffer, kBufferSize * 5);
  }
  for (unsigned int i = 0u; i < kDefaultDepth * 7; ++i) {
    rhs.push_back(kBuffer, kBufferSize * 9);
  }

  std::swap(lhs, rhs);

  ASSERT_EQ(7 * kDefaultDepth, lhs.length());
  ASSERT_EQ(7 * 9 * kDefaultDepth * kBufferSize, lhs.byte_length());

  ASSERT_EQ(3 * kDefaultDepth, rhs.length());
  ASSERT_EQ(3 * 5 * kDefaultDepth * kBufferSize, rhs.byte_length());
}

TEST(IoQueueTest, EnlargeVariant0) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> queue;
  ASSERT_TRUE(queue.empty());
  std::vector<scoped_refptr<GrowableIOBuffer>> v;

  // push idx_ to kDefaultDepth - 1
  for (unsigned int i = 0u; i < kDefaultDepth; ++i) {
    queue.push_back(kBuffer, kBufferSize);
    queue.pop_front();
  }
  ASSERT_TRUE(queue.empty());

  for (unsigned int i = 0u; i < kDefaultDepth * 2; ++i) {
    scoped_refptr<GrowableIOBuffer> buf = GrowableIOBuffer::copyBuffer(kBuffer, kBufferSize);
    v.push_back(buf);
    queue.push_back(buf);
  }

  ASSERT_EQ(kDefaultDepth * 2, v.size());
  ASSERT_EQ(kDefaultDepth * 2, queue.length());
  for (unsigned int i = 0u; i < kDefaultDepth * 2; ++i) {
    auto buf = queue.front();
    queue.pop_front();
    EXPECT_EQ(v[i].get(), buf.get());
  }
  ASSERT_TRUE(queue.empty());
}

TEST(IoQueueTest, EnlargeVariant1) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> queue;
  ASSERT_TRUE(queue.empty());
  std::vector<scoped_refptr<GrowableIOBuffer>> v;

  // push idx_ to 0

  for (unsigned int i = 0u; i < kDefaultDepth * 2; ++i) {
    scoped_refptr<GrowableIOBuffer> buf = GrowableIOBuffer::copyBuffer(kBuffer, kBufferSize);
    v.push_back(buf);
    queue.push_back(buf);
  }

  ASSERT_EQ(kDefaultDepth * 2, v.size());
  ASSERT_EQ(kDefaultDepth * 2, queue.length());
  for (unsigned int i = 0u; i < kDefaultDepth * 2; ++i) {
    auto buf = queue.front();
    queue.pop_front();
    EXPECT_EQ(v[i].get(), buf.get());
  }
  ASSERT_TRUE(queue.empty());
}

TEST(IoQueueTest, EnlargeVariant2) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> queue;
  ASSERT_TRUE(queue.empty());
  std::vector<scoped_refptr<GrowableIOBuffer>> v;

  // push idx_ to kDefaultDepth * 2 - 1
  for (unsigned int i = 0u; i < kDefaultDepth * 2 - 1; ++i) {
    queue.push_back(kBuffer, kBufferSize);
    queue.pop_front();
  }
  // ASSERT_EQ(queue.end_idx_, queue.length() - 1);
  ASSERT_TRUE(queue.empty());

  for (unsigned int i = 0u; i < kDefaultDepth * 2; ++i) {
    scoped_refptr<GrowableIOBuffer> buf = GrowableIOBuffer::copyBuffer(kBuffer, kBufferSize);
    v.push_back(buf);
    queue.push_back(buf);
  }

  ASSERT_EQ(kDefaultDepth * 2, v.size());
  ASSERT_EQ(kDefaultDepth * 2, queue.length());
  for (unsigned int i = 0u; i < kDefaultDepth * 2; ++i) {
    auto buf = queue.front();
    queue.pop_front();
    EXPECT_EQ(v[i].get(), buf.get());
  }
  ASSERT_TRUE(queue.empty());
}

TEST(IoQueueTest, EnlargeTwice) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> queue;
  ASSERT_TRUE(queue.empty());
  std::vector<scoped_refptr<GrowableIOBuffer>> v;

  for (unsigned int i = 0u; i < kDefaultDepth; ++i) {
    queue.push_back(kBuffer, kBufferSize);
    queue.pop_front();
  }

  for (unsigned int i = 0u; i < kDefaultDepth * 4; ++i) {
    scoped_refptr<GrowableIOBuffer> buf = GrowableIOBuffer::copyBuffer(kBuffer, kBufferSize);
    v.push_back(buf);
    queue.push_back(buf);
  }

  ASSERT_EQ(kDefaultDepth * 4, v.size());
  ASSERT_EQ(kDefaultDepth * 4, queue.length());
  for (unsigned int i = 0u; i < kDefaultDepth * 4; ++i) {
    auto buf = queue.front();
    queue.pop_front();
    EXPECT_EQ(v[i].get(), buf.get());
  }
  ASSERT_TRUE(queue.empty());
}

TEST(IoQueueTest, EnlargeThird) {
  IoQueue<GrowableIOBuffer, kDefaultDepth * 2> queue;
  ASSERT_TRUE(queue.empty());
  std::vector<scoped_refptr<GrowableIOBuffer>> v;

  for (unsigned int i = 0u; i < kDefaultDepth; ++i) {
    queue.push_back(kBuffer, kBufferSize);
    queue.pop_front();
  }

  for (unsigned int i = 0u; i < kDefaultDepth * 8; ++i) {
    scoped_refptr<GrowableIOBuffer> buf = GrowableIOBuffer::copyBuffer(kBuffer, kBufferSize);
    v.push_back(buf);
    queue.push_back(buf);
  }

  ASSERT_EQ(kDefaultDepth * 8, v.size());
  ASSERT_EQ(kDefaultDepth * 8, queue.length());
  for (unsigned int i = 0u; i < kDefaultDepth * 8; ++i) {
    auto buf = queue.front();
    queue.pop_front();
    EXPECT_EQ(v[i].get(), buf.get());
  }
  ASSERT_TRUE(queue.empty());
}
