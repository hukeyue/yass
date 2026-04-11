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

#include "cli/cli_server.hpp"
#include "config/config.hpp"
#include "config/config_cli.hpp"
#include "crypto/crypter_export.hpp"

#include <absl/debugging/failure_signal_handler.h>
#include <absl/debugging/symbolize.h>
#include <absl/flags/flag.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include <build/build_config.h>
#include <locale.h>
#include <memory>
#include <vector>
#include "third_party/googleurl/url/gurl.h"
#include "third_party/googleurl/url/url_util.h"
#include "third_party/boringssl/src/include/openssl/crypto.h"

#include "cli/cli_connection_stats.hpp"
#include "core/logging.hpp"
#include "crypto/crypter_export.hpp"
#include "net/asio.hpp"
#include "net/padding.hpp"
#include "net/resolver.hpp"
#include "yass/version.h"

namespace config {
const ProgramType pType = YASS_CLIENT_DEFAULT;
}  // namespace config

using namespace net::cli;

static asio::ip::tcp::resolver::results_type ResolveAddress(const std::string& domain_name, int port) {
  asio::error_code ec;
  auto addr = asio::ip::make_address(domain_name.c_str(), ec);
  bool host_is_ip_address = !ec;
  if (host_is_ip_address) {
    asio::ip::tcp::endpoint endpoint(addr, port);
    auto results = asio::ip::tcp::resolver::results_type::create(endpoint, domain_name, std::to_string(port));
    return results;
  } else {
    asio::io_context io_context;
    auto work_guard =
        std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(io_context.get_executor());
    net::Resolver resolver(io_context);
    if (resolver.Init() < 0) {
      LOG(WARNING) << "Resolver: Init failure";
      return {};
    }
    asio::ip::tcp::resolver::results_type results;
    resolver.AsyncResolve(domain_name, port, [&](asio::error_code ec, asio::ip::tcp::resolver::results_type _results) {
      work_guard.reset();
      if (ec) {
        LOG(WARNING) << "resolved domain name: " << domain_name << " failed due to: " << ec;
        return;
      }
      results = std::move(_results);
    });
    io_context.run();

    return results;
  }
}

static asio::error_code ListenAddress(asio::io_context& io_context, std::vector<std::unique_ptr<CliServer>> *servers,
                                      int64_t server_tag,
                                      std::string remote_host_name,
                                      std::string remote_host_sni,
                                      uint16_t remote_port,
                                      std::string remote_username,
                                      std::string remote_password,
                                      cipher_method remote_cipher,
                                      bool remote_padding_support,
                                      std::string local_host_name,
                                      uint16_t local_port,
                                      bool redir_mode) {
  if (remote_host_sni.empty()) {
    remote_host_sni = remote_host_name;
  }
  if (remote_host_sni.size() > TLSEXT_MAXLEN_host_name) {
    LOG(WARNING) << "Invalid server name or SNI: " << remote_host_sni;
    return asio::error::invalid_argument;
  }
  std::string remote_host_ips;
  if (remote_port == 0u) {
    LOG(WARNING) << "Invalid server port: " << remote_port;
    return asio::error::invalid_argument;
  }

  auto results = ResolveAddress(remote_host_name, remote_port);
  if (results.empty()) {
    return asio::error::invalid_argument;
  } else {
    std::vector<std::string> remote_ips;
    for (auto result : results) {
      if (result.endpoint().address().is_unspecified()) {
        LOG(WARNING) << "Unspecified remote address: " << remote_host_name;
        return asio::error::invalid_argument;
      }
      remote_ips.push_back(result.endpoint().address().to_string());
    }
    remote_host_ips = absl::StrJoin(remote_ips, ";");
    LOG(INFO) << "resolved server ips: " << remote_host_ips << " from " << remote_host_name;
  }

  std::vector<asio::ip::tcp::endpoint> endpoints;

  results = ResolveAddress(local_host_name, local_port);

  if (results.empty()) {
    return asio::error::invalid_argument;
  } else {
    endpoints.insert(endpoints.end(), std::begin(results), std::end(results));

    std::vector<std::string> local_ips;
    for (auto result : results) {
      local_ips.push_back(result.endpoint().address().to_string());
    }
    LOG(INFO) << "resolved local ips: " << absl::StrJoin(local_ips, ";") << " from " << local_host_name;
  }

  asio::error_code ec;
  auto server = std::make_unique<CliServer>(io_context, server_tag, remote_host_ips, remote_host_sni, remote_port,
                                            remote_username, remote_password, remote_cipher, remote_padding_support);
  for (auto& endpoint : endpoints) {
    server->listen(endpoint, {}, {}, {}, {}, {}, redir_mode, SOMAXCONN, ec);
    if (ec) {
      LOG(ERROR) << "tag " << server_tag << " listen failed due to: " << ec;
      return ec;
    }
    endpoint = server->endpoint();
    LOG(WARNING) << "tag " << server_tag << " tcp server listening at " << endpoint
                 << " with upstream sni: " << remote_host_sni << ":" << remote_port
                 << " (ip " << remote_host_ips << " )";
  }
  servers->push_back(std::move(server));
  return {};
}

static asio::error_code ListenProxyUri(asio::io_context& io_context, std::vector<std::unique_ptr<CliServer>> *servers,
                                       int64_t server_tag, std::string_view proxy_uri_str, std::string_view listen_uri_str) {
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
      LOG(WARNING) << "Invalid Proxy URL: " << proxy_uri_str;
      return asio::error::invalid_argument;
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
        LOG(WARNING) << "Invalid Proxy URL: " << proxy_uri_str << " Port is required for socks";
        return asio::error::invalid_argument;
      }
    } else {
      LOG(WARNING) << "Invalid Proxy Scheme: " << proxy_uri.scheme();
      return asio::error::invalid_argument;
    }

    GURL listen_uri(listen_uri_str);
    if (!listen_uri.is_valid() || !listen_uri.has_host() || !listen_uri.has_scheme()) {
      LOG(WARNING) << "Invalid Listen URL: " << listen_uri_str;
      return asio::error::invalid_argument;
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

    return ListenAddress(io_context, servers, server_tag, remote_host_name, remote_host_sni, remote_port,
                         remote_username, remote_password, remote_cipher, remote_padding_support,
                         local_host_name, local_port, redir_mode);
}

int main(int argc, const char* argv[]) {
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
    perror("pthread_sigmask failed");
    return -1;
  }
#endif
  SetExecutablePath(argv[0]);
  std::string exec_path;
  if (!GetExecutablePath(&exec_path)) {
    return -1;
  }

#ifdef _WIN32
  if (!EnableSecureDllLoading()) {
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

  // Major routine
  // - Read config from ss config file
  // - Listen by local address and local port
  absl::InitializeSymbolizer(exec_path.c_str());
  absl::FailureSignalHandlerOptions failure_handle_options;
  absl::InstallFailureSignalHandler(failure_handle_options);

  config::SetClientUsageMessage(exec_path);
  config::ReadConfigFileAndArguments(argc, argv);

  std::string err = config::ValidateConfig();
  if (!err.empty()) {
    LOG(WARNING) << "Failed to validate config: " << err;
    return -1;
  }
  if (config::testOnlyMode) {
    LOG(WARNING) << "Configuration Validated";
    return 0;
  }

#ifdef _WIN32
  int iResult = 0;
  WSADATA wsaData = {0};
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  CHECK_EQ(iResult, 0) << "WSAStartup failure";
#endif

  // Forcely disabling c-ares due to cli usage for android
#if defined(HAVE_C_ARES) && (BUILDFLAG(IS_ANDROID) || BUILDFLAG(IS_IOS) || BUILDFLAG(IS_OHOS))
  absl::SetFlag(&FLAGS_disable_cares, true);
#endif

  // Start Io Context
  asio::io_context io_context;
  auto work_guard =
      std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(io_context.get_executor());

  std::vector<std::unique_ptr<CliServer>> servers;
  asio::error_code ec;

  asio::signal_set signals(io_context);
  signals.add(SIGINT, ec);
  signals.add(SIGTERM, ec);
#ifdef SIGQUIT
  signals.add(SIGQUIT, ec);
#endif
#if defined(SIGUSR1)
  signals.add(SIGUSR1, ec);
#endif
  std::function<void(asio::error_code, int)> cb;
  cb = [&](asio::error_code /*ec*/, int signal_number) {
#if defined(SIGUSR1)
    if (signal_number == SIGUSR1) {
      PrintMallocStats();
      PrintCliStats();
      signals.async_wait(cb);
      return;
    }
#endif
#ifdef SIGQUIT
    if (signal_number == SIGQUIT) {
      LOG(WARNING) << "Application shuting down";
      for (auto& server : servers)
        server->shutdown();
    } else {
#endif
      LOG(WARNING) << "Application exiting";
      for (auto& server : servers)
        server->stop();
#ifdef SIGQUIT
    }
#endif
    work_guard.reset();
    signals.clear();
  };
  signals.async_wait(cb);

  // listen
  const std::vector<std::string>& proxy_uri_strs = absl::GetFlag(FLAGS_proxy).str_array;
  const std::vector<std::string>& listen_uri_strs = absl::GetFlag(FLAGS_listen).str_array;
  if (!proxy_uri_strs.empty() && !listen_uri_strs.empty()) {
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
    if (proxy_uri_strs.size() != listen_uri_strs.size()) {
      LOG(WARNING) << "Listen addresses do not match multiple proxy addresses";
      return -1;
    }
    for (unsigned i = 0; i < proxy_uri_strs.size(); ++i) {
      ec = ListenProxyUri(io_context, &servers, i, proxy_uri_strs[i], listen_uri_strs[i]);
      if (ec) {
        return -1;
      }
    }
  } else if (proxy_uri_strs.empty() ^ listen_uri_strs.empty()) {
    LOG(WARNING) << "Both of Listen URLs and Proxy URLs are required. Ignored now";
  } else {
    std::string remote_host_name = absl::GetFlag(FLAGS_server_host);
    std::string remote_host_sni = absl::GetFlag(FLAGS_server_sni);
    uint16_t remote_port = absl::GetFlag(FLAGS_server_port);
    std::string remote_username = absl::GetFlag(FLAGS_username);
    std::string remote_password = absl::GetFlag(FLAGS_password);
    cipher_method remote_cipher = absl::GetFlag(FLAGS_method);
    bool remote_padding_support = absl::GetFlag(FLAGS_padding_support);
    std::string local_host_name = absl::GetFlag(FLAGS_local_host);
    uint16_t local_port = absl::GetFlag(FLAGS_local_port);
    bool redir_mode = absl::GetFlag(FLAGS_redir_mode);

    ec = ListenAddress(io_context, &servers, 0, remote_host_name, remote_host_sni, remote_port,
                       remote_username, remote_password, remote_cipher, remote_padding_support,
                       local_host_name, local_port, redir_mode);
    if (ec) {
      LOG(WARNING) << ec;
      return -1;
    }
  }

  io_context.run();

  servers.clear();

  PrintMallocStats();
  PrintCliStats();

  return 0;
}
