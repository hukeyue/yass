// SPDX-License-Identifier: GPL-2.0
/* Copyright (c) 2024 Chilledheart  */
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
