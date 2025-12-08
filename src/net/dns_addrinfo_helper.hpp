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

#ifndef H_NET_DNS_ADDRINFO_HELPER_HPP
#define H_NET_DNS_ADDRINFO_HELPER_HPP

#include <string>
#include "net/dns_message_response.hpp"

extern "C" struct addrinfo;

namespace net {

bool is_localhost(std::string_view host);
struct addrinfo* addrinfo_loopback(bool is_ipv6, int port);
struct addrinfo* addrinfo_dup(bool is_ipv6, const net::dns_message::response& response, int port);
void addrinfo_freedup(struct addrinfo* addrinfo);

}  // namespace net

#endif  // H_NET_DNS_ADDRINFO_HELPER_HPP
