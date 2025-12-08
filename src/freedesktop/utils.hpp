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

#ifndef YASS_FREEDESKTOP_UTILS
#define YASS_FREEDESKTOP_UTILS

#include <cstdint>
#include <memory>
#include <string>

class Utils {
 public:
  static bool GetAutoStart();
  static void EnableAutoStart(bool on);
  static bool GetSystemProxy();
  static bool SetSystemProxy(bool on);
  static std::string GetLocalAddr();
  static std::string GetLocalAddrKDE();
};

bool QuerySystemProxy(bool* enabled, std::string* server_host, std::string* server_port, std::string* bypass_addr);

bool SetSystemProxy(bool enable,
                    const std::string& server_host,
                    const std::string& server_port,
                    const std::string& bypass_addr);

bool QuerySystemProxy_KDE(bool* enabled, std::string* server_addr, std::string* bypass_addr);

bool SetSystemProxy_KDE(bool enable, const std::string& server_addr, const std::string& bypass_addr);

#endif  // YASS_FREEDESKTOP_UTILS
