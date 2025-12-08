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

#include "core/span.hpp"

#ifndef _TEST_UTIL_H
#define _TEST_UTIL_H

#include <cstring>
#include <iosfwd>

namespace testing {

// hexdump writes |msg| to |fp| followed by the hex encoding of |len| bytes
// from |in|.
void hexdump(FILE* fp, const char* msg, const void* in, size_t len);

// Bytes is a wrapper over a byte slice which may be compared for equality. This
// allows it to be used in EXPECT_EQ macros.
struct Bytes {
  Bytes(const uint8_t* data_arg, size_t len_arg) : span_(data_arg, len_arg) {}
  Bytes(const char* data_arg, size_t len_arg) : span_(reinterpret_cast<const uint8_t*>(data_arg), len_arg) {}

  Bytes(const Bytes&) = default;
  Bytes& operator=(const Bytes&) = default;

  explicit Bytes(const char* str) : span_(reinterpret_cast<const uint8_t*>(str), strlen(str)) {}
  explicit Bytes(const std::string& str) : span_(reinterpret_cast<const uint8_t*>(str.data()), str.size()) {}
  explicit Bytes(span<const uint8_t> span) : span_(span) {}

  span<const uint8_t> span_;
};

// DecodeHex decodes |in| from hexadecimal and writes the output to |out|. It
// returns true on success and false if |in| is not a valid hexadecimal byte
// string.
bool DecodeHex(std::vector<uint8_t>* out, const std::string& in);

// EncodeHex returns |in| encoded in hexadecimal.
std::string EncodeHex(span<const uint8_t> in);

inline bool operator==(const ::testing::Bytes& a, const ::testing::Bytes& b) {
  return a.span_.size_bytes() == b.span_.size_bytes() &&
         memcmp(a.span_.data(), b.span_.data(), a.span_.size_bytes()) == 0;
}

inline bool operator!=(const ::testing::Bytes& a, const ::testing::Bytes& b) {
  return !(a == b);
}

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, ::testing::Bytes in) {
  if (in.span_.empty()) {
    return os << "<Empty Bytes>";
  }

  // Print a byte slice as hex.
  os << ::testing::EncodeHex(in.span_);
  return os;
}

}  // namespace testing

#endif  // _TEST_UTIL_H
