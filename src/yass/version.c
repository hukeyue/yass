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

/* Copyright (c) 2019-2026 Chilledheart  */

#include "yass/version.h"

const char* yass_app_name(void) {
  return YASS_APP_NAME;
}

void yass_app_version(int* major, int* minor, int* patch, int* tweak) {
  if (major)
    *major = YASS_APP_VERSION_MAJOR;

  if (minor)
    *minor = YASS_APP_VERSION_MINOR;

  if (patch)
    *patch = YASS_APP_VERSION_PATCH;

  if (tweak)
    *tweak = YASS_APP_VERSION_TWEAK;
}

const char* yass_app_version_str(void) {
  return YASS_APP_VERSION;
}
