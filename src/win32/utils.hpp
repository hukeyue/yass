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

/* Copyright (c) 2022-2025 Chilledheart  */

#ifndef YASS_WIN32_UTILS
#define YASS_WIN32_UTILS
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include <windows.h>

class Utils {
 public:
  enum DpiAwarenessType {
    DpiAwarenessUnware,
    DpiAwarenessSystem,
    DpiAwarenessPerMonitor,
    DpiAwarenessPerMonitorV2,
  };
  static bool SetDpiAwareness(DpiAwarenessType awareness_type = DpiAwarenessPerMonitorV2);
  static bool SetMixedThreadDpiHostingBehavior();
  // Determine the DPI to use, according to the DPI awareness mode
  static unsigned int GetDpiForWindowOrSystem(HWND hWnd);
  static bool EnableNonClientDpiScalingInt(HWND hWnd);
  static bool SystemParametersInfoForDpiInt(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi);
  static bool GetUserDefaultLocaleName(std::wstring* localeName);

  static bool GetAutoStart();
  static void EnableAutoStart(bool on);
  static bool GetSystemProxy();
  static bool SetSystemProxy(bool on);
  // http://127.0.0.1:1081
  static std::string GetLocalAddr();
};

std::wstring LoadStringStdW(HINSTANCE hInstance, UINT uID);

#define DEFAULT_AUTOSTART_NAME L"YASS"

// server_addr and bypass_addr should be something like
// server_addr http://127.0.0.1:1081 bypass_addr <local>
// server_addr http=127.0.0.1:1081;https=127.0.0.1:1081;ftp=127.0.0.1:1081;socks=127.0.0.1:1081 bypass_addr <local>
bool QuerySystemProxy(bool* enabled, std::string* server_addr, std::string* bypass_addr);

bool SetSystemProxy(bool enable, const std::string& server_addr, const std::string& bypass_addr);

bool SetSystemProxy(bool enable,
                    const std::string& server_addr,
                    const std::string& bypass_addr,
                    const std::wstring& conn_name);

bool GetAllRasConnection(std::vector<std::wstring>* result);

void WaitNetworkUp(std::function<void()> callback);

#endif  // YASS_WIN32_UTILS
