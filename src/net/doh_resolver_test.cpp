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

#include <absl/flags/flag.h>
#include <absl/time/clock.h>
#include <build/build_config.h>
#include <gtest/gtest-message.h>
#include <gtest/gtest.h>

#include <gmock/gmock.h>

#include "config/config_network.hpp"
#include "net/doh_resolver.hpp"
#include "test_util.hpp"

ABSL_FLAG(bool, no_doh_tests, false, "skip doh tests");
ABSL_FLAG(bool, use_china_dns_tests, false, "use China DNS in DNS tests");
ABSL_FLAG(int, use_timeout_dns_tests, 5000, "Timeout (in milliseconds) in DNS tests");
ABSL_FLAG(int, use_concurrent_dns_tests, 2, "Concurrent DNS tests in a row");
ABSL_FLAG(int, use_repeated_dns_tests, 2, "Repeated DNS tests in a row");

#define DOH_URL (absl::GetFlag(FLAGS_use_china_dns_tests) ? "https://223.5.5.5/dns-query" : \
                 "https://1.1.1.1/dns-query")

#define INVALID_DOH_URL (absl::GetFlag(FLAGS_use_china_dns_tests) ? "https://5.5.5.5/dns-query" : \
                 "https://2.2.2.2/dns-query")

using namespace net;

static void DoLocalResolve(asio::io_context& io_context, scoped_refptr<DoHResolver> resolver) {
  auto work_guard =
      std::make_shared<asio::executor_work_guard<asio::io_context::executor_type>>(io_context.get_executor());

  asio::post(io_context, [resolver, work_guard]() {
    resolver->AsyncResolve("doh-test.localhost", 80,
                           [work_guard](asio::error_code ec, asio::ip::tcp::resolver::results_type results) {
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
}

TEST(DNS_DOH_TEST, LocalBasic) {
  if (absl::GetFlag(FLAGS_no_doh_tests)) {
    GTEST_SKIP() << "skipped as required";
    return;
  }
  asio::error_code ec;
  asio::io_context io_context;

  auto resolver = DoHResolver::Create(io_context);
  auto ret = resolver->Init(DOH_URL, absl::GetFlag(FLAGS_use_timeout_dns_tests));
  ASSERT_EQ(ret, asio::error_code());

  io_context.restart();
  DoLocalResolve(io_context, resolver);
  EXPECT_NO_FATAL_FAILURE(io_context.run());
}

static void DoRemoteResolve(asio::io_context& io_context, scoped_refptr<DoHResolver> resolver) {
  auto work_guard =
      std::make_shared<asio::executor_work_guard<asio::io_context::executor_type>>(io_context.get_executor());

  asio::post(io_context, [resolver, work_guard]() {
    resolver->AsyncResolve("www.cloudflare.com", 80,
                           [work_guard](asio::error_code ec, asio::ip::tcp::resolver::results_type results) {
                             // Sometimes dns resolver don't get ack in time, ignore it safely
                             if (ec == asio::error::timed_out) {
                               return;
                             }
                             // Sometimes dns resolver don't get result without an answer like cloudflare
                             if (ec == asio::error::host_not_found) {
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
}

TEST(DNS_DOH_TEST, RemoteBasic) {
  if (absl::GetFlag(FLAGS_no_doh_tests)) {
    GTEST_SKIP() << "skipped as required";
    return;
  }
  asio::error_code ec;
  asio::io_context io_context;

  auto resolver = DoHResolver::Create(io_context);
  auto ret = resolver->Init(DOH_URL, absl::GetFlag(FLAGS_use_timeout_dns_tests));
  ASSERT_EQ(ret, asio::error_code());

  for (int i = 0; i < absl::GetFlag(FLAGS_use_repeated_dns_tests); ++i) {
    io_context.restart();
    DoRemoteResolve(io_context, resolver);
    EXPECT_NO_FATAL_FAILURE(io_context.run()) << "Failure at run " << i;
    absl::SleepFor(absl::Milliseconds(5));
  }
}

TEST(DNS_DOH_TEST, RemoteConcurrent) {
  if (absl::GetFlag(FLAGS_no_doh_tests)) {
    GTEST_SKIP() << "skipped as required";
    return;
  }
  asio::error_code ec;
  asio::io_context io_context;

  std::vector<scoped_refptr<DoHResolver>> resolvers;
  for (int i = 0; i < absl::GetFlag(FLAGS_use_concurrent_dns_tests); ++i) {
    auto resolver = DoHResolver::Create(io_context);
    auto ret = resolver->Init(DOH_URL, absl::GetFlag(FLAGS_use_timeout_dns_tests));
    ASSERT_EQ(ret, asio::error_code());
    resolvers.push_back(resolver);
  }

  for (int i = 0; i < absl::GetFlag(FLAGS_use_repeated_dns_tests); ++i) {
    io_context.restart();
    for (int j = 0; j < absl::GetFlag(FLAGS_use_concurrent_dns_tests); ++j) {
      DoRemoteResolve(io_context, resolvers[j]);
    }
    EXPECT_NO_FATAL_FAILURE(io_context.run()) << "Failure at run " << i;
    absl::SleepFor(absl::Milliseconds(10));
  }
}

TEST(DNS_DOH_TEST, Timeout) {
  if (absl::GetFlag(FLAGS_no_doh_tests)) {
    GTEST_SKIP() << "skipped as required";
    return;
  }
  asio::error_code ec;
  asio::io_context io_context;

  auto resolver = DoHResolver::Create(io_context);
  auto ret = resolver->Init(INVALID_DOH_URL, 1);
  ASSERT_EQ(ret, asio::error_code());

  auto work_guard =
      std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(io_context.get_executor());

  asio::post(io_context, [&]() {
    resolver->AsyncResolve("www.cloudflare.com", 80,
                           [&](asio::error_code ec, asio::ip::tcp::resolver::results_type results) {
                             work_guard.reset();
                             ASSERT_EQ(ec, asio::error::timed_out) << ec;
                           });
  });

  EXPECT_NO_FATAL_FAILURE(io_context.run());
}
