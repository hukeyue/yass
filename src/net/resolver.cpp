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

/* Copyright (c) 2024-2026 Chilledheart  */

#include "net/resolver.hpp"

#include <absl/flags/flag.h>

#include "config/config_network.hpp"
#include "core/utils.hpp"
#include "net/doh_resolver.hpp"
#include "net/dot_resolver.hpp"
#include "yass/feature.h"

#ifdef HAVE_C_ARES
#include "net/c-ares.hpp"
#endif

#ifdef HAVE_C_ARES
ABSL_FLAG(bool, disable_cares, false, "Disable C-Ares Component for DNS Resolving (If Available)");
#else
ABSL_FLAG(bool, disable_cares, true, "Disable C-Ares Component for DNS Resolving (Not Available)");
#endif

namespace net {

class Resolver::ResolverImpl {
 public:
  explicit ResolverImpl(asio::io_context& io_context)
      : io_context_(io_context),
        doh_resolver_(nullptr),
        dot_resolver_(nullptr),
#ifdef HAVE_C_ARES
        resolver_(nullptr),
#endif
        resolver_libc_(io_context)
  {
  }

  ~ResolverImpl() { Reset(); }

  asio::error_code Init() {
    doh_url_ = absl::GetFlag(FLAGS_doh_url);
    if (!doh_url_.empty()) {
      doh_resolver_ = DoHResolver::Create(io_context_);
      return doh_resolver_->Init(doh_url_, 10000);
    }
    dot_host_ = absl::GetFlag(FLAGS_dot_host);
    if (!dot_host_.empty()) {
      dot_resolver_ = DoTResolver::Create(io_context_);
      return dot_resolver_->Init(dot_host_, 10000);
    }
#ifdef HAVE_C_ARES
    if (!absl::GetFlag(FLAGS_disable_cares)) {
      resolver_ = CAresResolver::Create(io_context_);
      return resolver_->Init(5000);
    }
#endif
    return {};
  }

  void Cancel() {
    if (!doh_url_.empty()) {
      if (doh_resolver_) {
        doh_resolver_->Cancel();
      }
      return;
    }
    if (!dot_host_.empty()) {
      if (dot_resolver_) {
        dot_resolver_->Cancel();
      }
      return;
    }
#ifdef HAVE_C_ARES
    if (resolver_) {
      resolver_->Cancel();
      return;
    }
#endif
    resolver_libc_.cancel();
  }

  void Reset() {
    if (!doh_url_.empty()) {
      doh_resolver_.reset();
      return;
    }
    if (!dot_host_.empty()) {
      dot_resolver_.reset();
      return;
    }
#ifdef HAVE_C_ARES
    if (resolver_) {
      resolver_.reset();
      return;
    }
#endif
  }

  void AsyncResolve(const std::string& host_name, int port, AsyncResolveCallback cb) {
    if (!doh_url_.empty()) {
      doh_resolver_->AsyncResolve(host_name, port, cb);
      return;
    }
    if (!dot_host_.empty()) {
      dot_resolver_->AsyncResolve(host_name, port, cb);
      return;
    }
#ifdef HAVE_C_ARES
    if (resolver_) {
      resolver_->AsyncResolve(host_name, std::to_string(port), cb);
      return;
    }
#endif
    resolver_libc_.async_resolve(Net_ipv6works() ? asio::ip::tcp::unspec() : asio::ip::tcp::v4(),
                                 host_name, std::to_string(port), cb);
  }

 private:
  asio::io_context& io_context_;
  std::string doh_url_;
  std::string dot_host_;

  scoped_refptr<DoHResolver> doh_resolver_;
  scoped_refptr<DoTResolver> dot_resolver_;

#ifdef HAVE_C_ARES
  scoped_refptr<CAresResolver> resolver_;
#endif
  asio::ip::tcp::resolver resolver_libc_;
};

Resolver::Resolver(asio::io_context& io_context) : impl_(new ResolverImpl(io_context)) {}

Resolver::~Resolver() {
  delete impl_;
}

asio::error_code Resolver::Init() {
  return impl_->Init();
}

void Resolver::Cancel() {
  impl_->Cancel();
}

void Resolver::Reset() {
  impl_->Reset();
}

void Resolver::AsyncResolve(const std::string& host_name, int port, AsyncResolveCallback cb) {
  impl_->AsyncResolve(host_name, port, cb);
}

}  // namespace net
