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

#ifndef H_CONFIG_CONFIG_PTYPE
#define H_CONFIG_CONFIG_PTYPE

namespace config {

enum ProgramType {
  YASS_UNSPEC,
  YASS_SERVER_MASK = 0x10,
  YASS_SERVER_DEFAULT = 0x10,
  YASS_UNITTEST_MASK = 0x20,
  YASS_UNITTEST_DEFAULT = 0x20,
  YASS_BENCHMARK_MASK = 0x40,
  YASS_BENCHMARK_DEFAULT = 0x40,
  YASS_CLIENT_MASK = 0x80,
  YASS_CLIENT_DEFAULT = 0x80,
  YASS_CLIENT_GUI = 0x81,
};

extern const ProgramType pType;
const char* ProgramTypeToStr(ProgramType type);

inline bool pType_IsClient() {
  return pType & YASS_CLIENT_MASK;
}

inline bool pType_IsServer() {
  return pType & YASS_SERVER_MASK;
}

}  // namespace config

#endif  // H_CONFIG_CONFIG_PTYPE
