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
#include "core/utils.hpp"
#include "crypto/crypter_export.hpp"
#include "net/asio.hpp"
#include "net/padding.hpp"
#include "net/resolver.hpp"
#include "yass/feature.h"
#include "yass/version.h"

namespace config {
const ProgramType pType = YASS_CLIENT_DEFAULT;
}  // namespace config

using namespace net::cli;

int main(int argc, const char* argv[]) {
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


  // Forcely disabling c-ares due to cli usage for android
#if defined(HAVE_C_ARES) && (BUILDFLAG(IS_ANDROID) || BUILDFLAG(IS_IOS) || BUILDFLAG(IS_OHOS))
  absl::SetFlag(&FLAGS_disable_cares, true);
#endif

  // Create Client Instance
  auto instance = yass_client_instance_create();
  if (!instance) {
    LOG(WARNING) << "Failed to create client instance";
    return -1;
  }
  int ret = yass_client_instance_init(instance);
  if (ret != 0) {
    LOG(WARNING) << yass_client_instance_get_last_error_str(instance);
    yass_client_instance_destroy(instance);
    return -1;
  }

  // Start Io Context For Signal Hanlder
  asio::io_context io_context;
  auto work_guard =
      std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(io_context.get_executor());

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
      yass_client_instance_shutdown(instance);
    } else {
#endif
      LOG(WARNING) << "Application exiting";
      yass_client_instance_cancel(instance);
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
    if (proxy_uri_strs.size() != listen_uri_strs.size()) {
      LOG(WARNING) << "Listen addresses do not match multiple proxy addresses";
      return -1;
    }
    for (unsigned i = 0; i < proxy_uri_strs.size(); ++i) {
      ret = yass_client_instance_add_server_uri(instance, i, proxy_uri_strs[i].c_str(), listen_uri_strs[i].c_str(), nullptr);
      if (ret != 0) {
        LOG(WARNING) << yass_client_instance_get_last_error_str(instance);
        yass_client_instance_destroy(instance);
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

    ret = yass_client_instance_add_server(instance, 0,
                                          remote_host_name.c_str(), remote_host_sni.c_str(), remote_port,
                                          remote_username.c_str(), remote_password.c_str(),
                                          remote_cipher, remote_padding_support,
                                          local_host_name.c_str(), local_port, redir_mode, nullptr);
    if (ret) {
      LOG(WARNING) << yass_client_instance_get_last_error_str(instance);
      yass_client_instance_destroy(instance);
      return -1;
    }
  }

  std::thread signal_listener_thread([&]{
    io_context.run();
  });

  ret = yass_client_instance_run(instance);
  work_guard.reset();

  signal_listener_thread.join();

  if (ret) {
    LOG(WARNING) << yass_client_instance_get_last_error_str(instance);
    return -1;
  }

  yass_client_instance_destroy(instance);

  PrintMallocStats();
  PrintCliStats();

  return 0;
}
