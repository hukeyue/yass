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

#ifndef H_NET_PROTOCOL
#define H_NET_PROTOCOL

#ifdef _WIN32
#include <malloc.h>
#endif

#include <functional>
#include <string_view>
#include <utility>

#include <build/build_config.h>
#include "core/logging.hpp"
#include "net/asio.hpp"
#include "net/io_buffer.hpp"

#define SOCKET_BUF_SIZE (16384)
#define SOCKET_DEBUF_SIZE (16384)
#define SS_FRAME_SIZE (16384 - 128)

namespace net {

// This enum is used in Net.SSLNegotiatedAlpnProtocol histogram.
// Do not change or re-use values.
enum NextProto { kProtoUnknown = 0, kProtoHTTP11 = 1, kProtoHTTP2 = 2, kProtoQUIC = 3, kProtoLast = kProtoQUIC };

// List of protocols to use for ALPN, used for configuring HttpNetworkSessions.
typedef std::vector<NextProto> NextProtoVector;

NextProto NextProtoFromString(std::string_view proto_string);

std::string_view NextProtoToString(NextProto next_proto);

#if DCHECK_IS_ON()
inline void DumpHex_Impl(const char* file, int line, const char* prefix, const uint8_t* data, uint32_t length) {
  if (!VLOG_IS_ON(4)) {
    return;
  }
  char hex_buffer[4096];
  char* message = hex_buffer;
  int left_size = sizeof(hex_buffer) - 1;

  int written = snprintf(message, left_size, "%s LEN %u\n", prefix, length);
  if (written < 0 || written > left_size)
    goto done;
  message += written;
  left_size -= written;

  length = std::min<uint32_t>(length, sizeof(hex_buffer) / 4);
  for (uint32_t i = 0; i * 2 + 1 < length; ++i) {
    if (i % 8 == 0) {
      written = snprintf(message, left_size, "%s ", prefix);
      if (written < 0 || written > left_size)
        goto done;
      message += written;
      left_size -= written;
    }

    written = snprintf(message, left_size, "%02x%02x ", data[i * 2], data[i * 2 + 1]);
    if (written < 0 || written > left_size)
      goto done;
    message += written;
    left_size -= written;

    if ((i + 1) % 8 == 0) {
      written = snprintf(message, left_size, "\n");
      if (written < 0 || written > left_size)
        goto done;
      message += written;
      left_size -= written;
    }
  }
  written = snprintf(message, left_size, "\n");
  if (written < 0 || written > left_size)
    goto done;
  message += written;
  left_size -= written;

done:
  // ensure it is null-terminated
  hex_buffer[sizeof(hex_buffer) - 1] = '\0';
  ::gurl_base::logging::LogMessage(file, line, -4).stream() << hex_buffer;
}

inline void DumpHex_Impl(const char* file, int line, const char* prefix, const net::IOBuffer* buf) {
  const uint8_t* data = buf->bytes();
  uint32_t length = buf->size();
  DumpHex_Impl(file, line, prefix, data, length);
}
#endif  // DCHECK_IS_ON()

}  // namespace net

#if DCHECK_IS_ON()
#define DumpHex(...) ::net::DumpHex_Impl(__FILE__, __LINE__, __VA_ARGS__)
#else
#define DumpHex(...)
#endif

#endif  // H_NET_PROTOCOL
