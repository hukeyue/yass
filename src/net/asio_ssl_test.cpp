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

#include <build/build_config.h>
#include <gtest/gtest.h>

#include "core/utils.hpp"
#include "net/asio_ssl_internal.hpp"

TEST(SSL_TEST, LoadBuiltinCaBundle) {
  bssl::UniquePtr<SSL_CTX> ssl_ctx;
  ssl_ctx.reset(::SSL_CTX_new(::TLS_client_method()));
  std::string_view ca_bundle_content(_binary_ca_bundle_crt_start,
                                     _binary_ca_bundle_crt_end - _binary_ca_bundle_crt_start);
  ASSERT_FALSE(ca_bundle_content.empty());
  int result = load_ca_to_ssl_ctx_from_mem(ssl_ctx.get(), ca_bundle_content);
  ASSERT_NE(result, 0);
}

TEST(SSL_TEST, LoadSupplementaryCaBundle) {
  bssl::UniquePtr<SSL_CTX> ssl_ctx;
  ssl_ctx.reset(::SSL_CTX_new(::TLS_client_method()));
  std::string_view ca_content(_binary_supplementary_ca_bundle_crt_start,
                              _binary_supplementary_ca_bundle_crt_end - _binary_supplementary_ca_bundle_crt_start);
  ASSERT_FALSE(ca_content.empty());
  int result = load_ca_to_ssl_ctx_from_mem(ssl_ctx.get(), ca_content);
  ASSERT_NE(result, 0);
}

TEST(SSL_TEST, LoadSystemCa) {
  bssl::UniquePtr<SSL_CTX> ssl_ctx;
  ssl_ctx.reset(::SSL_CTX_new(::TLS_client_method()));
  int result = load_ca_to_ssl_ctx_from_system(ssl_ctx.get());
#if BUILDFLAG(IS_IOS)
  // we don't test on iOS
  GTEST_SKIP() << "skipped as system is not supported";
#else
  ASSERT_NE(result, 0);
#endif
}
