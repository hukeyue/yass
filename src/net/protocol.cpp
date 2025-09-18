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

#include "net/protocol.hpp"

#include <string_view>

using std::string_view_literals::operator""sv;

namespace net {

NextProto NextProtoFromString(std::string_view proto_string) {
  if (proto_string == "http/1.1"sv) {
    return kProtoHTTP11;
  }
  if (proto_string == "h2"sv) {
    return kProtoHTTP2;
  }
  if (proto_string == "quic"sv || proto_string == "hq"sv) {
    return kProtoQUIC;
  }

  return kProtoUnknown;
}

std::string_view NextProtoToString(NextProto next_proto) {
  switch (next_proto) {
    case kProtoHTTP11:
      return "http/1.1"sv;
    case kProtoHTTP2:
      return "h2"sv;
    case kProtoQUIC:
      return "quic"sv;
    case kProtoUnknown:
      break;
  }
  return "unknown";
}

}  // namespace net
