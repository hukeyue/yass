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

#ifndef H_NET_DNS_MESSAGE_RESPONSE
#define H_NET_DNS_MESSAGE_RESPONSE

#include "core/compiler_specific.hpp"
#include "core/logging.hpp"
#include "net/dns_message.hpp"
#include "net/protocol.hpp"

#include <stdint.h>
#include <string>
#include <vector>

namespace net {

namespace dns_message {
// Resource record format
//                                    1  1  1  1  1  1
//      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                                               |
//    /                                               /
//    /                      NAME                     /
//    |                                               |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                      TYPE                     |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                     CLASS                     |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                      TTL                      |
//    |                                               |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                   RDLENGTH                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
//    /                     RDATA                     /
//    /                                               /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

class response {
 public:
  response() : header_() {}

  uint16_t id() const {
    auto doh = reinterpret_cast<const uint8_t*>(&header_.id);
    return (unsigned short)((doh[0] << 8) | doh[1]);
  }

  const std::vector<asio::ip::address_v4>& a() const { return a_; }
  const std::vector<asio::ip::address_v6>& aaaa() const { return aaaa_; }
  const std::vector<std::string>& cname() const { return cname_; }

 private:
  friend class response_parser;
  header header_;
  std::vector<asio::ip::address_v4> a_;
  std::vector<asio::ip::address_v6> aaaa_;
  std::vector<std::string> cname_;
};  // dns_message

}  // namespace dns_message

}  // namespace net

#endif  // H_NET_DNS_MESSAGE_RESPONSE
