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

#ifndef H_NET_SOCKS5_REQUEST
#define H_NET_SOCKS5_REQUEST

#include "net/protocol.hpp"
#include "net/socks5.hpp"

#include <stdint.h>

namespace net {

namespace socks5 {

class method_select_request {
 public:
  method_select_request() : req_() {}

  uint8_t ver() const { return req_.ver; }
  uint8_t nmethods() const { return req_.nmethods; }

  typedef const uint8_t* const_iterator;
  const_iterator begin() const { return methods_; }
  const_iterator end() const { return &methods_[req_.nmethods]; }

  size_t length() const { return sizeof(req_) + req_.nmethods; }

 private:
  friend class method_select_request_parser;
  method_select_request_header req_;
  uint8_t methods_[255];
};

class auth_request {
 public:
  auth_request() : req_() {}

  uint8_t ver() const { return req_.ver; }
  const std::string& username() const { return username_; }
  const std::string& password() const { return password_; }

  size_t length() const { return sizeof(req_) + 1 + username_.size() + 1 + password_.size(); }

 private:
  friend class auth_request_parser;
  auth_request_header req_;
  std::string username_;
  std::string password_;
};

class request {
 public:
  request() : req_(), atyp_req_() {}

  uint8_t version() const { return req_.version; }
  uint8_t command() const { return req_.command; }
  uint8_t address_type() const { return atyp_req_.address_type; }

  size_t address_type_size() const {
    switch (address_type()) {
      case ipv4:
        return sizeof(asio::ip::address_v4::bytes_type) + sizeof(uint16_t);
      case domain:
        return sizeof(uint8_t) + atyp_req_.domain.domain_name_len + sizeof(uint16_t);
      case ipv6:
        return sizeof(asio::ip::address_v6::bytes_type) + sizeof(uint16_t);
      default:
        return 0;
    }
  }

  size_t length() const { return sizeof(req_) + sizeof(uint8_t) + address_type_size(); }

  asio::ip::tcp::endpoint endpoint() const {
    asio::ip::tcp::endpoint endpoint;
    if (address_type() == ipv4) {
      asio::ip::address_v4 address(atyp_req_.address4);
      endpoint = asio::ip::tcp::endpoint(address, port());
    } else if (address_type() == ipv6) {
      asio::ip::address_v6 address(atyp_req_.address6);
      endpoint = asio::ip::tcp::endpoint(address, port());
    }
    return endpoint;
  }

  const asio::ip::address_v4::bytes_type& address4() const { return atyp_req_.address4; }

  const asio::ip::address_v6::bytes_type& address6() const { return atyp_req_.address6; }

  std::string domain_name() const {
    return std::string((char*)atyp_req_.domain.domain_name, atyp_req_.domain.domain_name_len);
  }

  uint16_t port() const {
    unsigned short port = atyp_req_.port_high_byte;
    port = (port << 8) & 0xff00;
    port = port | atyp_req_.port_low_byte;
    return port;
  }

 private:
  friend class request_parser;
  request_header req_;
  address_type_header atyp_req_;
};
}  // namespace socks5

}  // namespace net

#endif  // H_NET_SOCKS5_REQUEST
