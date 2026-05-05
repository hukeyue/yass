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

#ifndef H_NET_CONTENT_SERVER
#define H_NET_CONTENT_SERVER

#ifdef HAVE_TBB
#include <tbb/concurrent_hash_map.h>
#else
#include <absl/container/flat_hash_map.h>
#endif
#include <absl/flags/flag.h>
#include <absl/strings/str_cat.h>
#include <algorithm>
#include <array>
#include <format>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <base/memory/scoped_refptr.h>

#include "config/config_tls.hpp"
#include "core/logging.hpp"
#include "core/utils.hpp"
#include "crypto/crypter_export.hpp"
#include "net/asio.hpp"
#include "net/client_connection_config.hpp"
#include "net/connection.hpp"
#include "net/network.hpp"
#include "net/protocol.hpp"
#include "net/server_connection_config.hpp"
#include "net/ssl_client_session_cache.hpp"
#include "net/ssl_socket.hpp"
#include "net/x509_util.hpp"

#define MAX_LISTEN_ADDRESSES 30

namespace net {

/// An interface used to provide service
template <typename T>
class ContentServer {
 public:
  using ConnectionType = typename T::ConnectionType;
  using tlsext_ctx_t = typename ConnectionType::tlsext_ctx_t;
  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void OnConnect(int connection_id) = 0;
    virtual void OnDisconnect(int connection_id) = 0;
  };

 public:
  explicit ContentServer(asio::io_context& io_context,
                         int64_t server_tag,
                         std::string_view remote_host_ips = {},
                         std::string_view remote_host_sni = {},
                         uint16_t remote_port = {},
                         std::string_view remote_username = {},
                         std::string_view remote_password = {},
                         cipher_method remote_cipher = {},
                         bool remote_padding_support = {},
                         std::string_view upstream_certificate = {},
                         std::string_view certificate = {},
                         std::string_view private_key = {},
                         ContentServer::Delegate* delegate = nullptr)
      : io_context_(io_context),
        work_guard_(
            std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(io_context_.get_executor())),
        server_tag_(server_tag),
        remote_config_(),
        renego_allowed_for_http11_proto_(true),
        enable_upstream_tls_(CIPHER_METHOD_IS_TLS(remote_cipher)),
        enable_tls_(true),
        upstream_certificate_(upstream_certificate),
        upstream_ssl_config_(),
        certificate_(certificate),
        private_key_(private_key),
        delegate_(delegate),
        wqthread_count_(std::thread::hardware_concurrency()) {
    remote_config_.host_ips = remote_host_ips;
    remote_config_.host_sni = remote_host_sni;
    remote_config_.port = remote_port;
    remote_config_.username = remote_username;
    remote_config_.password = remote_password;
    remote_config_.cipher = remote_cipher;
    remote_config_.padding_support = remote_padding_support;
    upstream_ssl_config_.renego_allowed_default = CIPHER_METHOD_IS_TLS(remote_config_.cipher);
    upstream_ssl_config_.renego_allowed_default &= T::Type == CONNECTION_FACTORY_CLIENT;
    upstream_ssl_config_.early_data_enabled = absl::GetFlag(FLAGS_tls13_early_data);
    renego_allowed_for_http11_proto_ &= T::Type == CONNECTION_FACTORY_SERVER;
    enable_upstream_tls_ &= T::Type == CONNECTION_FACTORY_CLIENT;
    enable_tls_ &= T::Type == CONNECTION_FACTORY_SERVER;
    DCHECK_LE(remote_config_.host_sni.size(), (unsigned int)TLSEXT_MAXLEN_host_name);

    StartWQThreads();
    VLOG(1) << "ContentServer (" << T::Name << ") " << "Tag " << server_tag_ << " allocated memory";
  }

  ~ContentServer() {
    VLOG(1) << "ContentServer (" << T::Name << ") " << "Tag " << server_tag_ << " freed memory";

    CHECK_EQ(pending_next_listen_ctxes_.size(), 0u) << "ContentServer freed on pending listen ctx";
#ifndef HAVE_TBB
    CHECK_EQ(opened_connections_, 0u) << "ContentServer freed on non-closed connections";
#endif
    CHECK_EQ(connection_map_.size(), 0u) << "ContentServer freed on non-closed connections";

    work_guard_.reset();
    CancelWQThreads();
    JoinWQThreads();
  }

  ContentServer(const ContentServer&) = delete;
  ContentServer& operator=(const ContentServer&) = delete;

  void StartWQThreads() {
#ifdef HAVE_TBB
    wqthreads_.reserve(wqthread_count_);
    for(int i = 0; i < wqthread_count_; ++i) {
      wqthreads_.emplace_back(std::make_unique<WQThreadCtx>(server_tag_, i));
    }
#endif
  }

  void CancelWQThreads() {
#ifdef HAVE_TBB
    for(int i = 0; i < wqthread_count_; ++i) {
      wqthreads_[i]->Cancel();
    }
#endif
  }

  void JoinWQThreads() {
#ifdef HAVE_TBB
    for(int i = 0; i < wqthread_count_; ++i) {
      wqthreads_[i]->Join();
    }
#endif
  }

  // Retrieve last local endpoint
  const asio::ip::tcp::endpoint& endpoint() const {
    DCHECK_NE(next_listen_ctx_, 0) << "Server should listen to some address";
    return listen_ctxs_[next_listen_ctx_ - 1].endpoint;
  }

  void listen(const asio::ip::tcp::endpoint& endpoint,
              std::string_view server_name,
              std::string_view server_username,
              std::string_view server_password,
              cipher_method server_cipher,
              bool server_padding_support,
              bool server_redir_mode,
              int backlog,
              asio::error_code& ec) {
    if (next_listen_ctx_ >= MAX_LISTEN_ADDRESSES) {
      ec = asio::error::already_started;
      return;
    }
    if (server_name.size() > TLSEXT_MAXLEN_host_name) {
      ec = asio::error::invalid_argument;
      return;
    }
    ListenCtx& ctx = listen_ctxs_[next_listen_ctx_];
    ctx.server_config.server_tag = server_tag_;
    ctx.server_config.server_name = server_name;
    ctx.server_config.username = server_username;
    ctx.server_config.password = server_password;
    ctx.server_config.cipher = server_cipher;
    ctx.server_config.padding_support = server_padding_support;
    ctx.server_config.redir_mode = server_redir_mode;
    ctx.enable_tls = enable_tls_ && CIPHER_METHOD_IS_TLS(server_cipher);
    ctx.renego_allowed_for_http11_proto = renego_allowed_for_http11_proto_ && CIPHER_METHOD_IS_TLS(server_cipher);
    ctx.endpoint = endpoint;
    ctx.acceptor = std::make_unique<asio::ip::tcp::acceptor>(io_context_);

    ctx.acceptor->open(endpoint.protocol(), ec);
    if (ec) {
      return;
    }
    if (absl::GetFlag(FLAGS_reuse_port)) {
      ctx.acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(true), ec);
      SetSOReusePort(ctx.acceptor->native_handle(), ec);
    }
    if (ec) {
      return;
    }
    SetTCPFastOpen(ctx.acceptor->native_handle(), ec);
    if (ec) {
      return;
    }
    ctx.acceptor->bind(endpoint, ec);
    if (ec) {
      return;
    }
    ctx.acceptor->listen(backlog, ec);
    if (ec) {
      return;
    }
    ctx.endpoint = ctx.acceptor->local_endpoint(ec);
    if (ec) {
      return;
    }
    if (enable_upstream_tls_) {
      setup_upstream_ssl_ctx(ec);
      if (ec) {
        return;
      }
    }
    if (ctx.enable_tls) {
      ctx.ssl_ctx = setup_ssl_ctx(ec);
      if (ec) {
        return;
      }
    }
    LOG(INFO) << "Listening (" << T::Name << ") " << "Tag " << server_tag_ << " on " << ctx.endpoint;
    int listen_ctx_num = next_listen_ctx_++;
    asio::post(io_context_, [this, listen_ctx_num]() { accept(listen_ctx_num); });
  }

  // Allow called from different threads
  void shutdown() {
    asio::post(io_context_, [this]() {
      for (int i = 0; i < next_listen_ctx_; ++i) {
        ListenCtx& ctx = listen_ctxs_[i];
        if (ctx.acceptor) {
          asio::error_code ec;
          ctx.acceptor->close(ec);
          ctx.acceptor.reset();
          if (ec) {
            LOG(WARNING) << "Connections (" << T::Name << ") " << "Tag " << server_tag_ << " acceptor (" << ctx.endpoint
                         << ") close failed: " << ec;
          }
        }
      }

      pending_next_listen_ctxes_.clear();

      if (connection_map_.empty()) {
        LOG(WARNING) << "No more connections alive... ready to stop";
        work_guard_.reset();
        in_shutdown_ = false;
      } else {
        LOG(WARNING) << "Waiting for remaining connects: " << connection_map_.size();
        in_shutdown_ = true;
      }

      CancelWQThreads();
    });
  }
  // Allow called from different threads
  void stop() {
    asio::post(io_context_, [this]() {
      for (int i = 0; i < next_listen_ctx_; ++i) {
        ListenCtx& ctx = listen_ctxs_[i];
        if (ctx.acceptor) {
          asio::error_code ec;
          ctx.acceptor->close(ec);
          ctx.acceptor.reset();
          if (ec) {
            LOG(WARNING) << "Connections (" << T::Name << ") " << "Tag " << server_tag_ << " acceptor (" << ctx.endpoint
                         << ") close failed: " << ec;
          }
        }
      }

      pending_next_listen_ctxes_.clear();

      auto connection_map = std::move(connection_map_);
      // Fatal: If this log triggers, then a hash table was move-assigned to itself
      // and then used again later without being reinitialized.
      connection_map_.clear();

      opened_connections_ = 0;
      for (auto [conn_id, conn] : connection_map) {
        VLOG(1) << "Connections (" << T::Name << ") " << "Tag " << server_tag_ << " closing Connection: " << conn_id;
        conn->close();
      }

      CancelWQThreads();
      work_guard_.reset();
    });
  }

  size_t num_of_connections() const { return opened_connections_; }

 private:
  void accept(int listen_ctx_num) {
    ListenCtx& ctx = listen_ctxs_[listen_ctx_num];
    ctx.acceptor->async_accept(
        ctx.peer_endpoint, [this, listen_ctx_num](asio::error_code ec, asio::ip::tcp::socket socket) {
          // acceptor->close might return success as well
          ListenCtx& ctx = listen_ctxs_[listen_ctx_num];
          if (!ctx.acceptor) {
            return;
          }
          // cancelled
          if (ec == asio::error::operation_aborted) {
            return;
          }
          if (ec) {
            LOG(WARNING) << "Acceptor (" << T::Name << ") " << "Tag " << server_tag_
                         << " failed to accept more due to: " << ec;
            work_guard_.reset();
            return;
          }
          int connection_id = next_connection_id_++;
          on_async_accept(listen_ctx_num, connection_id, std::move(socket));
        });
  }

  void on_async_accept(int listen_ctx_num, int connection_id, asio::ip::tcp::socket&& socket) {
    ListenCtx& ctx = listen_ctxs_[listen_ctx_num];
    tlsext_ctx_t* tlsext_ctx = nullptr;
    if (ctx.enable_tls) {
      tlsext_ctx = new tlsext_ctx_t{this, connection_id, listen_ctx_num};
      setup_ssl_ctx_alpn_cb(ctx.ssl_ctx.get(), tlsext_ctx, connection_id);
      setup_ssl_ctx_tlsext_cb(ctx.ssl_ctx.get(), tlsext_ctx, connection_id);
    }
#ifdef HAVE_TBB
    asio::io_context& io_context = wqthreads_[connection_id % wqthread_count_]->io_context;
    auto protocol = socket.local_endpoint().protocol();
    auto s = socket.release(); // requires windows 8.1 or later
    socket = asio::ip::tcp::socket(io_context); // rebind to new io_context
    socket.assign(protocol, s);
#else
    asio::io_context& io_context = io_context_;
#endif
    scoped_refptr<ConnectionType> conn =
        T::Create(io_context,
                  remote_config_, ctx.server_config, upstream_ssl_config_,
                  ctx.renego_allowed_for_http11_proto, upstream_ssl_ctx_.get(), ctx.ssl_ctx.get());
    on_accept(io_context, conn, std::move(socket), listen_ctx_num, connection_id, tlsext_ctx);
    if (in_shutdown_) {
      return;
    }
    if (connection_map_.size() >= absl::GetFlag(FLAGS_parallel_max)) {
      LOG(INFO) << "Disabling accepting new connection: " << listen_ctxs_[listen_ctx_num].endpoint;
      pending_next_listen_ctxes_.push_back(listen_ctx_num);
      return;
    }
    accept(listen_ctx_num);
  }

  void on_accept(asio::io_context& io_context,
                 scoped_refptr<ConnectionType> conn,
                 asio::ip::tcp::socket&& socket,
                 int listen_ctx_num,
                 int connection_id,
                 tlsext_ctx_t* tlsext_ctx) {
    asio::error_code ec;
    ListenCtx& ctx = listen_ctxs_[listen_ctx_num];

    socket.non_blocking(true, ec);
    if constexpr (T::Type == CONNECTION_FACTORY_SERVER) {
      SetTCPCongestion(socket.native_handle(), ec);
    }
    SetTCPKeepAlive(socket.native_handle(), ec);
    SetSocketTcpNoDelay(&socket, ec);
#ifdef _WIN32
    if (!IsWindowsVersionBNOrGreater(10, 0, 14393)) {
      asio::socket_base::send_buffer_size send_option(64 * 1024);
      socket.set_option(send_option, ec);
      asio::socket_base::receive_buffer_size recv_option(64 * 1024);
      socket.set_option(recv_option, ec);
    }
#endif

    conn->on_accept(std::move(socket), ctx.endpoint, ctx.peer_endpoint, connection_id, tlsext_ctx,
                    ssl_socket_data_index_, ssl_client_session_cache_.get());
    conn->set_disconnect_cb([this, conn]() mutable { on_disconnect(conn); });
    connection_map_.insert(std::make_pair(connection_id, conn));
    ++opened_connections_;
#ifndef HAVE_TBB
    DCHECK_EQ(connection_map_.size(), opened_connections_);
#endif
    if (delegate_) {
      delegate_->OnConnect(connection_id);
    }
    VLOG(1) << "Connection (" << T::Name << ") " << "Tag " << server_tag_ << " Id " << connection_id << " with "
            << conn->peer_endpoint() << " connected";
    asio::post(io_context, [conn]() { conn->start(); });
  }

  void on_disconnect(scoped_refptr<ConnectionType> conn) {
    int connection_id = conn->connection_id();
    VLOG(1) << "Connection (" << T::Name << ") " << "Tag " << server_tag_ << " Id " << connection_id
            << " disconnected (has ref " << std::boolalpha << conn->HasAtLeastOneRef() << std::noboolalpha << ")";
#ifdef HAVE_TBB
    typename ConnectionMapType::accessor a;
    bool found = connection_map_.find(a, connection_id);
    if (found) {
      connection_map_.erase(a);
      --opened_connections_;
    }
    a.release();
#else
    auto iter = connection_map_.find(connection_id);
    if (iter != connection_map_.end()) {
      connection_map_.erase(iter);
      --opened_connections_;
      DCHECK_EQ(connection_map_.size(), (size_t)opened_connections_);
    }
#endif
    if (delegate_) {
      delegate_->OnDisconnect(connection_id);
    }
    // reset guard to quit io loop if in shutdown
    if (in_shutdown_) {
      pending_next_listen_ctxes_.clear();
      if (connection_map_.empty()) {
        LOG(WARNING) << "No more connections alive... ready to stop";
        work_guard_.reset();
        in_shutdown_ = false;
      } else {
        LOG(WARNING) << "Waiting for remaining connects: " << connection_map_.size();
      }
    }
    auto listen_ctxes = std::move(pending_next_listen_ctxes_);
    for (int listen_ctx_num : listen_ctxes) {
      LOG(INFO) << "Resuming accepting new connection: " << listen_ctxs_[listen_ctx_num].endpoint;
      accept(listen_ctx_num);
    }
  }

  [[nodiscard]]
  bssl::UniquePtr<SSL_CTX> setup_ssl_ctx(asio::error_code& ec) {
    bssl::UniquePtr<SSL_CTX> ssl_ctx{::SSL_CTX_new(::TLS_server_method())};
    SSL_CTX* ctx = ssl_ctx.get();
    if (!ctx) {
      print_openssl_error();
      ec = asio::error::no_memory;
      return nullptr;
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, ::SSL_CTX_get_verify_callback(ctx));

    SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_SERVER);

    // Load Certificate Chain Files
    if (private_key_.empty()) {
      private_key_ = g_private_key_content;
      certificate_ = g_certificate_chain_content;
    }

    // Load Certificates (if set)
    if (!private_key_.empty()) {
      CHECK(!certificate_.empty()) << "certificate buffer is not provided";

      bssl::UniquePtr<BIO> bio(BIO_new_mem_buf(certificate_.data(), certificate_.size()));

      static pem_password_cb* callback = [](char* buf, int size, int rwflag, void* userdata) -> int {
        std::string password = absl::GetFlag(FLAGS_private_key_password);
        /* not enough buffer size */
        if (size < (int)password.size()) {
          return -1;
        }
        /* empty password */
        if (password.empty()) {
          return 0;
        }
        memcpy(buf, password.c_str(), password.size());
        return password.size();
      };
      static void* cb_userdata = nullptr;
      bssl::UniquePtr<X509> cert(PEM_read_bio_X509_AUX(bio.get(), nullptr, callback, cb_userdata));
      if (!cert) {
        print_openssl_error();
        ec = asio::error::bad_descriptor;
        return nullptr;
      }

      ERR_clear_error();
      int result = SSL_CTX_use_certificate(ctx, cert.get());
      if (result == 0 || ::ERR_peek_error() != 0) {
        print_openssl_error();
        ec = asio::error::bad_descriptor;
        return nullptr;
      }

      VLOG(1) << "Using certificate (in-memory)";
      bio.reset(BIO_new_mem_buf(private_key_.data(), private_key_.size()));
      bssl::UniquePtr<EVP_PKEY> pkey(PEM_read_bio_PrivateKey(bio.get(), nullptr, callback, cb_userdata));

      if (SSL_CTX_use_PrivateKey(ctx, pkey.get()) != 1) {
        print_openssl_error();
        ec = asio::error::bad_descriptor;
        return nullptr;
      }
      VLOG(1) << "Using privated key (in-memory)";
    }
    SSL_CTX_set_early_data_enabled(ctx, absl::GetFlag(FLAGS_tls13_early_data));

    CHECK(SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION));
    CHECK(SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION));

    // OpenSSL defaults some options to on, others to off. To avoid ambiguity,
    // set everything we care about to an absolute value.
    SslSetClearMask options;
    options.ConfigureFlag(SSL_OP_NO_COMPRESSION, true);
    options.ConfigureFlag(SSL_OP_ALL, true);

    SSL_CTX_set_options(ctx, options.set_mask);
    SSL_CTX_clear_options(ctx, options.clear_mask);

    // Same as above, this time for the SSL mode.
    SslSetClearMask mode;

    mode.ConfigureFlag(SSL_MODE_RELEASE_BUFFERS, true);

    SSL_CTX_set_mode(ctx, mode.set_mask);
    SSL_CTX_clear_mode(ctx, mode.clear_mask);

    // Use BoringSSL defaults, but disable 3DES and HMAC-SHA1 ciphers in ECDSA.
    // These are the remaining CBC-mode ECDSA ciphers.
    std::string command(kSSLDefaultCiphersList);

#if 0
    // SSLPrivateKey only supports ECDHE-based ciphers because it lacks decrypt.
    if (ssl_server_config_.require_ecdhe || (!pkey_ && private_key_))
      command.append(":!kRSA");

    // Remove any disabled ciphers.
    for (uint16_t id : ssl_server_config_.disabled_cipher_suites) {
      const SSL_CIPHER* cipher = SSL_get_cipher_by_value(id);
      if (cipher) {
        command.append(":!");
        command.append(SSL_CIPHER_get_name(cipher));
      }
    }
#endif

    CHECK(SSL_CTX_set_strict_cipher_list(ctx, command.c_str()));

    // TODO: implement these SSL options
    // SSL_CTX_set_ocsp_response
    // SSL_CTX_set_signed_cert_timestamp_list
    // SSL_CTX_set1_ech_keys

    uint8_t session_ctx_id = 0;
    SSL_CTX_set_session_id_context(ctx, &session_ctx_id, sizeof(session_ctx_id));
    // Deduplicate all certificates minted from the SSL_CTX in memory.
    SSL_CTX_set0_buffer_pool(ctx, x509_util::GetBufferPool());

    load_ca_to_ssl_ctx(ctx);

    return ssl_ctx;
  }

  void setup_ssl_ctx_alpn_cb(SSL_CTX* ctx, tlsext_ctx_t* tlsext_ctx, int connection_id) {
    SSL_CTX_set_alpn_select_cb(ctx, &ContentServer::on_alpn_select, tlsext_ctx);
    auto listen_ctx_num = tlsext_ctx->listen_ctx_num;
    auto renego_allowed_for_http11_proto = listen_ctxs_[listen_ctx_num].renego_allowed_for_http11_proto;
    auto cipher = listen_ctxs_[listen_ctx_num].server_config.cipher;
    std::string protos;
    if (CIPHER_METHOD_IS_HTTP2(cipher)) {
      if (renego_allowed_for_http11_proto) {
        protos = absl::StrCat(NextProtoToString(kProtoHTTP2), " ", NextProtoToString(kProtoHTTP11));
      } else {
        protos = NextProtoToString(kProtoHTTP2);
      }
    } else if (CIPHER_METHOD_IS_HTTPS(cipher)) {
      protos = NextProtoToString(kProtoHTTP11);
    } else {
      DLOG(FATAL) << "Alpn: Unexpected cipher: " << to_cipher_method_name(cipher);
    }
    VLOG(1) << "Alpn support (server) enabled for connection " << connection_id << " : " << protos;
  }

  [[nodiscard]]
  static int on_alpn_select(SSL* ssl,
                            const unsigned char** out,
                            unsigned char* outlen,
                            const unsigned char* in,
                            unsigned int inlen,
                            void* arg) {
    auto tlsext_ctx = reinterpret_cast<tlsext_ctx_t*>(arg);
    auto server = reinterpret_cast<ContentServer*>(tlsext_ctx->server);
    auto server_tag = server->server_tag_;
    auto listen_ctx_num = tlsext_ctx->listen_ctx_num;
    auto renego_allowed_for_http11_proto = server->listen_ctxs_[listen_ctx_num].renego_allowed_for_http11_proto;
    int connection_id = tlsext_ctx->connection_id;
    while (inlen) {
      if (in[0] + 1u > inlen) {
        goto err;
      }
      auto alpn = std::string_view(reinterpret_cast<const char*>(in + 1), in[0]);
      NextProto proto = NextProtoFromString(alpn);
      if (server->on_alpn_select(connection_id, proto)) {
        VLOG(1) << "Connection (" << T::Name << ") " << "Tag " << server_tag << " Id " << connection_id
                << " Alpn support (server) chosen: " << alpn;

        *out = in + 1;
        *outlen = in[0];

        // Enable ALPS for give proto with empty data.
        std::vector<uint8_t> data;
        SSL_add_application_settings(ssl, reinterpret_cast<const uint8_t*>(alpn.data()), alpn.size(), data.data(),
                                     data.size());
        return SSL_TLSEXT_ERR_OK;
      }

      VLOG(2) << "Connection (" << T::Name << ") " << "Tag " << server_tag << " Id " << connection_id
              << " Alpn support (server) skipped: " << alpn;
      inlen -= 1u + in[0];
      in += 1u + in[0];
    }

  err:
    LOG(WARNING) << "Connection (" << T::Name << ") " << "Tag " << server_tag << " Id " << connection_id
                 << " fatal error due to unexpected alpn protos";
    return SSL_TLSEXT_ERR_ALERT_FATAL;
  }

  void setup_ssl_ctx_tlsext_cb(SSL_CTX* ctx, tlsext_ctx_t* tlsext_ctx, int connection_id) {
    SSL_CTX_set_tlsext_servername_callback(ctx, &ContentServer::on_tlsext);
    SSL_CTX_set_tlsext_servername_arg(ctx, tlsext_ctx);

    VLOG(1) << "TLSEXT: Servername (server) enabled for connection " << connection_id
            << " server_name: " << listen_ctxs_[tlsext_ctx->listen_ctx_num].server_config.server_name;
  }

  [[nodiscard]]
  static int on_tlsext(SSL* ssl, int* al, void* arg) {
    auto tlsext_ctx = reinterpret_cast<tlsext_ctx_t*>(arg);
    auto server = reinterpret_cast<ContentServer*>(tlsext_ctx->server);
    auto server_tag = server->server_tag_;
    int connection_id = tlsext_ctx->connection_id;
    int listen_ctx_num = tlsext_ctx->listen_ctx_num;
    std::string_view expected_server_name = server->listen_ctxs_[listen_ctx_num].server_config.server_name;

    // SNI must be accessible from the SNI callback.
    const char* server_name_ptr = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);
    std::string_view server_name;
    if (server_name_ptr) {
      server_name = server_name_ptr;
    }
    // Allow once if matched
    if (server_name == expected_server_name) {
      return SSL_TLSEXT_ERR_OK;
    }

    VLOG(1) << "Connection (" << T::Name << ") " << "Tag " << server_tag << " Id " << connection_id
            << " TLSEXT: Servername mismatch "
            << "(got " << server_name << "; want " << expected_server_name << ").";
    return SSL_TLSEXT_ERR_ALERT_FATAL;
  }

  [[nodiscard]]
  bool on_alpn_select(int connection_id, NextProto proto) {
#ifdef HAVE_TBB
    typename ConnectionMapType::accessor a;
    bool found = connection_map_.find(a, connection_id);
    if (found) {
      return a->second->on_alpn_select(proto);
#else
    auto iter = connection_map_.find(connection_id);
    if (iter != connection_map_.end()) {
      return iter->second->on_alpn_select(proto);
#endif
    } else {
      LOG(INFO) << "Connection (" << T::Name << ") " << "Tag " << server_tag_
                << " invalid connection id: " << connection_id;
    }
    return false;
  }

  void setup_upstream_ssl_ctx(asio::error_code& ec) {
    upstream_ssl_ctx_.reset(::SSL_CTX_new(::TLS_client_method()));
    SSL_CTX* ctx = upstream_ssl_ctx_.get();
    if (!ctx) {
      print_openssl_error();
      ec = asio::error::no_memory;
      return;
    }

    SslSetClearMask options;
    options.ConfigureFlag(SSL_OP_ALL, true);

    SSL_CTX_set_options(ctx, options.set_mask);
    SSL_CTX_clear_options(ctx, options.clear_mask);

    CHECK(SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION));
    CHECK(SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION));

    if (absl::GetFlag(FLAGS_insecure_mode)) {
      SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, ::SSL_CTX_get_verify_callback(ctx));
    } else {
      SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, ::SSL_CTX_get_verify_callback(ctx));
      // FIXME
      // reverify only supported on custom verify callback
      // SSL_CTX_set_reverify_on_resume(ctx, 1);
    }
    if (ec) {
      return;
    }

    // priorize h2 before http/1.1
    //
    // don't do it at SSLSocket::SSLSocket as we don't know the context (until we copy the protos array)
    NextProtoVector alpn_protos;
    std::string protos;
    if (CIPHER_METHOD_IS_HTTP2(remote_config_.cipher)) {
      if (upstream_ssl_config_.renego_allowed_default) {
        protos = absl::StrCat(NextProtoToString(kProtoHTTP2), " ", NextProtoToString(kProtoHTTP11));
        alpn_protos = {kProtoHTTP2, kProtoHTTP11};
        upstream_ssl_config_.renego_allowed_for_protos = {kProtoHTTP11};
      } else {
        // ??? BoringSSL bugs, http1.1 will always be enabled
        protos = NextProtoToString(kProtoHTTP2);
        alpn_protos = {kProtoHTTP2};
      }
    } else if (CIPHER_METHOD_IS_HTTPS(remote_config_.cipher)) {
      protos = NextProtoToString(kProtoHTTP11);
      alpn_protos = {kProtoHTTP11};
    } else {
      DLOG(FATAL) << "Alpn: Unexpected remote cipher: " << to_cipher_method_name(remote_config_.cipher);
    }
    VLOG(1) << "Alpn support (client) enabled: " << protos;
    upstream_ssl_config_.alpn_protos = std::move(alpn_protos);

    if (upstream_certificate_.empty()) {
      upstream_certificate_ = g_certificate_chain_content;
    }

    const auto& cert = upstream_certificate_;
    if (!cert.empty()) {
      if (ec) {
        return;
      }

      bssl::UniquePtr<BIO> bio(BIO_new_mem_buf(cert.data(), cert.size()));

      bssl::UniquePtr<X509> cert(PEM_read_bio_X509(bio.get(), nullptr, 0, nullptr));
      if (!cert) {
        print_openssl_error();
        ec = asio::error::bad_descriptor;
        return;
      }
      X509_STORE* store = SSL_CTX_get_cert_store(ctx);
      if (!store) {
        print_openssl_error();
        ec = asio::error::no_memory;
        return;
      }

      ERR_clear_error();

      if (X509_STORE_add_cert(store, cert.get()) != 1) {
        print_openssl_error();
        ec = asio::error::bad_descriptor;
        return;
      }

      VLOG(1) << "Using upstream certificate (in-memory)";
    }

    std::call_once(boringssl_data_index_init_flag_, [&] {
      ssl_ctx_data_index_ = SSL_CTX_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);
      ssl_socket_data_index_ = SSL_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);
    });
    SSL_CTX_set_ex_data(ctx, ssl_ctx_data_index_, this);
    ssl_client_session_cache_ = std::make_unique<SSLClientSessionCache>(SSLClientSessionCache::Config{});
#if defined(TBB_PREVIEW_CONCURRENT_LRU_CACHE) || !defined(HAVE_TBB)
    // Disable the internal session cache. Session caching is handled
    // externally (i.e. by SSLClientSessionCache).
    SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_CLIENT | SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(ctx, NewSessionCallback);
#endif

    SSL_CTX_set_timeout(ctx, 1 * 60 * 60 /* one hour */);

    SSL_CTX_set_grease_enabled(ctx, 1);

    // Deduplicate all certificates minted from the SSL_CTX in memory.
    SSL_CTX_set0_buffer_pool(ctx, x509_util::GetBufferPool());

    load_ca_to_ssl_ctx(ctx);
  }

 private:
  static std::once_flag boringssl_data_index_init_flag_;
  static int ssl_ctx_data_index_;
  static int ssl_socket_data_index_;

  static SSLSocket* GetClientSocketFromSSL(const SSL* ssl) {
    DCHECK(ssl);
    SSLSocket* socket = static_cast<SSLSocket*>(SSL_get_ex_data(ssl, ssl_socket_data_index_));
    DCHECK(socket);
    return socket;
  }

  static int NewSessionCallback(SSL* ssl, SSL_SESSION* session) {
    SSLSocket* socket = GetClientSocketFromSSL(ssl);
    return socket->NewSessionCallback(session);
  }

 private:
  asio::io_context& io_context_;
  /// stopping the io_context from running out of work
  std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> work_guard_;

  /// mark of server
  int64_t server_tag_;

  ClientConnectionConfig remote_config_;

  bool renego_allowed_for_http11_proto_;
  bool enable_upstream_tls_;
  bool enable_tls_;
  char _reserved0_[5];
  std::string upstream_certificate_;
  bssl::UniquePtr<SSL_CTX> upstream_ssl_ctx_;
  std::unique_ptr<SSLClientSessionCache> ssl_client_session_cache_;
  SSLConfig upstream_ssl_config_;

  std::string certificate_;
  std::string private_key_;

  ContentServer::Delegate* delegate_;

  struct ListenCtx {
    ServerConnectionConfig server_config;
    bool enable_tls;
    bool renego_allowed_for_http11_proto;
    char _reserved0[6];
    bssl::UniquePtr<SSL_CTX> ssl_ctx;
    asio::ip::tcp::endpoint endpoint;
    asio::ip::tcp::endpoint peer_endpoint;
    std::unique_ptr<asio::ip::tcp::acceptor> acceptor;
  };
  std::array<ListenCtx, MAX_LISTEN_ADDRESSES> listen_ctxs_;
  int next_listen_ctx_ = 0;
  std::vector<int> pending_next_listen_ctxes_;
  bool in_shutdown_ = false;

#ifdef HAVE_TBB
  typedef tbb::concurrent_hash_map<int, scoped_refptr<ConnectionType>> ConnectionMapType;
#else
  typedef absl::flat_hash_map<int, scoped_refptr<ConnectionType>> ConnectionMapType;
#endif
  ConnectionMapType connection_map_;

  std::atomic<int32_t> next_connection_id_ = 1;
  std::atomic<size_t> opened_connections_ = 0;

  const int wqthread_count_;

#ifdef HAVE_TBB
  struct WQThreadCtx {
    int tid;
    int reserved;
    asio::io_context io_context;
    std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> work_guard_;
    std::thread t;
    WQThreadCtx(int tag, int thread_id)
      : tid(thread_id),
        work_guard_(std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(io_context.get_executor())),
        t([this, tag, thread_id]{
      std::string tname = std::format("wq-{}-{}-{}", tag, thread_id, T::Name);
      if (!SetCurrentThreadName(tname)) {
        PLOG(WARNING) << "wqthread: failed to set thread name: " << tname;
      }
      if (!SetCurrentThreadPriority(ThreadPriority::ABOVE_NORMAL)) {
        PLOG(WARNING) << "wqthread: failed to set thread priority";
      }

      LOG(INFO) << "wqthread: " << tname << " started";
      io_context.run();
      LOG(INFO) << "wqthread: " << tname << " stopped";
    }) {}
    WQThreadCtx(const WQThreadCtx&) = delete;
    WQThreadCtx& operator=(const WQThreadCtx&) = delete;
    WQThreadCtx(WQThreadCtx&&) = default;
    WQThreadCtx& operator=(WQThreadCtx&&) = default;
    void Cancel() {
      work_guard_.reset();
    }
    void Join() {
      t.join();
    }
  };
  std::vector<std::unique_ptr<WQThreadCtx>> wqthreads_;
#endif
};

template <typename T>
std::once_flag ContentServer<T>::boringssl_data_index_init_flag_ = {};

template <typename T>
int ContentServer<T>::ssl_ctx_data_index_ = -1;

template <typename T>
int ContentServer<T>::ssl_socket_data_index_ = -1;

}  // namespace net

#endif  // H_NET_CONTENT_SERVER
