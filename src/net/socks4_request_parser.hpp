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

#ifndef H_NET_SOCKS4_REQUEST_PARSER
#define H_NET_SOCKS4_REQUEST_PARSER

#include <cstdlib>

#include "core/logging.hpp"
#include "net/socks4_request.hpp"

namespace net {

namespace socks4 {
class request;
class request_parser {
 public:
  /// Construct ready to parse the request method.
  request_parser();

  /// Reset to initial parser state.
  void reset();

  /// Result of parse.
  enum result_type { good, bad, indeterminate };

  /// Parse some data. The enum return value is good when a complete request has
  /// been parsed, bad if the data is invalid, indeterminate when more data is
  /// required. The InputIterator return value indicates how much of the input
  /// has been consumed.
  template <typename InputIterator>
  std::tuple<result_type, InputIterator> parse(request& req, InputIterator begin, InputIterator end) {
    InputIterator i = begin;
    switch (state_) {
      case request_start:
        if (end - i < (int)sizeof(request_header)) {
          return std::make_tuple(indeterminate, i);
        }
        memcpy(&req.req_, &*i, sizeof(request_header));
        if (req.version() != version) {
          return std::make_tuple(bad, i);
        }
        VLOG(3) << "socks4: anom request:" << std::hex << " ver: 0x" << (int)req.version() << " cmd: 0x"
                << (int)req.command() << std::dec << " addr: " << req.endpoint() << " is_socks4a: " << std::boolalpha
                << req.is_socks4a() << std::dec;

        i += sizeof(request_header);
        state_ = request_userid_start;
        return parse(req, i, end);
      case request_userid_start:
        while (i != end && *i != '\0') {
          ++i;
        }
        if (i == end) {
          return std::make_tuple(indeterminate, i);
        }
        VLOG(3) << "socks4: user id: " << std::hex << (int)*begin;
        req.user_id_ = std::string(begin, i);
        ++i;
        if (req.is_socks4a()) {
          state_ = request_domain_start;
          return parse(req, i, end);
        }
        break;
      case request_domain_start:
        while (i != end && *i != '\0') {
          ++i;
        }
        if (i == end) {
          return std::make_tuple(indeterminate, i);
        }
        VLOG(3) << "socks4: domain_name: " << begin;
        req.domain_name_ = std::string(begin, i);
        ++i;
        break;
      default:
        return std::make_tuple(bad, i);
    }
    return std::make_tuple(good, i);
  }

 private:
  enum state {
    request_start,
    request_userid_start,
    request_domain_start,
  } state_;
};

}  // namespace socks4

}  // namespace net

#endif  // H_NET_SOCKS4_REQUEST_PARSER
