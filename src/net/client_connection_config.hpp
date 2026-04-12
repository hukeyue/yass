// SPDX-License-Identifier: GPL-2.0 OR LGPL-2.1 OR CDDL-1.0
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

/* Copyright (c) 2019-2026 Chilledheart  */

#ifndef H_NET_CLIENT_CONNECTION_CONFIG
#define H_NET_CLIENT_CONNECTION_CONFIG

#include <string>

#include "crypto/crypter_export.hpp"

namespace net {

struct ClientConnectionConfig {
  ClientConnectionConfig();
  ClientConnectionConfig(const ClientConnectionConfig& other);
  ClientConnectionConfig(ClientConnectionConfig&& other);
  ~ClientConnectionConfig();
  ClientConnectionConfig& operator=(const ClientConnectionConfig&);
  ClientConnectionConfig& operator=(ClientConnectionConfig&&);

  std::string host_ips;
  std::string host_sni;
  uint16_t port = 0;
  std::string username;
  std::string password;
  cipher_method cipher = CRYPTO_INVALID;
  bool padding_support = true;
};

}  // namespace net

#endif  // H_NET_CLIENT_CONNECTION_CONFIG
