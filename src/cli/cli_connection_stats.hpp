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

/* Copyright (c) 2021-2025 Chilledheart  */

#ifndef H_CLI_CONNECTION_STATS
#define H_CLI_CONNECTION_STATS

#include <atomic>
#include <cstdint>

namespace net::cli {

/// statistics of total received bytes (non-encoded)
extern std::atomic<uint64_t> total_rx_bytes;
/// statistics of total sent bytes (non-encoded)
extern std::atomic<uint64_t> total_tx_bytes;
/// statistics of total received times (non-encoded)
extern std::atomic<uint64_t> total_rx_times;
/// statistics of total sent times (non-encoded)
extern std::atomic<uint64_t> total_tx_times;
/// statistics of total yield times (rx) (non-encoded)
extern std::atomic<uint64_t> total_rx_yields;
/// statistics of total yield times (tx) (non-encoded)
extern std::atomic<uint64_t> total_tx_yields;

}  // namespace net::cli

void PrintCliStats();

#endif  // H_CLI_CONNECTION_STATS
