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

#include "net/base64.hpp"

#include <base/strings/string_util.h>
#include <stddef.h>
#include "core/logging.hpp"
#include "core/span.hpp"

#include "third_party/modp_b64/modp_b64.h"

using namespace gurl_base;

namespace net {

namespace {

ModpDecodePolicy GetModpPolicy(Base64DecodePolicy policy) {
  switch (policy) {
    case Base64DecodePolicy::kStrict:
      return ModpDecodePolicy::kStrict;
    case Base64DecodePolicy::kForgiving:
      return ModpDecodePolicy::kForgiving;
  }
  DCHECK(false) << "Unexpect Base64 Decode Policy: " << (int)policy;
  return ModpDecodePolicy::kStrict;
}

}  // namespace

std::string Base64Encode(span<const uint8_t> input) {
  std::string output;
  Base64EncodeAppend(input, &output);
  return output;
}

void Base64EncodeAppend(span<const uint8_t> input, std::string* output) {
  // Ensure `modp_b64_encode_data_len` will not overflow.
  CHECK_LE(input.size(), MODP_B64_MAX_INPUT_LEN);
  size_t encode_data_len = modp_b64_encode_data_len(input.size());

  size_t prefix_len = output->size();
  output->resize(encode_data_len + prefix_len);

  const size_t output_size =
      modp_b64_encode_data(output->data() + prefix_len, reinterpret_cast<const char*>(input.data()), input.size());
  CHECK_EQ(output->size(), prefix_len + output_size);
}

bool Base64Decode(std::string_view input, std::string* output, Base64DecodePolicy policy) {
  std::string temp;
  temp.resize(modp_b64_decode_len(input.size()));

  // does not null terminate result since result is binary data!
  size_t input_size = input.size();
  size_t output_size = modp_b64_decode(&(temp[0]), input.data(), input_size, GetModpPolicy(policy));

  // Forgiving mode requires whitespace to be stripped prior to decoding.
  // We don't do that in the above code to ensure that the "happy path" of
  // input without whitespace is as fast as possible. Since whitespace in input
  // will always cause `modp_b64_decode` to fail, just handle whitespace
  // stripping on failure. This is not much slower than just scanning for
  // whitespace first, even for input with whitespace.
  if (output_size == MODP_B64_ERROR && policy == Base64DecodePolicy::kForgiving) {
    // We could use `output` here to avoid an allocation when decoding is done
    // in-place, but it violates the API contract that `output` is only modified
    // on success.
    std::string input_without_whitespace;
    RemoveChars(input, kInfraAsciiWhitespace, &input_without_whitespace);
    output_size = modp_b64_decode(&(temp[0]), input_without_whitespace.data(), input_without_whitespace.size(),
                                  GetModpPolicy(policy));
  }

  if (output_size == MODP_B64_ERROR)
    return false;

  temp.resize(output_size);
  output->swap(temp);
  return true;
}

std::optional<std::vector<uint8_t>> Base64Decode(std::string_view input) {
  std::vector<uint8_t> ret(modp_b64_decode_len(input.size()));

  size_t input_size = input.size();
  size_t output_size = modp_b64_decode(reinterpret_cast<char*>(ret.data()), input.data(), input_size);
  if (output_size == MODP_B64_ERROR)
    return std::nullopt;

  ret.resize(output_size);
  return ret;
}

}  // namespace net
