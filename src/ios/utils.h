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

#ifndef YASS_IOS_UTILS
#define YASS_IOS_UTILS

#include <string>
#include <string_view>
#include <vector>

void initNetworkPathMonitor();
void deinitNetworkPathMonitor();

bool connectedToNetwork();

std::string serializeTelemetryJson(uint64_t total_rx_bytes, uint64_t total_tx_bytes);
bool parseTelemetryJson(std::string_view resp, uint64_t* total_rx_bytes, uint64_t* total_tx_bytes);

constexpr const char kAppMessageGetTelemetry[] = "__get_telemetry";

constexpr const char kServerHostFieldName[] = "server_host";
constexpr const char kServerSNIFieldName[] = "server_sni";
constexpr const char kServerPortFieldName[] = "server_port";
constexpr const char kUsernameFieldName[] = "username";
constexpr const char kPasswordFieldName[] = "password";
constexpr const char kMethodStringFieldName[] = "method_string";
constexpr const char kDoHURLFieldName[] = "doh_url";
constexpr const char kDoTHostFieldName[] = "dot_host";
constexpr const char kLimitRateFieldName[] = "limit_rate";
constexpr const char kConnectTimeoutFieldName[] = "connect_timeout";

constexpr const char kEnablePostQuantumKyberKey[] = "ENABLE_POST_QUANTUM_KYBER";

#endif  //  YASS_IOS_UTILS
