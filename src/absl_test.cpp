// SPDX-License-Identifier: GPL-2.0
/* Copyright (c) 2025 Chilledheart  */

#include <gtest/gtest.h>

#include <absl/container/flat_hash_map.h>

typedef absl::flat_hash_map<int, int> HashMap;

class HashMapTest : public ::testing::TestWithParam<int> {};

TEST_P(HashMapTest, UseAfterMove) {
  const auto MaxCount = GetParam();

  HashMap connection_map_a, connection_map_b;
  for (ParamType i = 0; i < MaxCount; ++i) {
    connection_map_a[i] = i + 1;
  }
  connection_map_b = std::move(connection_map_a);
#if 1
  connection_map_a.clear();
#else
  connection_map_a = HashMap();
#endif
  EXPECT_TRUE(connection_map_a.empty());

  ParamType count = 0;
  for (auto [id, conn] : connection_map_b) {
    ++count;
    EXPECT_EQ(id + 1, conn);
  }
  EXPECT_EQ(MaxCount, count);
}

INSTANTIATE_TEST_SUITE_P(Abseil,
                         HashMapTest,
                         ::testing::Values(2, 10, 48, 1024, 65535),
                         ::testing::PrintToStringParamName());
