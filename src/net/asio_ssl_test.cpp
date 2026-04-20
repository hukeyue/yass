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

#include <build/build_config.h>
#include <gtest/gtest.h>

#include "core/utils.hpp"
#include "net/asio_ssl_internal.hpp"
#include "net/x509_util.hpp"

TEST(SSL_TEST, LoadBuiltinCaBundle) {
  bssl::UniquePtr<SSL_CTX> ssl_ctx;
  ssl_ctx.reset(::SSL_CTX_new(::TLS_client_method()));
  // Deduplicate all certificates minted from the SSL_CTX in memory.
  SSL_CTX_set0_buffer_pool(ssl_ctx.get(), net::x509_util::GetBufferPool());

  std::string ca_bundle_content;
  ca_bundle_content.reserve(kMaxBinaryCaBundleBuffer);
  ASSERT_EQ(0, get_binary_ca_bundle(&ca_bundle_content));
  ASSERT_LE(ca_bundle_content.size(), kMaxBinaryCaBundleBuffer);
  ASSERT_FALSE(ca_bundle_content.empty());
  int result = load_ca_to_ssl_ctx_from_mem(ssl_ctx.get(), ca_bundle_content);
  ASSERT_NE(result, 0);
}

TEST(SSL_TEST, LoadSupplementaryCaBundle) {
  bssl::UniquePtr<SSL_CTX> ssl_ctx;
  ssl_ctx.reset(::SSL_CTX_new(::TLS_client_method()));
  // Deduplicate all certificates minted from the SSL_CTX in memory.
  SSL_CTX_set0_buffer_pool(ssl_ctx.get(), net::x509_util::GetBufferPool());

  std::string ca_content;
  ca_content.reserve(kMaxSupplementaryBinaryCaBundleBuffer);
  ASSERT_EQ(0, get_binary_supplementary_ca_bundle(&ca_content));
  ASSERT_LE(ca_content.size(), kMaxSupplementaryBinaryCaBundleBuffer);
  ASSERT_FALSE(ca_content.empty());
  int result = load_ca_to_ssl_ctx_from_mem(ssl_ctx.get(), ca_content);
  ASSERT_NE(result, 0);
}

TEST(SSL_TEST, LoadSystemCa) {
  bssl::UniquePtr<SSL_CTX> ssl_ctx;
  ssl_ctx.reset(::SSL_CTX_new(::TLS_client_method()));
  // Deduplicate all certificates minted from the SSL_CTX in memory.
  SSL_CTX_set0_buffer_pool(ssl_ctx.get(), net::x509_util::GetBufferPool());

  int result = load_ca_to_ssl_ctx_from_system(ssl_ctx.get());
#if BUILDFLAG(IS_IOS)
  // we don't test on iOS
  GTEST_SKIP() << "skipped as system is not supported";
#else
  ASSERT_NE(result, 0);
#endif
}
