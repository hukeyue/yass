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

/* Copyright (c) 2019-2025 Chilledheart  */

#ifndef H_CONFIG_CONFIG
#define H_CONFIG_CONFIG

#include <string>
#include <string_view>

#include "config/config_core.hpp"
#include "config/config_network.hpp"
#include "config/config_ptype.hpp"
#include "config/config_tls.hpp"

namespace config {

bool ReadConfig();
bool SaveConfig();

extern bool testOnlyMode;
void ReadConfigFileAndArguments(int argc, const char** argv);

std::string ValidateConfig();

std::string ReadConfigFromArgument(std::string_view server_host,
                                   std::string_view server_sni,
                                   std::string_view server_port,
                                   std::string_view username,
                                   std::string_view password,
                                   cipher_method method,
                                   std::string_view local_host,
                                   std::string_view local_port,
                                   std::string_view doh_url,
                                   std::string_view dot_host,
                                   std::string_view limit_rate,
                                   std::string_view connect_timeout);

std::string ReadConfigFromArgument(std::string_view server_host,
                                   std::string_view server_sni,
                                   std::string_view server_port,
                                   std::string_view username,
                                   std::string_view password,
                                   std::string_view method_string,
                                   std::string_view local_host,
                                   std::string_view local_port,
                                   std::string_view doh_url,
                                   std::string_view dot_host,
                                   std::string_view limit_rate,
                                   std::string_view connect_timeout);

void SetClientUsageMessage(std::string_view exec_path);
void SetServerUsageMessage(std::string_view exec_path);

}  // namespace config

#endif  // H_CONFIG_CONFIG
