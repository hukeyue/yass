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

#include <absl/flags/flag.h>
#include <build/build_config.h>
#include <gtest/gtest-message.h>
#include <gtest/gtest.h>

#include <gmock/gmock.h>

#include "config/config_network.hpp"
#include "net/doh_resolver.hpp"
#include "test_util.hpp"

ABSL_FLAG(bool, no_doh_tests, false, "skip doh tests");

using namespace net;

static void DoLocalResolve(asio::io_context& io_context, scoped_refptr<DoHResolver> resolver) {
  auto work_guard =
      std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(io_context.get_executor());

  io_context.restart();

  asio::post(io_context, [&]() {
    resolver->AsyncResolve("doh-test.localhost", 80,
                           [&](asio::error_code ec, asio::ip::tcp::resolver::results_type results) {
                             work_guard.reset();
                             ASSERT_FALSE(ec) << ec;
                             bool has_ipv6 = false;
                             for (auto iter = std::begin(results); iter != std::end(results); ++iter) {
                               const asio::ip::tcp::endpoint& endpoint = *iter;
                               auto addr = endpoint.address();
                               EXPECT_TRUE(addr.is_loopback()) << addr;
                               has_ipv6 |= addr.is_v6();
                             }
                             if (absl::GetFlag(FLAGS_ipv6_mode)) {
                               EXPECT_TRUE(has_ipv6) << "Expected IPv6 addresses on IPv6 mode";
                             }
                           });
  });

  EXPECT_NO_FATAL_FAILURE(io_context.run());
}

TEST(DOH_TEST, LocalBasic) {
  if (absl::GetFlag(FLAGS_no_doh_tests)) {
    GTEST_SKIP() << "skipped as required";
    return;
  }
  asio::error_code ec;
  asio::io_context io_context;

  auto resolver = DoHResolver::Create(io_context);
  int ret = resolver->Init("https://1.1.1.1/dns-query", 5000);
  ASSERT_EQ(ret, 0);

  DoLocalResolve(io_context, resolver);
}

static void DoRemoteResolve(asio::io_context& io_context, scoped_refptr<DoHResolver> resolver) {
  auto work_guard =
      std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(io_context.get_executor());

  io_context.restart();

  asio::post(io_context, [&]() {
    resolver->AsyncResolve("www.google.com", 80,
                           [&](asio::error_code ec, asio::ip::tcp::resolver::results_type results) {
                             work_guard.reset();
                             // Sometimes dns resolver don't get ack in time, ignore it safely
                             if (ec == asio::error::timed_out) {
                               return;
                             }
                             ASSERT_FALSE(ec) << ec;
                             for (auto iter = std::begin(results); iter != std::end(results); ++iter) {
                               const asio::ip::tcp::endpoint& endpoint = *iter;
                               auto addr = endpoint.address();
                               EXPECT_FALSE(addr.is_loopback()) << addr;
                               EXPECT_FALSE(addr.is_unspecified()) << addr;
                             }
                           });
  });

  EXPECT_NO_FATAL_FAILURE(io_context.run());
}

TEST(DOH_TEST, RemoteBasic) {
  if (absl::GetFlag(FLAGS_no_doh_tests)) {
    GTEST_SKIP() << "skipped as required";
    return;
  }
  asio::error_code ec;
  asio::io_context io_context;

  auto resolver = DoHResolver::Create(io_context);
  int ret = resolver->Init("https://1.1.1.1/dns-query", 5000);
  ASSERT_EQ(ret, 0);

  DoRemoteResolve(io_context, resolver);
}

TEST(DOH_TEST, RemoteMulti) {
  if (absl::GetFlag(FLAGS_no_doh_tests)) {
    GTEST_SKIP() << "skipped as required";
    return;
  }
  asio::error_code ec;
  asio::io_context io_context;

  auto resolver = DoHResolver::Create(io_context);
  int ret = resolver->Init("https://1.1.1.1/dns-query", 5000);
  ASSERT_EQ(ret, 0);

  DoRemoteResolve(io_context, resolver);
  DoRemoteResolve(io_context, resolver);
  DoRemoteResolve(io_context, resolver);
  DoRemoteResolve(io_context, resolver);
  DoRemoteResolve(io_context, resolver);
}

TEST(DOH_TEST, Timeout) {
  if (absl::GetFlag(FLAGS_no_doh_tests)) {
    GTEST_SKIP() << "skipped as required";
    return;
  }
  asio::error_code ec;
  asio::io_context io_context;

  auto resolver = DoHResolver::Create(io_context);
  int ret = resolver->Init("https://2.2.2.2/dns-query", 1);
  ASSERT_EQ(ret, 0);

  auto work_guard =
      std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(io_context.get_executor());

  asio::post(io_context, [&]() {
    resolver->AsyncResolve("www.google.com", 80,
                           [&](asio::error_code ec, asio::ip::tcp::resolver::results_type results) {
                             work_guard.reset();
                             ASSERT_EQ(ec, asio::error::timed_out) << ec;
                           });
  });

  EXPECT_NO_FATAL_FAILURE(io_context.run());
}
