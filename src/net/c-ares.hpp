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

/* Copyright (c) 2023-2025 Chilledheart  */

#ifndef H_NET_C_ARES_HPP
#define H_NET_C_ARES_HPP

#ifdef HAVE_C_ARES

#include <ares.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include <base/memory/ref_counted.h>
#include <base/memory/scoped_refptr.h>

#include "net/asio.hpp"

namespace net {

#ifdef _WIN32
using fd_t = SOCKET;
#else
using fd_t = int;
#endif

class CAresResolver : public gurl_base::RefCountedThreadSafe<CAresResolver> {
 public:
  CAresResolver(asio::io_context& io_context);
  static scoped_refptr<CAresResolver> Create(asio::io_context& io_context) {
    return gurl_base::MakeRefCounted<CAresResolver>(io_context);
  }
  ~CAresResolver();

  int Init(int timeout_ms);

  void Cancel();
  void Destroy();

  using AsyncResolveCallback = std::function<void(asio::error_code ec, asio::ip::tcp::resolver::results_type)>;
  void AsyncResolve(const std::string& host, const std::string& service, AsyncResolveCallback cb);

 private:
  static void OnAsyncResolveCtx(void* arg, int status, int timeouts, struct ares_addrinfo* result);
  void OnAsyncResolve(AsyncResolveCallback cb,
                      const std::string& host,
                      const std::string& service,
                      int status,
                      int timeouts,
                      struct ares_addrinfo* result);

 private:
  struct ResolverPerContext : public RefCountedThreadSafe<ResolverPerContext> {
    static scoped_refptr<ResolverPerContext> Create(asio::io_context& io_context, fd_t fd) {
      return gurl_base::MakeRefCounted<ResolverPerContext>(io_context, fd);
    }
    ResolverPerContext(asio::io_context& io_context, fd_t fd) : socket(io_context, asio::ip::udp::v4(), fd) {}
    ~ResolverPerContext() {
      asio::error_code ec;
      static_cast<void>(socket.close(ec));
    }

    asio::ip::udp::socket socket;
    bool read_enable = false;
    bool write_enable = false;
  };

  static void OnSockStateCtx(void* arg, fd_t fd, int readable, int writable);
  void OnSockState(fd_t fd, int readable, int writable);

  void WaitRead(scoped_refptr<ResolverPerContext> ctx, fd_t fd);
  void WaitWrite(scoped_refptr<ResolverPerContext> ctx, fd_t fd);
  void WaitTimer();

 private:
  asio::io_context& io_context_;

  bool init_ = false;
  ares_channel channel_;
  ares_options ares_opts_;
  std::unordered_map<fd_t, scoped_refptr<ResolverPerContext>> fd_map_;

  char lookups_[3] = "fb";
  int timeout_ms_ = 0;
  asio::steady_timer resolve_timer_;

  bool done_ = true;
  bool expired_;
};

}  // namespace net

#endif  // HAVE_C_ARES

#endif  // H_NET_C_ARES_HPP
