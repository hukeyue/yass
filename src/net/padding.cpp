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

#include "net/padding.hpp"

#include <base/rand_util.h>

/// <payload_length> <padding length> <payload> <padding>
/// input:
///                                       *
/// output:
/// p[0] << 8 + p[1]       p[2]           *         *
ABSL_FLAG(bool, padding_support, true, "Enable padding support");

static char kEmptyPadding[net::kMaxPaddingSize] = {};

namespace net {

scoped_refptr<GrowableIOBuffer> AddPadding(IOBuffer* buf) {
  auto ret = gurl_base::MakeRefCounted<GrowableIOBuffer>();
  DCHECK_LE(buf->size(), 0xffff);
  uint16_t payload_size = buf->size();
  size_t padding_size = gurl_base::RandInt(0, kMaxPaddingSize);

  ret->SetCapacity(kPaddingHeaderSize + payload_size + padding_size);

  uint8_t p[kPaddingHeaderSize];
  p[0] = payload_size >> 8;
  p[1] = payload_size & 0xff;
  p[2] = padding_size;
  memcpy(ret->data(), p, sizeof(p));
  memcpy(ret->data() + kPaddingHeaderSize, buf->data(), payload_size);
  memset(ret->data() + kPaddingHeaderSize + payload_size, 0, padding_size);

  DCHECK_EQ(ret->size(), int(kPaddingHeaderSize + payload_size + padding_size));

  return ret;
}

/// <payload_length> <padding length> <payload> <padding>
/// input:
/// p[0] << 8 + p[1]       p[2]           *         *
/// output:
///                                       *
scoped_refptr<GrowableIOBuffer> RemovePadding(GrowableIOBuffer* buf, asio::error_code& ec) {
  if (buf->size() < kPaddingHeaderSize) {
    ec = asio::error::try_again;
    return nullptr;
  }
  const uint8_t* p = buf->bytes();
  int payload_size = (p[0] << 8) + p[1];
  if (payload_size == 0) {
    ec = asio::error::invalid_argument;
    return nullptr;
  }
  int padding_size = p[2];
  if (buf->size() < kPaddingHeaderSize + payload_size + padding_size) {
    ec = asio::error::try_again;
    return nullptr;
  }
  ec = asio::error_code();
  buf->set_offset(buf->offset() + kPaddingHeaderSize);
  auto ret = gurl_base::MakeRefCounted<GrowableIOBuffer>();
  ret->SetCapacity(payload_size);
  memcpy(ret->data(), buf->data(), payload_size);
  buf->set_offset(buf->offset() + payload_size + padding_size);
  return ret;
}

}  // namespace net
