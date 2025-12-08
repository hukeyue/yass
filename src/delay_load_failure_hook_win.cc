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

// windows.h needs to be included before delayimp.h.
#include <windows.h>

#define __pfnDliFailureHook2 __pfnDliFailureHook2Hidden
#include <delayimp.h>
#undef __pfnDliFailureHook2
#include <stdlib.h>

namespace yass {

extern void DisableDelayLoadFailureHooksForMainExecutable();

extern FARPROC WINAPI HandleDelayLoadFailureCommon(unsigned reason, DelayLoadInfo* dll_info);

FARPROC WINAPI HandleDelayLoadFailureCommon(unsigned reason,
                                            DelayLoadInfo* dll_info) {
  // ERROR_COMMITMENT_LIMIT means that there is no memory. Convert this into a
  // more suitable crash rather than just CHECKing in this function.
  if (dll_info->dwLastError == ERROR_COMMITMENT_LIMIT) {
    _exit(255);
  }

  // DEBUG_ALIAS_FOR_CSTR(dll_name, dll_info->szDll, 256);
  // SCOPED_CRASH_KEY_STRING256("DelayLoad", "ModuleName", dll_name);

  // Deterministically crash here. Returning 0 from the hook would likely result
  // in the process crashing anyway, but in a form that might trigger undefined
  // behavior or be hard to diagnose. See https://crbug.com/1320845.
  abort();

  return nullptr;
}

namespace {

bool g_hooks_enabled = true;

// Delay load failure hook that generates a crash report. By default a failure
// to delay load will trigger an exception handled by the delay load runtime and
// this won't generate a crash report.
FARPROC WINAPI DelayLoadFailureHookEXE(unsigned reason,
                                       DelayLoadInfo* dll_info) {
  if (!g_hooks_enabled)
    return nullptr;

  return HandleDelayLoadFailureCommon(reason, dll_info);
}

}  // namespace

void DisableDelayLoadFailureHooksForMainExecutable() {
  g_hooks_enabled = false;
}

}  // namespace yass

// Set the delay load failure hook to the function above.
//
// The |__pfnDliFailureHook2| failure notification hook gets called
// automatically by the delay load runtime in case of failure, see
// https://docs.microsoft.com/en-us/cpp/build/reference/failure-hooks?view=vs-2019
// for more information about this.
extern "C" const PfnDliHook __pfnDliFailureHook2 =
    yass::DelayLoadFailureHookEXE;
