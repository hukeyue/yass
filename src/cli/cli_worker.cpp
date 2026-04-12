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

/* Copyright (c) 2022-2026 Chilledheart  */

#include "cli/cli_worker.hpp"

#include <absl/flags/flag.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include "third_party/boringssl/src/include/openssl/crypto.h"

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <pthread.h>
#include <signal.h>
#endif

#include "core/utils.hpp"
#include "net/padding.hpp"
#include "yass/client.h"

using namespace std::string_literals;

class WorkerPrivate {
 public:
  WorkerPrivate() : instance_(yass_client_instance_create()) {
    VLOG(1) << "worker: allocated memory";
  }
  ~WorkerPrivate() {
    VLOG(1) << "worker: freed memory";
    yass_client_instance_destroy(instance_);
  }

  int Init() {
    return yass_client_instance_init(instance_);
  }

  int Add(int64_t server_tag, const char* remote_host_name, const char* remote_host_sni, uint16_t remote_port, const char* remote_username, const char* remote_password, int remote_cipher, bool remote_padding_support, const char* local_host_name, uint16_t local_port, bool redir_mode, uint16_t* listen_port) {
    return yass_client_instance_add_server(instance_, server_tag, remote_host_name, remote_host_sni, remote_port, remote_username, remote_password, remote_cipher, remote_padding_support, local_host_name, local_port, redir_mode, listen_port);
  }

  int Run() {
    return yass_client_instance_run(instance_);
  }

  int NumOfConnections() {
    return yass_client_instance_num_of_connections(instance_);
  }

  int Stop() {
    return yass_client_instance_cancel(instance_);
  }

  asio::error_code GetLastError() const {
    return asio::error_code(yass_client_instance_get_last_error(instance_), asio::error::system_category);
  }

  const char* GetLastErrorStr() const {
    return yass_client_instance_get_last_error_str(instance_);
  }

 private:
  yass_client_instance instance_;
};

Worker::Worker()
    : private_(new WorkerPrivate) {}

Worker::~Worker() {
  start_callback_ = nullptr;
  stop_callback_ = nullptr;

  Stop(nullptr);
  if (thread_) {
    thread_->join();
  }

  delete private_;
}

void Worker::Start(absl::AnyInvocable<void(asio::error_code)>&& callback) {
  DCHECK(!start_callback_);

  start_callback_ = std::move(callback);

  if (thread_) {
    thread_->join();
  }
  thread_ = std::make_unique<std::thread>([this] {
    WorkFunc();
  });
}

void Worker::Stop(absl::AnyInvocable<void()>&& callback) {
  DCHECK(!stop_callback_);
  stop_callback_ = std::move(callback);
  /// stop in the worker thread
  private_->Stop();
}

size_t Worker::currentConnections() const {
  return private_->NumOfConnections();
}

std::vector<std::string> Worker::GetRemoteIpsV4() const {
  return remote_server_ips_v4_;
}

std::vector<std::string> Worker::GetRemoteIpsV6() const {
  return remote_server_ips_v6_;
}

std::string Worker::GetDomain() const {
  return absl::StrCat(cached_local_host_, ":", cached_local_port_);
}

std::string Worker::GetRemoteDomain() const {
  return absl::StrCat(cached_server_host_, ":", cached_server_port_);
}

int Worker::GetLocalPort() const {
  return local_port_;
}

void Worker::WorkFunc() {
  if (!SetCurrentThreadName("background"s)) {
    PLOG(WARNING) << "worker: failed to set thread name";
  }
  if (!SetCurrentThreadPriority(ThreadPriority::ABOVE_NORMAL)) {
    PLOG(WARNING) << "worker: failed to set thread priority";
  }

  LOG(INFO) << "worker: background thread started";

#ifndef _WIN32
  /* Check if we have blocked SIGPIPE in all threads, this can happen if a thread calls write on
     a closed pipe. */
  sigset_t saved_mask;
  if (pthread_sigmask(SIG_BLOCK, nullptr, &saved_mask) == 0) {
    if (sigismember(&saved_mask, SIGPIPE)) {
      LOG(INFO) << "worker: signal SIGPIPE is masked as BLOCKED";
    } else {
      PLOG(WARNING) << "worker: signal SIGPIPE is not masked as BLOCKED!";
    }
  } else {
    PLOG(WARNING) << "worker: pthread_sigmask failed";
  }
#endif

  // cache all fields
  cached_server_host_ = absl::GetFlag(FLAGS_server_host);
  cached_server_sni_ = absl::GetFlag(FLAGS_server_sni);
  cached_server_port_ = absl::GetFlag(FLAGS_server_port);
  cached_server_username_ = absl::GetFlag(FLAGS_username);
  cached_server_password_ = absl::GetFlag(FLAGS_password);
  cached_server_cipher_ = absl::GetFlag(FLAGS_method).method;
  cached_server_padding_support_ = absl::GetFlag(FLAGS_padding_support);
  cached_server_redir_mode_ = absl::GetFlag(FLAGS_redir_mode);
  cached_local_host_ = absl::GetFlag(FLAGS_local_host);
  cached_local_port_ = absl::GetFlag(FLAGS_local_port);

  int ret = private_->Init();
  if (ret < 0) {
    LOG(WARNING) << "worker: resolver error: " << private_->GetLastErrorStr();
    on_resolve_done(asio::error::connection_refused);
    return;
  }

  std::string host_name = cached_server_host_;
  uint16_t port = cached_server_port_;
  remote_server_sni_ = !cached_server_sni_.empty() ? cached_server_sni_ : cached_server_host_;
  DCHECK_LE(remote_server_sni_.size(), (unsigned int)TLSEXT_MAXLEN_host_name);

  local_port_ = 0;
  ret = private_->Add(0, host_name.c_str(), remote_server_sni_.c_str(), port, cached_server_username_.c_str(), cached_server_password_.c_str(), cached_server_cipher_, cached_server_padding_support_, cached_local_host_.c_str(), cached_local_port_, cached_server_redir_mode_, &local_port_);
  if (ret != 0) {
    LOG(WARNING) << "worker: resolver error: " << private_->GetLastErrorStr();
    on_resolve_done(private_->GetLastError());
    return;
  }

  LOG(INFO) << "worker: tcp server listening at " << local_port_;

  on_resolve_done({});

  private_->Run();

  auto callback = std::move(stop_callback_);
  DCHECK(!stop_callback_);
  if (callback) {
    callback();
  }

  LOG(INFO) << "worker: background thread stopped";
}

void Worker::on_resolve_done(asio::error_code ec) {
  if (auto callback = std::move(start_callback_)) {
    callback(ec);
  }
}
