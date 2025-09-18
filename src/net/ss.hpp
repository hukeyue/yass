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

#ifndef H_NET_SS
#define H_NET_SS

#include "core/compiler_specific.hpp"

#include <stdint.h>

#include "net/asio.hpp"

namespace net {

namespace ss {

//
// Shadowsocks TCP Relay Header:
//
//    +------+----------+----------+
//    | ATYP | DST.ADDR | DST.PORT |
//    +------+----------+----------+
//    |  1   | Variable |    2     |
//    +------+----------+----------+
//
//
// In an address field (DST.ADDR, BND.ADDR), the ATYP field specifies
//    the type of address contained within the field:
//
//           o  X'01'
//
//    the address is a version-4 IP address, with a length of 4 octets
//
//           o  X'03'
//
//    the address field contains a fully-qualified domain name.  The first
//    octet of the address field contains the number of octets of name that
//    follow, there is no terminating NUL octet.
//
//           o  X'04'
//
//    the address is a version-6 IP address, with a length of 16 octets.
enum address_type {
  ipv4 = 0x01,
  domain = 0x03,
  ipv6 = 0x04,
};

// +------+----------+----------+
// | ATYP | DST.ADDR | DST.PORT |
// +------+----------+----------+
// |  1   | Variable |    2     |
// +------+----------+----------+
//
//  ATYP   address type of following address
//  o  IP V4 address: X'01'
//  o  DOMAINNAME: X'03'
//  o  IP V6 address: X'04'
//  DST.ADDR       desired destination address
//  DST.PORT desired destination port in network octet
//  order
struct address_type_domain_header {
  uint8_t domain_name_len;
  uint8_t domain_name[255];
};
struct address_type_header {
  uint8_t address_type;
  union {
    asio::ip::address_v4::bytes_type address4;
    asio::ip::address_v6::bytes_type address6;
    address_type_domain_header domain;
  };
  uint8_t port_high_byte;
  uint8_t port_low_byte;
};

}  // namespace ss

}  // namespace net
#endif  // H_NET_SS
