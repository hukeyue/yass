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

/* Copyright (c) 2019-2026 Chilledheart  */

#include "yass/client.h"

#include "cli/cli_server.hpp"
#include "config/config.hpp"
#include "config/config_cli.hpp"
#include "crypto/crypter_export.hpp"

#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include <absl/synchronization/mutex.h>
#include <build/build_config.h>
#include <locale.h>
#include <memory>
#include <string>
#include <vector>
#include "third_party/boringssl/src/include/openssl/crypto.h"
#include "third_party/googleurl/url/gurl.h"
#include "third_party/googleurl/url/url_util.h"

#include "cli/cli_connection_stats.hpp"
#include "crypto/crypter_export.hpp"
#include "net/asio.hpp"
#include "net/padding.hpp"
#include "net/resolver.hpp"
#include "yass/feature.h"
#include "yass/version.h"

using namespace net::cli;

namespace {
class YassClientPrivate {
  asio::ip::tcp::resolver::results_type ResolveAddress(const std::string& domain_name, int port, asio::error_code& ec);
  asio::error_code ListenAddress(int64_t server_tag,
                                 std::string remote_host_name,
                                 std::string remote_host_sni,
                                 uint16_t remote_port,
                                 std::string remote_username,
                                 std::string remote_password,
                                 cipher_method remote_cipher,
                                 bool remote_padding_support,
                                 std::string local_host_name,
                                 uint16_t local_port,
                                 bool redir_mode,
                                 uint16_t* listen_port,
                                 std::string* remote_server_ips_str,
                                 std::string* remote_server_ips_v4_str,
                                 std::string* remote_server_ips_v6_str);
  asio::error_code ListenProxyUri(int64_t server_tag,
                                  std::string_view proxy_uri_str,
                                  std::string_view listen_uri_str,
                                  uint16_t* listen_port,
                                  std::string* remote_server_ips_str,
                                  std::string* remote_server_ips_v4_str,
                                  std::string* remote_server_ips_v6_str);

 public:
  YassClientPrivate() : resolver_(resolver_io_context_) {
    url::AddStandardScheme("auto", url::SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION);
    url::AddStandardScheme("socks", url::SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION);
    url::AddStandardScheme("http2", url::SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION);
    url::AddStandardScheme("naive", url::SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION);
    url::AddStandardScheme("redir", url::SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION);
  }
  ~YassClientPrivate() = default;
  int Init();
  int Add(int64_t server_tag,
          const std::string& proxy_uri_str,
          const std::string& listen_uri_str,
          uint16_t* listen_port,
          std::string* remote_server_ips_str,
          std::string* remote_server_ips_v4_str,
          std::string* remote_server_ips_v6_str);
  int Add(int64_t server_tag,
          std::string remote_host_name,
          std::string remote_host_sni,
          uint16_t remote_port,
          std::string remote_username,
          std::string remote_password,
          cipher_method remote_cipher,
          bool remote_padding_support,
          std::string local_host_name,
          uint16_t local_port,
          bool redir_mode,
          uint16_t* listen_port,
          std::string* remote_server_ips_str,
          std::string* remote_server_ips_v4_str,
          std::string* remote_server_ips_v6_str);
  int Run();  // block current thread
  int NumOfConnections();
  int PostTask(yass_client_task_func_t func, void* arg);

  int Shutdown();  // thread-safe
  int Stop();      // thread-safe
  int GetLastError() const {
    absl::ReaderMutexLock lk(last_error_mutex_);
    return _GetLastError().value();
  }
  const char* GetLastErrorStrUnsafe() const {
    absl::ReaderMutexLock lk(last_error_mutex_);
    return _GetLastErrorStr();
  }
  int GetLastErrorXSI(char* strerrbuf, size_t buflen) const {
    absl::ReaderMutexLock lk(last_error_mutex_);
    return _GetLastErrorXSI(strerrbuf, buflen).value();
  }

 private:
  asio::error_code _GetLastError() const ABSL_SHARED_LOCKS_REQUIRED(last_error_mutex_) { return last_error_; }

  const char* _GetLastErrorStr() const ABSL_SHARED_LOCKS_REQUIRED(last_error_mutex_) {
    auto str = last_error_ss_.str();
    strncpy(last_error_str_, str.c_str(), sizeof(last_error_str_) - 1);
    return last_error_str_;
  }

  asio::error_code _GetLastErrorXSI(char* strerrbuf, size_t buflen) const
      ABSL_SHARED_LOCKS_REQUIRED(last_error_mutex_) {
    auto str = last_error_ss_.str();
    strncpy(strerrbuf, str.c_str(), buflen);
    return last_error_;
  }

  std::ostream& _SetLastError(asio::error_code ec) ABSL_EXCLUSIVE_LOCKS_REQUIRED(last_error_mutex_) {
    last_error_ = ec;
    last_error_ss_.str("");
    last_error_ss_.clear();
    if (ec) {
      return last_error_ss_ << "Error: " << ec << " ";
    }
    return last_error_ss_;
  }

  asio::io_context io_context_;
  std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> work_guard_;

  asio::io_context resolver_io_context_;
  net::Resolver resolver_;

  std::vector<std::unique_ptr<CliServer>> servers_ ABSL_GUARDED_BY(server_mutex_);
  mutable absl::Mutex server_mutex_;

  void _Add(std::unique_ptr<CliServer> server) ABSL_EXCLUSIVE_LOCKS_REQUIRED(server_mutex_);
  void _Clear() ABSL_EXCLUSIVE_LOCKS_REQUIRED(server_mutex_);
  void _Shutdown() ABSL_SHARED_LOCKS_REQUIRED(server_mutex_);
  void _Stop() ABSL_SHARED_LOCKS_REQUIRED(server_mutex_);

  mutable absl::Mutex last_error_mutex_;
  asio::error_code last_error_ ABSL_GUARDED_BY(last_error_mutex_);
  std::stringstream last_error_ss_ ABSL_GUARDED_BY(last_error_mutex_);

  mutable char last_error_str_[256] = {};
};

asio::ip::tcp::resolver::results_type YassClientPrivate::ResolveAddress(const std::string& domain_name,
                                                                        int port,
                                                                        asio::error_code& ec) {
  auto addr = asio::ip::make_address(domain_name.c_str(), ec);
  bool host_is_ip_address = !ec;
  ec = {};
  if (host_is_ip_address) {
    asio::ip::tcp::endpoint endpoint(addr, port);
    auto results = asio::ip::tcp::resolver::results_type::create(endpoint, domain_name, std::to_string(port));
    return results;
  } else {
    auto resolver_work_guard = std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(
        resolver_io_context_.get_executor());
    asio::ip::tcp::resolver::results_type results;
    resolver_.AsyncResolve(domain_name, port,
                           [&](asio::error_code _ec, asio::ip::tcp::resolver::results_type _results) {
                             resolver_work_guard.reset();
                             ec = _ec;
                             if (ec) {
                               absl::WriterMutexLock lk(last_error_mutex_);
                               _SetLastError(ec) << "failed to resolved domain name: " << domain_name;
                               return;
                             }
                             results = std::move(_results);
                           });
    resolver_io_context_.run();
    resolver_io_context_.restart();

    return results;
  }
}

asio::error_code YassClientPrivate::ListenAddress(int64_t server_tag,
                                                  std::string remote_host_name,
                                                  std::string remote_host_sni,
                                                  uint16_t remote_port,
                                                  std::string remote_username,
                                                  std::string remote_password,
                                                  cipher_method remote_cipher,
                                                  bool remote_padding_support,
                                                  std::string local_host_name,
                                                  uint16_t local_port,
                                                  bool redir_mode,
                                                  uint16_t* listen_port,
                                                  std::string* remote_server_ips_str,
                                                  std::string* remote_server_ips_v4_str,
                                                  std::string* remote_server_ips_v6_str) {
  if (remote_host_sni.empty()) {
    remote_host_sni = remote_host_name;
  }
  if (remote_host_sni.empty() || remote_host_sni.size() > TLSEXT_MAXLEN_host_name) {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError(asio::error::invalid_argument) << "Invalid server name or SNI: " << remote_host_sni;
    return _GetLastError();
  }
  if (remote_port == 0u) {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError(asio::error::invalid_argument) << "Invalid server port: " << remote_port;
    return _GetLastError();
  }

  if (!is_valid_cipher_method(remote_cipher)) {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError(asio::error::invalid_argument) << "Invalid Cipher: " << to_cipher_method_str(remote_cipher);
    return _GetLastError();
  }

  if (remote_cipher == CRYPTO_SOCKS4 || remote_cipher == CRYPTO_SOCKS4A) {
    if (!remote_username.empty() || !remote_password.empty()) {
      absl::WriterMutexLock lk(last_error_mutex_);
      _SetLastError(asio::error::invalid_argument) << "SOCKS4/SOCKSA doesn't support username and passsword";
      return _GetLastError();
    }
  }

  if (remote_cipher == CRYPTO_SOCKS5 || remote_cipher == CRYPTO_SOCKS5H) {
    if (remote_username.empty() ^ remote_password.empty()) {
      absl::WriterMutexLock lk(last_error_mutex_);
      _SetLastError(asio::error::invalid_argument) << "SOCKS5/SOCKS5H requires both of username and passsword";
      return _GetLastError();
    }
  }

  if (CIPHER_METHOD_IS_HTTP(remote_cipher)) {
    if (remote_username.empty() ^ remote_password.empty()) {
      absl::WriterMutexLock lk(last_error_mutex_);
      _SetLastError(asio::error::invalid_argument) << "HTTP requires both of username and passsword";
      return _GetLastError();
    }
  }
  if (local_host_name.empty() || local_host_name.size() > TLSEXT_MAXLEN_host_name) {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError(asio::error::invalid_argument) << "Invalid Local Host: " << local_host_name;
    return _GetLastError();
  }

  asio::error_code ec;
  auto results = ResolveAddress(remote_host_name, remote_port, ec);
  if (ec) {
    return ec;
  } else if (results.empty()) {
    return asio::error::host_not_found;
  }

  std::string remote_host_ips;
  std::vector<std::string> remote_ips;
  std::vector<std::string> remote_server_ips_v4, remote_server_ips_v6;
  for (auto result : results) {
    if (result.endpoint().address().is_unspecified()) {
      absl::WriterMutexLock lk(last_error_mutex_);
      _SetLastError(asio::error::invalid_argument) << "Unspecified remote address: " << remote_host_name;
      return _GetLastError();
    }
    remote_ips.push_back(result.endpoint().address().to_string());
    if (result.endpoint().address().is_v4()) {
      remote_server_ips_v4.push_back(result.endpoint().address().to_string());
    } else {
      remote_server_ips_v6.push_back(result.endpoint().address().to_string());
    }
  }
  remote_host_ips = *remote_server_ips_str = absl::StrJoin(remote_ips, ";");
  *remote_server_ips_v4_str = absl::StrJoin(remote_server_ips_v4, ";");
  *remote_server_ips_v6_str = absl::StrJoin(remote_server_ips_v6, ";");
  LOG(INFO) << "resolved server ips: " << remote_host_ips << " from " << remote_host_name;

  std::vector<asio::ip::tcp::endpoint> endpoints;

  results = ResolveAddress(local_host_name, local_port, ec);

  if (ec) {
    return ec;
  } else if (results.empty()) {
    return asio::error::host_not_found;
  } else {
    endpoints.insert(endpoints.end(), std::begin(results), std::end(results));

    std::vector<std::string> local_ips;
    for (auto result : results) {
      local_ips.push_back(result.endpoint().address().to_string());
    }
    LOG(INFO) << "resolved local ips: " << absl::StrJoin(local_ips, ";") << " from " << local_host_name;
  }

  auto server = std::make_unique<CliServer>(io_context_, server_tag, remote_host_ips, remote_host_sni, remote_port,
                                            remote_username, remote_password, remote_cipher, remote_padding_support);
  for (auto& endpoint : endpoints) {
    server->listen(endpoint, {}, {}, {}, {}, {}, redir_mode, SOMAXCONN, ec);
    if (ec) {
      absl::WriterMutexLock lk(last_error_mutex_);
      _SetLastError(ec) << "failed to tag " << server_tag << " listen";
      return _GetLastError();
    }
    endpoint = server->endpoint();
    LOG(WARNING) << "tag " << server_tag << " tcp server listening at " << endpoint
                 << " with upstream sni: " << remote_host_sni << ":" << remote_port << " (ip " << remote_host_ips
                 << " )";
    if (listen_port) {
      *listen_port = endpoint.port();
    }
  }
  absl::WriterMutexLock lk(server_mutex_);
  _Add(std::move(server));
  return {};
}

void YassClientPrivate::_Add(std::unique_ptr<CliServer> server) {
  servers_.emplace_back(std::move(server));
}

asio::error_code YassClientPrivate::ListenProxyUri(int64_t server_tag,
                                                   std::string_view proxy_uri_str,
                                                   std::string_view listen_uri_str,
                                                   uint16_t* listen_port,
                                                   std::string* remote_server_ips_str,
                                                   std::string* remote_server_ips_v4_str,
                                                   std::string* remote_server_ips_v6_str) {
  std::string remote_host_name;
  std::string remote_host_sni;
  uint16_t remote_port;
  std::string remote_username;
  std::string remote_password;
  cipher_method remote_cipher;
  bool remote_padding_support = false;
  std::string local_host_name;
  uint16_t local_port;
  bool redir_mode = false;

  GURL proxy_uri(proxy_uri_str);
  if (!proxy_uri.is_valid() || !proxy_uri.has_host() || !proxy_uri.has_scheme()) {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError(asio::error::invalid_argument) << "Invalid Proxy URL: " << proxy_uri_str;
    return _GetLastError();
  }
  remote_host_name = proxy_uri.host();
  remote_port = proxy_uri.EffectiveIntPort();
  remote_username = proxy_uri.username();
  remote_password = proxy_uri.password();

  if (proxy_uri.scheme() == "https") {
    remote_cipher = CRYPTO_HTTPS;
  } else if (proxy_uri.scheme() == "http2") {
    remote_cipher = CRYPTO_HTTP2;
    if (!proxy_uri.has_port()) {
      remote_port = 443u;
    }
  } else if (proxy_uri.scheme() == "naive") {
    remote_padding_support = true;
    remote_cipher = CRYPTO_HTTP2;
    if (!proxy_uri.has_port()) {
      remote_port = 443u;
    }
  } else if (proxy_uri.scheme() == "socks") {
    remote_cipher = CRYPTO_SOCKS5H;
    if (!proxy_uri.has_port()) {
      absl::WriterMutexLock lk(last_error_mutex_);
      _SetLastError(asio::error::invalid_argument)
          << "Invalid Proxy URL: " << proxy_uri_str << " Port is required for socks";
      return _GetLastError();
    }
  } else {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError(asio::error::invalid_argument) << "Invalid Proxy Scheme: " << proxy_uri.scheme();
    return _GetLastError();
  }

  GURL listen_uri(listen_uri_str);
  if (!listen_uri.is_valid() || !listen_uri.has_host() || !listen_uri.has_scheme()) {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError(asio::error::invalid_argument) << "Invalid Listen URL: " << listen_uri_str;
    return _GetLastError();
  }
  if (listen_uri.scheme() != "auto" && listen_uri.scheme() != "redir") {
    LOG(WARNING) << "Following listen uri's scheme is ignored due to automatical detection: " << listen_uri.scheme();
  }
  local_host_name = listen_uri.host();
  local_port = listen_uri.EffectiveIntPort();
  if (!listen_uri.has_port()) {
    local_port = 1080u;
  }
  if (listen_uri.scheme() == "redir") {
    redir_mode = true;
  } else {
    redir_mode = false;
  }

  return ListenAddress(server_tag, remote_host_name, remote_host_sni, remote_port, remote_username, remote_password,
                       remote_cipher, remote_padding_support, local_host_name, local_port, redir_mode, listen_port,
                       remote_server_ips_str, remote_server_ips_v4_str, remote_server_ips_v6_str);
}

int YassClientPrivate::Init() {
#ifndef _WIN32
  // setup signal handler
  signal(SIGPIPE, SIG_IGN);

  /* Block SIGPIPE in all threads, this can happen if a thread calls write on
     a closed pipe. */
  sigset_t sigpipe_mask;
  sigemptyset(&sigpipe_mask);
  sigaddset(&sigpipe_mask, SIGPIPE);
  sigset_t saved_mask;
  if (pthread_sigmask(SIG_BLOCK, &sigpipe_mask, &saved_mask) == -1) {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError(asio::error_code(errno, asio::error::get_system_category())) << "pthread_sigmask failed";
    return -1;
  }
#endif

#if BUILDFLAG(IS_POSIX) && !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_OHOS)
  // Set C library locale to make sure CommandLine can parse
  // argument values in the correct encoding and to make sure
  // generated file names (think downloads) are in the file system's
  // encoding.
  setlocale(LC_ALL, "");
  // For numbers we never want the C library's locale sensitive
  // conversion from number to string because the only thing it
  // changes is the decimal separator which is not good enough for
  // the UI and can be harmful elsewhere.
  setlocale(LC_NUMERIC, "C");
#endif

#ifdef _WIN32
  int iResult = 0;
  WSADATA wsaData = {0};
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError(asio::error::operation_not_supported) << "WSAStartup failure";
    return -1;
  }
#endif

  {
    absl::WriterMutexLock lk(server_mutex_);
    _Clear();
  }

  auto ec = resolver_.Init();
  if (ec) {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError(ec) << "Resolver: Init failure";
    return -1;
  }
  {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError({});
  }
  return 0;
}

void YassClientPrivate::_Clear() {
  servers_.clear();
}

int YassClientPrivate::Add(int64_t server_tag,
                           const std::string& proxy_uri_str,
                           const std::string& listen_uri_str,
                           uint16_t* listen_port,
                           std::string* remote_server_ips_str,
                           std::string* remote_server_ips_v4_str,
                           std::string* remote_server_ips_v6_str) {
  asio::error_code ec;

  ec = ListenProxyUri(server_tag, proxy_uri_str, listen_uri_str, listen_port, remote_server_ips_str,
                      remote_server_ips_v4_str, remote_server_ips_v6_str);
  if (ec) {
    return -1;
  }

  {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError({});
  }

  return 0;
}

int YassClientPrivate::Add(int64_t server_tag,
                           std::string remote_host_name,
                           std::string remote_host_sni,
                           uint16_t remote_port,
                           std::string remote_username,
                           std::string remote_password,
                           cipher_method remote_cipher,
                           bool remote_padding_support,
                           std::string local_host_name,
                           uint16_t local_port,
                           bool redir_mode,
                           uint16_t* listen_port,
                           std::string* remote_server_ips_str,
                           std::string* remote_server_ips_v4_str,
                           std::string* remote_server_ips_v6_str) {
  asio::error_code ec;

  ec = ListenAddress(server_tag, remote_host_name, remote_host_sni, remote_port, remote_username, remote_password,
                     remote_cipher, remote_padding_support, local_host_name, local_port, redir_mode, listen_port,
                     remote_server_ips_str, remote_server_ips_v4_str, remote_server_ips_v6_str);
  if (ec) {
    return -1;
  }

  {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError({});
  }

  return 0;
}

int YassClientPrivate::Run() {
  DCHECK(!work_guard_);
  work_guard_ =
      std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(io_context_.get_executor());

  io_context_.run();
  io_context_.restart();

  {
    absl::WriterMutexLock lk(server_mutex_);
    _Clear();
  }

  {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError({});
  }

  return 0;
}

int YassClientPrivate::NumOfConnections() {
  int count = 0;
  {
    absl::ReaderMutexLock lk(server_mutex_);
    for (auto& server : servers_)
      count += server->num_of_connections();
  }
  {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError({});
  }
  return count;
}

int YassClientPrivate::PostTask(yass_client_task_func_t func, void* arg) {
  if (func == nullptr) {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError(asio::error::invalid_argument) << "function pointer cannout be NULL";
    return -1;
  }
  asio::post(io_context_, [=]() { func(arg); });
  {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError({});
  }
  return 0;
}

int YassClientPrivate::Shutdown() {
  /// shutdown in the worker thread
  asio::post(io_context_, [this]() {
    {
      absl::ReaderMutexLock lk(server_mutex_);
      _Shutdown();
    }
    work_guard_.reset();
  });
  {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError({});
  }
  return 0;
}

void YassClientPrivate::_Shutdown() {
  LOG(WARNING) << "Client shuting down";
  for (auto& server : servers_)
    server->shutdown();
}

int YassClientPrivate::Stop() {
  /// stop in the worker thread
  asio::post(io_context_, [this]() {
    {
      absl::ReaderMutexLock lk(server_mutex_);
      _Stop();
    }
    work_guard_.reset();
  });
  {
    absl::WriterMutexLock lk(last_error_mutex_);
    _SetLastError({});
  }
  return 0;
}

void YassClientPrivate::_Stop() {
  LOG(WARNING) << "Client stopping";
  for (auto& server : servers_)
    server->stop();
}

}  // anonymous namespace

yass_client_instance yass_client_instance_create() {
  auto instance = std::make_unique<YassClientPrivate>();
  return instance.release();
}

int yass_client_instance_init(yass_client_instance _instance) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  return instance->Init();
}

int yass_client_instance_add_server_uri(yass_client_instance _instance,
                                        int64_t server_tag,
                                        const char* proxy_uri,
                                        const char* listen_uri,
                                        uint16_t* listen_port) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  std::string remote_server_ips_str, remote_server_ips_v4_str, remote_server_ips_v6_str;
  return instance->Add(server_tag, proxy_uri ? proxy_uri : std::string(), listen_uri ? listen_uri : std::string(),
                       listen_port, &remote_server_ips_str, &remote_server_ips_v4_str, &remote_server_ips_v6_str);
}

int yass_client_instance_add_server_uri_v1(yass_client_instance _instance,
                                           int64_t server_tag,
                                           const char* proxy_uri,
                                           const char* listen_uri,
                                           uint16_t* listen_port,
                                           char* remote_server_ips_cstr,
                                           size_t* remote_server_ips_cstr_len,
                                           char* remote_server_ips_v4_cstr,
                                           size_t* remote_server_ips_v4_cstr_len,
                                           char* remote_server_ips_v6_cstr,
                                           size_t* remote_server_ips_v6_cstr_len) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  std::string remote_server_ips_str, remote_server_ips_v4_str, remote_server_ips_v6_str;
  int ret = instance->Add(server_tag, proxy_uri ? proxy_uri : std::string(), listen_uri ? listen_uri : std::string(),
                          listen_port, &remote_server_ips_str, &remote_server_ips_v4_str, &remote_server_ips_v6_str);
  if (remote_server_ips_cstr && remote_server_ips_cstr_len) {
    *remote_server_ips_cstr_len = std::min(*remote_server_ips_cstr_len, remote_server_ips_str.size());
    strncpy(remote_server_ips_cstr, remote_server_ips_str.c_str(), *remote_server_ips_cstr_len);
  }
  if (remote_server_ips_v4_cstr && remote_server_ips_v4_cstr_len) {
    *remote_server_ips_v4_cstr_len = std::min(*remote_server_ips_v4_cstr_len, remote_server_ips_v4_str.size());
    strncpy(remote_server_ips_v4_cstr, remote_server_ips_v4_str.c_str(), *remote_server_ips_v4_cstr_len);
  }
  if (remote_server_ips_v6_cstr && remote_server_ips_v6_cstr_len) {
    *remote_server_ips_v6_cstr_len = std::min(*remote_server_ips_v6_cstr_len, remote_server_ips_v6_str.size());
    strncpy(remote_server_ips_v6_cstr, remote_server_ips_v6_str.c_str(), *remote_server_ips_v6_cstr_len);
  }
  return ret;
}

int yass_client_instance_add_server(yass_client_instance _instance,
                                    int64_t server_tag,
                                    const char* remote_host_name,
                                    const char* remote_host_sni,
                                    uint16_t remote_port,
                                    const char* remote_username,
                                    const char* remote_password,
                                    int remote_cipher,
                                    bool remote_padding_support,
                                    const char* local_host_name,
                                    uint16_t local_port,
                                    bool redir_mode,
                                    uint16_t* listen_port) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  std::string remote_server_ips_str, remote_server_ips_v4_str, remote_server_ips_v6_str;
  return instance->Add(
      server_tag, remote_host_name ? remote_host_name : std::string(),
      remote_host_sni ? remote_host_sni : std::string(), remote_port, remote_username ? remote_username : std::string(),
      remote_password ? remote_password : std::string(), (cipher_method)remote_cipher, remote_padding_support,
      local_host_name ? local_host_name : std::string(), local_port, redir_mode, listen_port, &remote_server_ips_str,
      &remote_server_ips_v4_str, &remote_server_ips_v6_str);
}

int yass_client_instance_add_server_v1(yass_client_instance _instance,
                                       int64_t server_tag,
                                       const char* remote_host_name,
                                       const char* remote_host_sni,
                                       uint16_t remote_port,
                                       const char* remote_username,
                                       const char* remote_password,
                                       int remote_cipher,
                                       bool remote_padding_support,
                                       const char* local_host_name,
                                       uint16_t local_port,
                                       bool redir_mode,
                                       uint16_t* listen_port,
                                       char* remote_server_ips_cstr,
                                       size_t* remote_server_ips_cstr_len,
                                       char* remote_server_ips_v4_cstr,
                                       size_t* remote_server_ips_v4_cstr_len,
                                       char* remote_server_ips_v6_cstr,
                                       size_t* remote_server_ips_v6_cstr_len) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  std::string remote_server_ips_str, remote_server_ips_v4_str, remote_server_ips_v6_str;
  int ret = instance->Add(
      server_tag, remote_host_name ? remote_host_name : std::string(),
      remote_host_sni ? remote_host_sni : std::string(), remote_port, remote_username ? remote_username : std::string(),
      remote_password ? remote_password : std::string(), (cipher_method)remote_cipher, remote_padding_support,
      local_host_name ? local_host_name : std::string(), local_port, redir_mode, listen_port, &remote_server_ips_str,
      &remote_server_ips_v4_str, &remote_server_ips_v6_str);
  if (remote_server_ips_cstr && remote_server_ips_cstr_len) {
    *remote_server_ips_cstr_len = std::min(*remote_server_ips_cstr_len, remote_server_ips_str.size());
    strncpy(remote_server_ips_cstr, remote_server_ips_str.c_str(), *remote_server_ips_cstr_len);
  }
  if (remote_server_ips_v4_cstr && remote_server_ips_v4_cstr_len) {
    *remote_server_ips_v4_cstr_len = std::min(*remote_server_ips_v4_cstr_len, remote_server_ips_v4_str.size());
    strncpy(remote_server_ips_v4_cstr, remote_server_ips_v4_str.c_str(), *remote_server_ips_v4_cstr_len);
  }
  if (remote_server_ips_v6_cstr && remote_server_ips_v6_cstr_len) {
    *remote_server_ips_v6_cstr_len = std::min(*remote_server_ips_v6_cstr_len, remote_server_ips_v6_str.size());
    strncpy(remote_server_ips_v6_cstr, remote_server_ips_v6_str.c_str(), *remote_server_ips_v6_cstr_len);
  }
  return ret;
}

int yass_client_instance_run(yass_client_instance _instance) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  return instance->Run();
}

int yass_client_instance_num_of_connections(yass_client_instance _instance) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  return instance->NumOfConnections();
}

int yass_client_instance_shutdown(yass_client_instance _instance) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  return instance->Shutdown();
}

int yass_client_instance_cancel(yass_client_instance _instance) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  return instance->Stop();
}

void yass_client_instance_destroy(yass_client_instance _instance) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  if (instance == nullptr) {
    return;
  }
  delete instance;
}

int yass_client_instance_get_last_error(yass_client_instance _instance) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  return instance->GetLastError();
}

const char* yass_client_instance_get_last_error_str(yass_client_instance _instance) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  return instance->GetLastErrorStrUnsafe();
}

int yass_client_instance_get_last_error_xsi_r(yass_client_instance _instance, char* strerrbuf, size_t buflen) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  return instance->GetLastErrorXSI(strerrbuf, buflen);
}

int yass_client_instance_post_task(yass_client_instance _instance, yass_client_task_func_t func, void* arg) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  return instance->PostTask(func, arg);
}
