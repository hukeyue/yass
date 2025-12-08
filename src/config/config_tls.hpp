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

#ifndef H_CONFIG_CONFIG_TLS
#define H_CONFIG_CONFIG_TLS

#include <absl/flags/declare.h>
#include <string>

extern std::string g_certificate_chain_content;
extern std::string g_private_key_content;
ABSL_DECLARE_FLAG(std::string, certificate_chain_file);
ABSL_DECLARE_FLAG(std::string, private_key_file);
ABSL_DECLARE_FLAG(std::string, private_key_password);
ABSL_DECLARE_FLAG(bool, insecure_mode);
ABSL_DECLARE_FLAG(std::string, cacert);
ABSL_DECLARE_FLAG(std::string, capath);
ABSL_DECLARE_FLAG(bool, tls13_early_data);
ABSL_DECLARE_FLAG(bool, enable_post_quantum_kyber);

namespace config {
bool ReadTLSConfigFile();
}  // namespace config

#endif  // H_CONFIG_CONFIG_TLS
