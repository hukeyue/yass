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

/* Copyright (c) 2026 Chilledheart  */

#include "config/config_cli.hpp"

#include <absl/flags/flag.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include <stdint.h>
#include <optional>
#include <sstream>

#include "config/config_core.hpp"
#include "core/utils.hpp"

static constexpr const std::string_view kProxyUsage = R"(
PROXY-URI = <PROXY-PROTO>"://"[<USER>":"<PASS>"@"]<HOSTNAME>[":"<PORT>]
PROXY-PROTO = "https" | "http2" | "socks" | "naive"

Routes traffic via the proxy URI. (CLI only)
The naive proxy scheme is negotiated automatically for Naive padding (controlled by --padding_support)

If multiple proxies are specified, they must match the number of specified
LISTEN-URIs, and each LISTEN-URI is routed to the PROXY matched by position.
PROXY-CHAIN is not supported.

Once specified, all of server_host, server_sni, server_port, username, password, method, padding_support, redir_mode, local_host and local_port are ignored.
)";
ABSL_FLAG(StringArrayFlag, proxy, {}, kProxyUsage);

static constexpr const std::string_view kListenUsage = R"(
LISTEN-URI = <LISTEN-PROTO>"://"[<USER>":"<PASS>"@"][<ADDR>][":"<PORT>]
LISTEN-PROTO = "auto" | "socks" | "http" | "redir"

Listens at addr:port with protocol <LISTEN-PROTO>. (CLI only)
Can be specified multiple times to listen on multiple ports.
LISTEN-PROTO is required but ignored in use.
Default proto, addr, port: auto, 0.0.0.0, 1080.

Once specified, all of server_host, server_sni, server_port, username, password, method, padding_support, redir_mode, local_host and local_port are ignored.
)";

ABSL_FLAG(StringArrayFlag, listen, {}, kListenUsage);

