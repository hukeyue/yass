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

#ifndef H_NET_ASIO
#define H_NET_ASIO

#ifdef _WIN32
#include <malloc.h>  // for _alloca
#endif               // _WIN32

#include <exception>
#include <thread>

#include "net/io_buffer.hpp"

#ifndef ASIO_NO_SSL
#include "third_party/boringssl/src/include/openssl/ssl.h"
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#pragma push
// #pragma warning(pop): likely mismatch, popping warning state pushed in
// different file
#pragma warning(disable : 5031)
// pointer to potentially throwing function passed to extern C function
#pragma warning(disable : 5039)
#endif  // defined(_MSC_VER) && !defined(__clang__)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

#if 0
#define ASIO_ENABLE_HANDLER_TRACKING
#endif

#ifdef __MINGW32__
#undef _POSIX_THREADS
#endif
#include <asio.hpp>
#include "net/asio_throw_exceptions.hpp"

#pragma GCC diagnostic pop

#if defined(_MSC_VER) && !defined(__clang__)
#pragma pop
#endif  // defined(_MSC_VER) && !defined(__clang__)

/// Create a new modifiable buffer that represents the given memory range.
/**
 * @returns <tt>mutable_buffer(tail, tailroom)</tt>.
 */
inline asio::ASIO_MUTABLE_BUFFER tail_buffer(net::GrowableIOBuffer* io_buf,
                                             uint32_t max_length = UINT32_MAX) ASIO_NOEXCEPT {
  return asio::ASIO_MUTABLE_BUFFER(io_buf->data(), std::min<uint32_t>(io_buf->size(), max_length));
}

/// Create a new modifiable buffer that represents the given memory range.
/**
 * @returns <tt>mutable_buffer(data, capacity)</tt>.
 */
inline asio::ASIO_MUTABLE_BUFFER mutable_buffer(net::GrowableIOBuffer* io_buf) ASIO_NOEXCEPT {
  return asio::ASIO_MUTABLE_BUFFER(io_buf->StartOfBuffer(), io_buf->capacity());
}

/// Create a new non-modifiable buffer that represents the given memory range.
/**
 * @returns <tt>const_buffer(data, length)</tt>.
 */
inline asio::ASIO_CONST_BUFFER const_buffer(net::GrowableIOBuffer* io_buf) ASIO_NOEXCEPT {
  return asio::ASIO_CONST_BUFFER(io_buf->data(), io_buf->size());
}

#ifndef ASIO_NO_SSL
void print_openssl_error();
void load_ca_to_ssl_ctx(SSL_CTX* ssl_ctx);
#endif

#endif  // H_NET_ASIO
