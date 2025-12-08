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

/* Copyright (c) 2023-2025 Chilledheart  */

#ifndef _WIN32

#include <absl/flags/flag.h>
#include <build/build_config.h>
#include <gtest/gtest-message.h>
#include <gtest/gtest.h>

#include <gmock/gmock.h>

#include "core/logging.hpp"
#include "core/process_utils.hpp"
#include "core/utils.hpp"
#include "test_util.hpp"

#if BUILDFLAG(IS_IOS) || BUILDFLAG(IS_ANDROID)
ABSL_FLAG(bool, no_exec_proc_tests, true, "skip execute_process tests");
#else
ABSL_FLAG(bool, no_exec_proc_tests, false, "skip execute_process tests");
#endif

using namespace std::string_literals;

TEST(PROCESS_TEST, ExecuteProcessBasic) {
  if (absl::GetFlag(FLAGS_no_exec_proc_tests)) {
    GTEST_SKIP() << "skipped as required";
    return;
  }
  std::string main_exe;
  GetExecutablePath(&main_exe);
  std::vector<std::string> params = {main_exe, "--version"s};
  std::string output, error;
  int ret = ExecuteProcess(params, &output, &error);
  EXPECT_EQ(ret, 0);
  EXPECT_THAT(output, ::testing::StartsWith("yass_test\n"));
  EXPECT_EQ(error, "");
}

#endif  // _WIN32
