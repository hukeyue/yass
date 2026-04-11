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

/* Copyright (c) 2019-2026 Chilledheart  */

#include "yass/client.h"

#include "cli/cli_server.hpp"
#include "config/config.hpp"
#include "config/config_cli.hpp"
#include "crypto/crypter_export.hpp"

#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include <build/build_config.h>
#include <locale.h>
#include <memory>
#include <string>
#include <vector>
#include "third_party/googleurl/url/gurl.h"
#include "third_party/googleurl/url/url_util.h"
#include "third_party/boringssl/src/include/openssl/crypto.h"

#include "cli/cli_connection_stats.hpp"
#include "crypto/crypter_export.hpp"
#include "net/asio.hpp"
#include "net/padding.hpp"
#include "net/resolver.hpp"
#include "yass/feature.h"
#include "yass/version.h"

#if 0
namespace config {
const ProgramType pType = YASS_CLIENT_LIB;
}  // namespace config
#endif

using namespace net::cli;

namespace {
class YassClientPrivate {
  asio::ip::tcp::resolver::results_type ResolveAddress(const std::string& domain_name, int port, asio::error_code &ec);
  asio::error_code ListenAddress(int64_t server_tag, std::string remote_host_name, std::string remote_host_sni, uint16_t remote_port, std::string remote_username, std::string remote_password, cipher_method remote_cipher, bool remote_padding_support, std::string local_host_name, uint16_t local_port, bool redir_mode, uint16_t *listen_port);
  asio::error_code ListenProxyUri(int64_t server_tag, std::string_view proxy_uri_str, std::string_view listen_uri_str, uint16_t *listen_port);

 public:
  YassClientPrivate() : resolver_(resolver_io_context_) {
    url::AddStandardScheme("auto",
                           url::SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION);
    url::AddStandardScheme("socks",
                           url::SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION);
    url::AddStandardScheme("http2",
                           url::SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION);
    url::AddStandardScheme("naive",
                           url::SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION);
    url::AddStandardScheme("redir",
                           url::SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION);
  }
  ~YassClientPrivate() = default;
  int Init();
  int Add(int64_t server_tag, const std::string& proxy_uri_str, const std::string& listen_uri_str, uint16_t *listen_port);
  int Add(int64_t server_tag, std::string remote_host_name, std::string remote_host_sni, uint16_t remote_port, std::string remote_username, std::string remote_password, cipher_method remote_cipher, bool remote_padding_support, std::string local_host_name, uint16_t local_port, bool redir_mode, uint16_t *listen_port);
  int Run(); // block current thread
  int NumOfConnections();

  int Shutdown(); // thread-safe
  int Stop(); // thread-safe
  int GetLastError() const { return last_error_.value(); }
  const char* GetLastErrorStr() const {
    auto str = last_error_ss_.str();
    strncpy(last_error_str_, str.c_str(), sizeof(last_error_str_));
    return last_error_str_;
  }

 private:
  asio::io_context io_context_;
  std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> work_guard_;

  asio::io_context resolver_io_context_;
  net::Resolver resolver_;

  std::vector<std::unique_ptr<CliServer>> servers_;
  asio::error_code last_error_;
  mutable char last_error_str_[4096];
  std::stringstream last_error_ss_;
};

asio::ip::tcp::resolver::results_type YassClientPrivate::ResolveAddress(const std::string& domain_name, int port, asio::error_code &ec) {
  auto addr = asio::ip::make_address(domain_name.c_str(), ec);
  bool host_is_ip_address = !ec;
  ec = {};
  if (host_is_ip_address) {
    asio::ip::tcp::endpoint endpoint(addr, port);
    auto results = asio::ip::tcp::resolver::results_type::create(endpoint, domain_name, std::to_string(port));
    return results;
  } else {
    auto resolver_work_guard =
        std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(resolver_io_context_.get_executor());
    asio::ip::tcp::resolver::results_type results;
    resolver_.AsyncResolve(domain_name, port, [&](asio::error_code ec, asio::ip::tcp::resolver::results_type _results) {
      resolver_work_guard.reset();
      last_error_ = ec;
      if (ec) {
        last_error_ss_ << "resolved domain name: " << domain_name << " failed due to: " << ec;
        return;
      }
      results = std::move(_results);
    });
    resolver_io_context_.run();
    resolver_io_context_.restart();
    ec = last_error_;

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
                                                  bool redir_mode, uint16_t *listen_port) {
  if (remote_host_sni.empty()) {
    remote_host_sni = remote_host_name;
  }
  if (remote_host_sni.size() > TLSEXT_MAXLEN_host_name) {
    last_error_ss_ << "Invalid server name or SNI: " << remote_host_sni;
    last_error_ = asio::error::invalid_argument;
    return last_error_;
  }
  std::string remote_host_ips;
  if (remote_port == 0u) {
    last_error_ss_ << "Invalid server port: " << remote_port;
    last_error_ = asio::error::invalid_argument;
    return last_error_;
  }

  asio::error_code ec;
  auto results = ResolveAddress(remote_host_name, remote_port, ec);
  if (ec) {
    return ec;
  } else if (results.empty()) {
    return asio::error::host_not_found;
  } else {
    std::vector<std::string> remote_ips;
    for (auto result : results) {
      if (result.endpoint().address().is_unspecified()) {
        last_error_ss_ << "Unspecified remote address: " << remote_host_name;
        last_error_ = asio::error::invalid_argument;
        return last_error_;
      }
      remote_ips.push_back(result.endpoint().address().to_string());
    }
    remote_host_ips = absl::StrJoin(remote_ips, ";");
    LOG(INFO) << "resolved server ips: " << remote_host_ips << " from " << remote_host_name;
  }

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
      last_error_ss_ << "tag " << server_tag << " listen failed due to: " << ec;
      last_error_ = ec;
      return ec;
    }
    endpoint = server->endpoint();
    LOG(WARNING) << "tag " << server_tag << " tcp server listening at " << endpoint
                 << " with upstream sni: " << remote_host_sni << ":" << remote_port
                 << " (ip " << remote_host_ips << " )";
    if (listen_port) {
      *listen_port = endpoint.port();
    }
  }
  servers_.push_back(std::move(server));
  return {};
}

asio::error_code YassClientPrivate::ListenProxyUri(int64_t server_tag, std::string_view proxy_uri_str, std::string_view listen_uri_str, uint16_t* listen_port) {
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
    last_error_ss_ << "Invalid Proxy URL: " << proxy_uri_str;
    last_error_ =  asio::error::invalid_argument;
    return last_error_;
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
      last_error_ss_ << "Invalid Proxy URL: " << proxy_uri_str << " Port is required for socks";
      last_error_ =  asio::error::invalid_argument;
      return last_error_;
    }
  } else {
    last_error_ss_ << "Invalid Proxy Scheme: " << proxy_uri.scheme();
    last_error_ =  asio::error::invalid_argument;
    return last_error_;
  }

  GURL listen_uri(listen_uri_str);
  if (!listen_uri.is_valid() || !listen_uri.has_host() || !listen_uri.has_scheme()) {
    last_error_ss_ << "Invalid Listen URL: " << listen_uri_str;
    last_error_ =  asio::error::invalid_argument;
    return last_error_;
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

  return ListenAddress(server_tag, remote_host_name, remote_host_sni, remote_port,
                       remote_username, remote_password, remote_cipher, remote_padding_support,
                       local_host_name, local_port, redir_mode, listen_port);
}

int YassClientPrivate::Init() {
  DCHECK(!work_guard_);
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
    last_error_ = asio::error_code(errno, asio::error::get_system_category());
    last_error_ss_ << "pthread_sigmask failed";
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
    last_error_ = asio::error::operation_not_supported;
    last_error_ss_ << "WSAStartup failure";
    return -1;
  }
#endif

  work_guard_.reset();
  servers_.clear();

  if (resolver_.Init() < 0) {
    last_error_ = asio::error::operation_not_supported;
    last_error_ss_ << "Resolver: Init failure";
    return -1;
  }
  return 0;
}

int YassClientPrivate::Add(int64_t server_tag, const std::string& proxy_uri_str, const std::string& listen_uri_str, uint16_t *listen_port) {
  DCHECK(!work_guard_);
  asio::error_code ec;

  ec = ListenProxyUri(server_tag, proxy_uri_str, listen_uri_str, listen_port);
  if (ec) {
    return -1;
  }

  return 0;
}

int YassClientPrivate::Add(int64_t server_tag, std::string remote_host_name, std::string remote_host_sni, uint16_t remote_port, std::string remote_username, std::string remote_password, cipher_method remote_cipher, bool remote_padding_support, std::string local_host_name, uint16_t local_port, bool redir_mode, uint16_t *listen_port) {
  DCHECK(!work_guard_);
  asio::error_code ec;

  ec = ListenAddress(server_tag, remote_host_name, remote_host_sni, remote_port,
                     remote_username, remote_password, remote_cipher, remote_padding_support,
                     local_host_name, local_port, redir_mode, listen_port);
  if (ec) {
    return -1;
  }

  return 0;
}

int YassClientPrivate::Run() {
  DCHECK(!work_guard_);
  work_guard_ =
      std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(io_context_.get_executor());

  io_context_.run();
  io_context_.restart();

  servers_.clear();

  return 0;
}

int YassClientPrivate::NumOfConnections() {
  int count = 0;
  for (auto& server : servers_)
    count += server->num_of_connections();
  return count;
}

int YassClientPrivate::Shutdown() {
  /// shutdown in the worker thread
  asio::post(io_context_, [this]() {
    LOG(WARNING) << "Application shuting down";

    for (auto& server : servers_)
      server->shutdown();

    work_guard_.reset();
  });
  return 0;
}

int YassClientPrivate::Stop() {
  /// stop in the worker thread
  asio::post(io_context_, [this]() {
    LOG(WARNING) << "Application stopping";

    for (auto& server : servers_)
      server->stop();

    work_guard_.reset();
  });
  return 0;
}

} // anonymous namespace

yass_client_instance yass_client_instance_create() {
  auto instance = std::make_unique<YassClientPrivate>();
  return instance.release();
}

int yass_client_instance_init(yass_client_instance _instance) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  return instance->Init();
}

int yass_client_instance_add_server_uri(yass_client_instance _instance, int64_t server_tag, const char* proxy_uri, const char* listen_uri, uint16_t* listen_port) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  if (!proxy_uri || !listen_uri) {
    return -1;
  }
  return instance->Add(server_tag, proxy_uri, listen_uri, listen_port);
}

int yass_client_instance_add_server(yass_client_instance _instance, int64_t server_tag, const char* remote_host_name, const char* remote_host_sni, uint16_t remote_port, const char* remote_username, const char* remote_password, int remote_cipher, bool remote_padding_support, const char* local_host_name, uint16_t local_port, bool redir_mode, uint16_t* listen_port) {
  auto instance = reinterpret_cast<YassClientPrivate*>(_instance);
  DCHECK(instance);
  if (!remote_host_name || !remote_host_sni || !remote_username || !remote_password || ! local_host_name) {
    return -1;
  }
  return instance->Add(server_tag, remote_host_name, remote_host_sni, remote_port, remote_username, remote_password, (cipher_method)remote_cipher, remote_padding_support, local_host_name, local_port, redir_mode, listen_port);
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
  return instance->GetLastErrorStr();
}
