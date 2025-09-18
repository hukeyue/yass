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

#ifndef NET_BASE64_H_
#define NET_BASE64_H_

#include <stdint.h>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "core/span.hpp"

namespace net {

// Encodes the input binary data in base64.
std::string Base64Encode(span<const uint8_t> input);

// Encodes the input binary data in base64 and appends it to the output.
void Base64EncodeAppend(span<const uint8_t> input, std::string* output);

// Decodes the base64 input string.  Returns true if successful and false
// otherwise. The output string is only modified if successful. The decoding can
// be done in-place.
enum class Base64DecodePolicy {
  // Input should match the output format of Base64Encode. i.e.
  // - Input length should be divisible by 4
  // - Maximum of 2 padding characters
  // - No non-base64 characters.
  kStrict,

  // Matches https://infra.spec.whatwg.org/#forgiving-base64-decode.
  // - Removes all ascii whitespace
  // - Maximum of 2 padding characters
  // - Allows input length not divisible by 4 if no padding chars are added.
  kForgiving,
};
bool Base64Decode(std::string_view input, std::string* output, Base64DecodePolicy policy = Base64DecodePolicy::kStrict);

// Decodes the base64 input string. Returns `std::nullopt` if unsuccessful.
std::optional<std::vector<uint8_t>> Base64Decode(std::string_view input);

}  // namespace net

#endif  // NET_BASE64_H_
