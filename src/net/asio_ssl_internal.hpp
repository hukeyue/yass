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

/* Copyright (c) 2024-2026 Chilledheart  */

#ifndef H_NET_ASIO_SSL_INTERNAL
#define H_NET_ASIO_SSL_INTERNAL

#include "net/asio.hpp"

#include <string_view>

constexpr const size_t kMaxBinaryCaBundleBuffer = 384 * 1024;
constexpr const size_t kMaxSupplementaryBinaryCaBundleBuffer = 16 * 1024;

int get_binary_ca_bundle(std::string* output);
int get_binary_supplementary_ca_bundle(std::string* output);

int load_ca_to_ssl_ctx_from_mem(SSL_CTX* ssl_ctx, std::string_view cadata);
int load_ca_to_ssl_ctx_from_system(SSL_CTX* ssl_ctx);
int load_ca_to_ssl_ctx_from_system_extra(SSL_CTX* ssl_ctx);

#endif  // H_NET_ASIO_SSL_INTERNAL
