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

#ifndef H_CONFIG_CONFIG_CORE
#define H_CONFIG_CONFIG_CORE

#include <absl/flags/declare.h>
#include <absl/strings/string_view.h>
#include <base/compiler_specific.h>
#include <stdint.h>
#include <string>
#include <string_view>

#include "config/config_export.hpp"

ABSL_DECLARE_FLAG(std::string, server_host);
ABSL_DECLARE_FLAG(std::string, server_sni);
ABSL_DECLARE_FLAG(PortFlag, server_port);
ABSL_DECLARE_FLAG(std::string, username);
ABSL_DECLARE_FLAG(std::string, password);
ABSL_DECLARE_FLAG(CipherMethodFlag, method);
ABSL_DECLARE_FLAG(std::string, local_host);
ABSL_DECLARE_FLAG(PortFlag, local_port);

ABSL_DECLARE_FLAG(uint32_t, parallel_max);
ABSL_DECLARE_FLAG(RateFlag, limit_rate);  // bytes per second

bool AbslParseFlag(absl::string_view text, PortFlag* flag, std::string* err);

std::string AbslUnparseFlag(const PortFlag&);

bool AbslParseFlag(absl::string_view text, CipherMethodFlag* flag, std::string* err);

std::string AbslUnparseFlag(const CipherMethodFlag&);

bool AbslParseFlag(absl::string_view text, RateFlag* flag, std::string* err);

std::string AbslUnparseFlag(const RateFlag&);

#if BUILDFLAG(IS_MAC)
ABSL_DECLARE_FLAG(bool, ui_display_realtime_status);
#endif

#endif  // H_CONFIG_CONFIG_CORE
