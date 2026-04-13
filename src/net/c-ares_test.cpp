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

/* Copyright (c) 2023-2026 Chilledheart  */

#include "yass/feature.h"

#ifdef HAVE_C_ARES

#include <absl/flags/flag.h>
#include <absl/time/clock.h>
#include <build/build_config.h>
#include <gtest/gtest-message.h>
#include <gtest/gtest.h>

#include <gmock/gmock.h>

#include "config/config_network.hpp"
#include "net/c-ares.hpp"
#include "net/resolver.hpp"
#include "test_util.hpp"

ABSL_FLAG(bool, no_cares_tests,
#if BUILDFLAG(IS_IOS) || BUILDFLAG(IS_ANDROID)
          true,
#else
          false,
#endif
          "skip c-ares tests");

ABSL_DECLARE_FLAG(int, use_timeout_dns_tests);
ABSL_DECLARE_FLAG(int, use_concurrent_dns_tests);
ABSL_DECLARE_FLAG(int, use_repeated_dns_tests);

using namespace net;

TEST(DNS_CARES_TEST, LocalfileBasic) {
  asio::error_code ec;
  asio::io_context io_context;
  auto resolver = CAresResolver::Create(io_context);
  auto ret = resolver->Init(10);
  ASSERT_EQ(ret, asio::error_code());
  auto work_guard =
      std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(io_context.get_executor());

  asio::post(io_context, [&]() {
    resolver->AsyncResolve("localhost", "80", [&](asio::error_code ec, asio::ip::tcp::resolver::results_type results) {
      work_guard.reset();
      ASSERT_FALSE(ec) << ec;
      bool has_ipv6 = false;
      for (auto iter = std::begin(results); iter != std::end(results); ++iter) {
        asio::ip::tcp::endpoint endpoint = *iter;
        auto addr = endpoint.address();
        EXPECT_TRUE(addr.is_loopback()) << addr;
        has_ipv6 |= addr.is_v6();
      }
      if (absl::GetFlag(FLAGS_ipv6_mode)) {
        EXPECT_TRUE(has_ipv6) << "Expected IPv6 addresses on IPv6 mode";
      }
    });
  });

  io_context.run();
}

TEST(DNS_CARES_TEST, RemoteNotFound) {
  if (absl::GetFlag(FLAGS_disable_cares) || absl::GetFlag(FLAGS_no_cares_tests)) {
    GTEST_SKIP() << "skipped as required";
    return;
  }
  asio::error_code ec;
  asio::io_context io_context;

  auto resolver = CAresResolver::Create(io_context);
  auto ret = resolver->Init(10);
  ASSERT_EQ(ret, asio::error_code());

  auto work_guard =
      std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(io_context.get_executor());

  asio::post(io_context, [&]() {
    resolver->AsyncResolve("not-found.invalid", "80",
                           [&](asio::error_code ec, asio::ip::tcp::resolver::results_type results) {
                             work_guard.reset();
                             ASSERT_TRUE(ec) << ec;
                           });
  });

  io_context.run();
}

static void DoRemoteResolve(asio::io_context& io_context, scoped_refptr<CAresResolver> resolver) {
  auto work_guard =
      std::make_shared<asio::executor_work_guard<asio::io_context::executor_type>>(io_context.get_executor());

  asio::post(io_context, [resolver, work_guard]() {
    resolver->AsyncResolve("www.cloudflare.com", "80",
                           [work_guard](asio::error_code ec, asio::ip::tcp::resolver::results_type results) {
                             // Sometimes c-ares don't get ack in time, ignore it safely
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
}

TEST(DNS_CARES_TEST, RemoteBasic) {
  if (absl::GetFlag(FLAGS_disable_cares) || absl::GetFlag(FLAGS_no_cares_tests)) {
    GTEST_SKIP() << "skipped as required";
    return;
  }
  asio::error_code ec;
  asio::io_context io_context;

  auto resolver = CAresResolver::Create(io_context);
  auto ret = resolver->Init(absl::GetFlag(FLAGS_use_timeout_dns_tests));
  ASSERT_EQ(ret, asio::error_code());

  for (int i = 0; i < absl::GetFlag(FLAGS_use_repeated_dns_tests); ++i) {
    io_context.restart();
    DoRemoteResolve(io_context, resolver);
    EXPECT_NO_FATAL_FAILURE(io_context.run()) << "Failure at run " << i;
    absl::SleepFor(absl::Milliseconds(5));
  }
}

TEST(DNS_CARES_TEST, RemoteConcurrent) {
  if (absl::GetFlag(FLAGS_disable_cares) || absl::GetFlag(FLAGS_no_cares_tests)) {
    GTEST_SKIP() << "skipped as required";
    return;
  }
  asio::error_code ec;
  asio::io_context io_context;

  std::vector<scoped_refptr<CAresResolver>> resolvers;
  for (int i = 0; i < absl::GetFlag(FLAGS_use_concurrent_dns_tests); ++i) {
    auto resolver = CAresResolver::Create(io_context);
    auto ret = resolver->Init(absl::GetFlag(FLAGS_use_timeout_dns_tests));
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

#endif  // HAVE_C_ARES
