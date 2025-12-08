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

#ifndef YASS_WIN32_APP
#define YASS_WIN32_APP

#include "cli/cli_worker.hpp"

#include <windows.h>

class CYassFrame;
/// The main Application for YetAnotherShadowSocket
class CYassApp {
 public:
  CYassApp(HINSTANCE hInstance);
  ~CYassApp();

 private:
  const HINSTANCE m_hInstance;
  BOOL InitInstance();
  int ExitInstance();

 public:
  int RunMainLoop();

  BOOL HandleThreadMessage(UINT message, WPARAM w, LPARAM l);

  void OnStart(bool quiet = false);
  void OnStop(bool quiet = false);

  std::wstring GetStatus() const;
  enum YASSState { STARTED, STARTING, START_FAILED, STOPPING, STOPPED };
  YASSState GetState() const { return state_; }

 private:
  void OnStarted(WPARAM w, LPARAM l);
  void OnStartFailed(WPARAM w, LPARAM l);
  void OnStopped(WPARAM w, LPARAM l);

  BOOL OnIdle();

 private:
  BOOL CheckFirstInstance();
  std::string SaveConfig();

 private:
  YASSState state_;

  friend class CYassFrame;
  CYassFrame* frame_;

  Worker worker_;
  std::string error_msg_;
};

extern CYassApp* mApp;

#define WM_MYAPP_STARTED (WM_USER + 100)
#define WM_MYAPP_START_FAILED (WM_USER + 101)
#define WM_MYAPP_STOPPED (WM_USER + 102)
#define WM_MYAPP_NETWORK_UP (WM_USER + 103)

#endif  // YASS_WIN32_APP
