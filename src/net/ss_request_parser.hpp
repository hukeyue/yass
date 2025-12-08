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

#ifndef H_NET_SS_REQUEST_PARSER
#define H_NET_SS_REQUEST_PARSER

#include <cstdlib>

#include "core/logging.hpp"
#include "net/ss_request.hpp"

namespace net {

namespace ss {
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
      case request_address_start:
        if (end - i < (int)sizeof(uint8_t)) {
          return std::make_tuple(indeterminate, i);
        }
        memcpy(&req.atyp_req_.address_type, &*i, sizeof(uint8_t));
        ++i;
        if (req.address_type() != ipv4 && req.address_type() != domain && req.address_type() != ipv6) {
          return std::make_tuple(bad, i);
        }
        size_t address_type_size = req.address_type_size();
        if (end - i < (int)address_type_size) {
          return std::make_tuple(indeterminate, i);
        }
        /* deal with header, variable part */
        switch (req.address_type()) {
          case ipv4:
            memcpy(&req.atyp_req_.address4, &*i, sizeof(asio::ip::address_v4::bytes_type));
            i += sizeof(asio::ip::address_v4::bytes_type);

            req.port_high_byte() = *i;
            i += sizeof(uint8_t);
            req.port_low_byte() = *i;
            i += sizeof(uint8_t);
            break;
          case domain:
            memcpy(&req.atyp_req_.domain.domain_name_len, &*i, sizeof(uint8_t));
            if (end - i < (int)req.atyp_req_.domain.domain_name_len + (int)sizeof(uint16_t)) {
              return std::make_tuple(indeterminate, i);
            }
            i += sizeof(uint8_t);

            memcpy(req.atyp_req_.domain.domain_name, &*i, req.atyp_req_.domain.domain_name_len);
            i += req.atyp_req_.domain.domain_name_len;

            req.port_high_byte() = *i;
            i += sizeof(uint8_t);
            req.port_low_byte() = *i;
            i += sizeof(uint8_t);
            break;
          case ipv6:
            memcpy(&req.atyp_req_.address6, &*i, sizeof(asio::ip::address_v6::bytes_type));
            i += sizeof(asio::ip::address_v6::bytes_type);

            req.port_high_byte() = *i;
            i += sizeof(uint8_t);
            req.port_low_byte() = *i;
            i += sizeof(uint8_t);
            break;
          default:
            return std::make_tuple(bad, i);
        }

        if (req.address_type() == domain) {
          VLOG(3) << "ss: adt: 0x" << std::hex << (int)req.address_type() << std::dec << " addr: " << req.domain_name()
                  << " port: " << req.port();
          ;
        } else {
          VLOG(3) << "ss: adt: 0x" << std::hex << (int)req.address_type() << std::dec << " addr: " << req.endpoint();
        }
        return std::make_tuple(good, i);
    }
    return std::make_tuple(bad, i);
  }

 private:
  enum state {
    request_address_start,
  } state_;
};

}  // namespace ss

}  // namespace net

#endif  // H_NET_SS_REQUEST_PARSER
