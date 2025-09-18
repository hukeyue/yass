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

#include "config/config_ptype.hpp"

#include "gui_variant.h"

namespace config {

const char* ProgramTypeToStr(ProgramType type) {
  switch (type) {
    case YASS_SERVER_DEFAULT:
      return "server";
    case YASS_UNITTEST_DEFAULT:
      return "unittest";
    case YASS_BENCHMARK_DEFAULT:
      return "benchmark";
    case YASS_CLIENT_DEFAULT:
      return "client";
    case YASS_CLIENT_GUI:
      return "gui client (" YASS_GUI_FLAVOUR ")";
    case YASS_UNSPEC:
    default:
      return "unspec";
  }
}

}  // namespace config
