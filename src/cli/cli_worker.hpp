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

#ifndef YASS_CLI_WORKER
#define YASS_CLI_WORKER

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include <absl/functional/any_invocable.h>

#include "config/config.hpp"
#include "core/logging.hpp"
#include "crypto/crypter_export.hpp"
#include "net/asio.hpp"
#include "net/resolver.hpp"

class WorkerPrivate;
class Worker {
 public:
  Worker();
  ~Worker();

  void Start(absl::AnyInvocable<void(asio::error_code)>&& callback);
  void Stop(absl::AnyInvocable<void()>&& callback);

  std::vector<std::string> GetRemoteIpsV4() const;
  std::vector<std::string> GetRemoteIpsV6() const;
  std::string GetDomain() const;
  std::string GetRemoteDomain() const;
  int GetLocalPort() const;

  size_t currentConnections() const;

 private:
  void WorkFunc();

  void on_resolve_remote(asio::error_code ec, asio::ip::tcp::resolver::results_type results);

  void on_resolve_local(asio::error_code ec, asio::ip::tcp::resolver::results_type results);

  void on_resolve_done(asio::error_code ec);

  asio::io_context io_context_;
  /// stopping the io_context from running out of work
  std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> work_guard_;
  /// used to resolve local and remote endpoint
  net::Resolver resolver_;
  /// used to do io in another thread
  std::unique_ptr<std::thread> thread_;

  absl::AnyInvocable<void(asio::error_code)> start_callback_;
  absl::AnyInvocable<void()> stop_callback_;

  // cached entry
  std::string cached_server_host_;
  std::string cached_server_sni_;
  uint16_t cached_server_port_;
  std::string cached_local_host_;
  uint16_t cached_local_port_;

  WorkerPrivate* private_;
  std::string remote_server_ips_;
  std::vector<std::string> remote_server_ips_v4_;
  std::vector<std::string> remote_server_ips_v6_;
  std::string remote_server_sni_;
  std::string local_server_ips_;
  uint16_t local_port_ = 0;
  std::vector<asio::ip::tcp::endpoint> endpoints_;
  std::atomic_bool in_destroy_ = false;
};

#endif  // YASS_CLI_WORKER
