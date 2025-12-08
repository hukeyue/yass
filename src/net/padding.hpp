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

#ifndef H_NET_PADDING
#define H_NET_PADDING

#include "net/asio.hpp"

#include <absl/flags/declare.h>

ABSL_DECLARE_FLAG(bool, padding_support);

namespace net {

constexpr const int kFirstPaddings = 8;
constexpr const int kPaddingHeaderSize = 3;
constexpr const int kMaxPaddingSize = 255;

scoped_refptr<GrowableIOBuffer> AddPadding(IOBuffer* buf);
scoped_refptr<GrowableIOBuffer> RemovePadding(GrowableIOBuffer* buf, asio::error_code& ec);

}  // namespace net

#endif  // H_NET_PADDING
