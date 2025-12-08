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

/* Copyright (c) 2019-2025 Chilledheart  */

#ifndef H_YASS_CORE_LOGGING
#define H_YASS_CORE_LOGGING

#include "third_party/googleurl/polyfills/base/check.h"
#include "third_party/googleurl/polyfills/base/check_op.h"
#include "third_party/googleurl/polyfills/base/logging.h"

#include <iosfwd>

// override operator<< std::error_code from STL
inline std::ostream& operator<<(std::ostream& os, const std::error_code& ec) {
#ifdef _WIN32
  return os << ec.message() << " value: " << ec.value();
#else
  return os << ec.message();
#endif
}

#endif  //  H_YASS_CORE_LOGGING
