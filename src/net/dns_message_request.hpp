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

#ifndef H_NET_DNS_MESSAGE_REQUEST
#define H_NET_DNS_MESSAGE_REQUEST

#include "core/compiler_specific.hpp"
#include "core/logging.hpp"
#include "net/dns_message.hpp"
#include "net/protocol.hpp"

#include <stdint.h>
#include <string>
#include <vector>

namespace net {

namespace dns_message {

class request {
 public:
  request() : header_() {}

  bool init(const std::string& host_name, DNStype dnstype) {
    static_assert(sizeof(header) == 12);
    if (host_name.empty())
      return false;
    size_t hostlen = host_name.length();
    size_t expected_len = 12 + 1 + hostlen + 4;
    if (host_name[hostlen - 1] != '.')
      expected_len++;

    if (expected_len > (256 + 16)) /* RFCs 1034, 1035 */
      return /*DOH_DNS_NAME_TOO_LONG*/ false;

    header_.rd = 0x1;
    header_.qdcount = 0x0100;
    body_.resize(expected_len - 12);

    /* encode each label and store it in the QNAME */
    const char* hostp = host_name.c_str();
    char* dnsp = &body_[0];

    while (*hostp) {
      size_t labellen;
      const char* dot = strchr(hostp, '.');
      if (dot)
        labellen = dot - hostp;
      else
        labellen = strlen(hostp);
      if ((labellen > 63) || (!labellen)) {
        /* label is too long or too short, error out */
        return /*DOH_DNS_BAD_LABEL*/ false;
      }
      /* label is non-empty, process it */
      *dnsp++ = (unsigned char)labellen;
      memcpy(dnsp, hostp, labellen);
      dnsp += labellen;
      hostp += labellen;
      /* advance past dot, but only if there is one */
      if (dot)
        hostp++;
    } /* next label */

    *dnsp++ = 0; /* append zero-length label for root */

    /* There are assigned TYPE codes beyond 255: use range [1..65535]  */
    *dnsp++ = (unsigned char)(255 & (dnstype >> 8)); /* upper 8 bit TYPE */
    *dnsp++ = (unsigned char)(255 & dnstype);        /* lower 8 bit TYPE */

    *dnsp++ = '\0';         /* upper 8 bit CLASS */
    *dnsp++ = DNS_CLASS_IN; /* IN - "the Internet" */
    DCHECK_EQ(static_cast<size_t>(dnsp - &body_[0]) + 12, expected_len);

    return true;
  }

  std::array<asio::mutable_buffer, 2> buffers() {
    return {{asio::buffer(&header_, sizeof(header_)), asio::buffer(body_)}};
  }

 private:
  header header_;
  std::vector<char> body_;
};  // dns_message

}  // namespace dns_message

}  // namespace net

#endif  // H_NET_DNS_MESSAGE_REQUEST
