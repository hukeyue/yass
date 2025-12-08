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

#include "cli/cli_connection_stats.hpp"

#include <sstream>

#include "core/logging.hpp"
#include "core/utils.hpp"

namespace net::cli {

std::atomic<uint64_t> total_rx_bytes;
std::atomic<uint64_t> total_tx_bytes;

std::atomic<uint64_t> total_rx_times;
std::atomic<uint64_t> total_tx_times;

std::atomic<uint64_t> total_rx_yields;
std::atomic<uint64_t> total_tx_yields;

}  // namespace net::cli
static std::string HumanReadableByteCountBinStr(uint64_t bytes) {
  std::stringstream ss;

  HumanReadableByteCountBin(&ss, bytes);
  return ss.str();
}

void PrintCliStats() {
  LOG(ERROR) << "Cli Connection Stats: Sent: " << HumanReadableByteCountBinStr(net::cli::total_rx_bytes);
  LOG(ERROR) << "Cli Connection Stats: Received: " << HumanReadableByteCountBinStr(net::cli::total_tx_bytes);
  LOG(ERROR) << "Cli Connection Stats: Sent Times: " << net::cli::total_rx_times;
  LOG(ERROR) << "Cli Connection Stats: Received Times: " << net::cli::total_tx_times;
  LOG(ERROR) << "Cli Connection Stats: Sent Average: "
             << HumanReadableByteCountBinStr(net::cli::total_rx_bytes / (net::cli::total_rx_times + 1));
  LOG(ERROR) << "Cli Connection Stats: Received Average: "
             << HumanReadableByteCountBinStr(net::cli::total_tx_bytes / (net::cli::total_tx_times + 1));
  LOG(ERROR) << "Cli Connection Stats: Sent Yield Times: " << net::cli::total_rx_yields;
  LOG(ERROR) << "Cli Connection Stats: Received Yield Times: " << net::cli::total_tx_yields;
}
