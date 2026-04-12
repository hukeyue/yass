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

#ifndef H_NET_CONNECTION
#define H_NET_CONNECTION

#include <deque>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "config/config.hpp"
#include "core/logging.hpp"
#include "net/asio.hpp"
#include "net/network.hpp"
#include "net/protocol.hpp"
#include "net/client_connection_config.hpp"
#include "net/server_connection_config.hpp"
#include "net/ssl_client_session_cache.hpp"
#include "net/ssl_server_socket.hpp"

#include <absl/functional/any_invocable.h>

namespace net {

class Downlink {
 public:
  using io_handle_t = absl::AnyInvocable<void(asio::error_code, std::size_t)>;
  using handle_t = absl::AnyInvocable<void(asio::error_code)>;

  Downlink(asio::io_context& io_context) : io_context_(io_context), socket_(io_context_) {}

  virtual ~Downlink() {}

  void on_accept(asio::ip::tcp::socket&& socket) { socket_ = std::move(socket); }

 public:
  virtual void handshake(handle_t&& cb) { cb(asio::error_code()); }

  virtual bool do_peek() {
    asio::error_code ec;
    if (socket_.available(ec)) {
      return true;
    }
    return false;
  }

  virtual void async_read_some(handle_t&& cb) { socket_.async_wait(asio::ip::tcp::socket::wait_read, std::move(cb)); }

  virtual size_t read_some(GrowableIOBuffer* buf, asio::error_code& ec) {
    return socket_.read_some(tail_buffer(buf), ec);
  }

  virtual void async_write_some(handle_t&& cb) { socket_.async_wait(asio::ip::tcp::socket::wait_write, std::move(cb)); }

  virtual size_t write_some(GrowableIOBuffer* buf, asio::error_code& ec) {
    return socket_.write_some(const_buffer(buf), ec);
  }

  virtual void async_shutdown(handle_t&& cb) {
    asio::error_code ec;
    socket_.shutdown(asio::ip::tcp::socket::shutdown_send, ec);
    cb(ec);
  }

  virtual void shutdown(asio::error_code& ec) { socket_.shutdown(asio::ip::tcp::socket::shutdown_send, ec); }

  [[nodiscard]]
  virtual bool on_alpn_select(NextProto proto) { LOG(INFO) << "Alpn: Unexpected call"; return false; }

  virtual void close(asio::error_code& ec) { socket_.close(ec); }

  virtual void async_wait_error(handle_t&& cb) { socket_.async_wait(asio::ip::tcp::socket::wait_error, std::move(cb)); }

 public:
  asio::io_context& io_context_;
  asio::ip::tcp::socket socket_;
  handle_t handshake_callback_;  // FIXME handle it gracefully
};

class SSLDownlink : public Downlink {
 public:
  SSLDownlink(asio::io_context& io_context, bool renego_allowed_for_http11_proto, cipher_method *local_cipher, SSL_CTX* ssl_ctx)
      : Downlink(io_context),
        renego_allowed_for_http11_proto_(renego_allowed_for_http11_proto),
        local_cipher_(local_cipher),
        ssl_socket_(SSLServerSocket::Create(&io_context, &socket_, ssl_ctx)) {}

  ~SSLDownlink() override { DCHECK(!handshake_callback_); }

  void handshake(handle_t&& cb) override {
    DCHECK(!handshake_callback_);
    handshake_callback_ = std::move(cb);
    ssl_socket_->Handshake([this](int result) {
      auto callback = std::move(handshake_callback_);
      DCHECK(!handshake_callback_);
      asio::error_code ec = result == OK ? asio::error_code() : asio::error::connection_refused;
      if (!ec) {
        auto proto = ssl_socket_->negotiated_protocol();
        DCHECK_EQ(selected_proto_, proto);
        static_cast<void>(proto);
        VLOG(1) << "Alpn selected (server): " << NextProtoToString(proto);
      }
      if (callback) {
        callback(ec);
      }
    });
  }

  bool do_peek() override {
    char byte;
    auto ssl = ssl_socket_->native_handle();
    int rv = SSL_peek(ssl, &byte, 1);
    int ssl_err = SSL_get_error(ssl, rv);
    if (ssl_err != SSL_ERROR_WANT_READ && ssl_err != SSL_ERROR_WANT_WRITE) {
      return true;
    }
    return false;
  }

  void async_read_some(handle_t&& cb) override { ssl_socket_->WaitRead(std::move(cb)); }

  size_t read_some(GrowableIOBuffer* buf, asio::error_code& ec) override { return ssl_socket_->Read(buf, ec); }

  void async_write_some(handle_t&& cb) override { ssl_socket_->WaitWrite(std::move(cb)); }

  size_t write_some(GrowableIOBuffer* buf, asio::error_code& ec) override { return ssl_socket_->Write(buf, ec); }

  void async_shutdown(handle_t&& cb) override { ssl_socket_->Shutdown(std::move(cb)); }

  void shutdown(asio::error_code& ec) override {
    ec = asio::error_code();
    ssl_socket_->Shutdown([](asio::error_code ec) {}, true);
  }

  [[nodiscard]]
  bool on_alpn_select(NextProto proto) override {
    DCHECK(CIPHER_METHOD_IS_TLS(*local_cipher_));
    switch (proto) {
      case kProtoHTTP2:
        if (CIPHER_METHOD_IS_HTTP2(*local_cipher_))
          goto alpn_selected;
        break;
      case kProtoHTTP11:
        if (CIPHER_METHOD_IS_HTTP2(*local_cipher_) && renego_allowed_for_http11_proto_)
          goto alpn_selected;
        if (CIPHER_METHOD_IS_HTTPS(*local_cipher_))
          goto alpn_selected;
        break;
      default:
        break;
    }
    // rejected
    return false;

alpn_selected:
    selected_proto_ = proto;
    if (proto == kProtoHTTP11) {
      *local_cipher_ = CRYPTO_HTTPS;
    }
    return true;
  }

  void close(asio::error_code& ec) override { ssl_socket_->Disconnect(); }

 private:
  NextProto selected_proto_ = kProtoUnknown;
  bool renego_allowed_for_http11_proto_;
  cipher_method* local_cipher_;
  scoped_refptr<SSLServerSocket> ssl_socket_;
};

class Connection {
  using io_handle_t = Downlink::io_handle_t;
  using handle_t = Downlink::handle_t;

 public:
  struct tlsext_ctx_t {
    void* server;
    int connection_id;
    int listen_ctx_num;
  };

 public:
  /// Construct the connection with io context
  ///
  /// \param io_context the io context associated with the service
  /// \param remote_config the network config used for upstream
  /// \param local_config the network config used for downstream
  /// \param upstream_ssl_config ssl config such as alpn used for upstream
  /// \param renego_allowed_for_http11_proto the data channel falls back to http1.1 (via alpn)
  /// \param upstream_ssl_ctx the ssl context object for tls data transfer (upstream)
  /// \param ssl_ctx the ssl context object for tls data transfer (downstream)
  Connection(asio::io_context& io_context,
             const ClientConnectionConfig& remote_config,
             const ServerConnectionConfig& local_config,
             const SSLConfig& upstream_ssl_config,
             bool renego_allowed_for_http11_proto,
             SSL_CTX* upstream_ssl_ctx,
             SSL_CTX* ssl_ctx)
      : io_context_(&io_context),
        remote_config_(remote_config),
        local_config_(local_config),
        remote_cipher_(remote_config.cipher),
        local_cipher_(local_config.cipher),
        upstream_ssl_config_(upstream_ssl_config),
        upstream_ssl_ctx_(upstream_ssl_ctx),
        ssl_ctx_(ssl_ctx) {
    DCHECK_LE(remote_config_.host_sni.size(), (unsigned int)TLSEXT_MAXLEN_host_name);
    if (ssl_ctx_ != nullptr) {
      downlink_ = std::make_unique<SSLDownlink>(io_context, renego_allowed_for_http11_proto, &local_cipher_, ssl_ctx_);
    } else {
      downlink_ = std::make_unique<Downlink>(io_context);
    }
  }

  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;

  Connection(Connection&&) = delete;
  Connection& operator=(Connection&&) = delete;

  virtual ~Connection() = default;

  [[nodiscard]]
  bool on_alpn_select(NextProto proto) { return downlink_->on_alpn_select(proto); }

 public:
  /// Construct the connection with socket
  ///
  /// \param socket the socket bound to the service
  /// \param endpoint the service socket's endpoint
  /// \param peer_endpoint the peer endpoint
  /// \param the number of connection id
  /// \param the pointer of tlsext ctx
  /// \param the ssl client data index
  /// \param the ssl client session cache
  void on_accept(asio::ip::tcp::socket&& socket,
                 const asio::ip::tcp::endpoint& endpoint,
                 const asio::ip::tcp::endpoint& peer_endpoint,
                 int connection_id,
                 tlsext_ctx_t* tlsext_ctx,
                 int ssl_socket_data_index,
                 SSLClientSessionCache* ssl_client_session_cache) {
    downlink_->on_accept(std::move(socket));
    endpoint_ = endpoint;
    peer_endpoint_ = peer_endpoint;
    connection_id_ = connection_id;
    tlsext_ctx_.reset(tlsext_ctx);
    ssl_socket_data_index_ = ssl_socket_data_index;
    ssl_client_session_cache_ = ssl_client_session_cache;
  }

  /// set callback
  ///
  /// \param cb the callback function pointer when disconnect happens
  void set_disconnect_cb(absl::AnyInvocable<void()>&& cb) { disconnect_cb_ = std::move(cb); }

  /// call callback
  ///
  void on_disconnect() {
    downlink_->handshake_callback_ = nullptr;
    auto cb = std::move(disconnect_cb_);
    DCHECK(!disconnect_cb_);
    if (cb) {
      cb();
    }
  }

  asio::io_context& io_context() { return *io_context_; }

  const asio::ip::tcp::endpoint& endpoint() const { return endpoint_; }

  const asio::ip::tcp::endpoint& peer_endpoint() const { return peer_endpoint_; }

  int connection_id() const { return connection_id_; }

  const tlsext_ctx_t& tlsext_ctx() const {
    DCHECK(tlsext_ctx_);
    return *tlsext_ctx_;
  }

  int ssl_socket_data_index() const { return ssl_socket_data_index_; }
  SSLClientSessionCache* ssl_client_session_cache() const { return ssl_client_session_cache_; }

 protected:
  /// the peek current io
  bool DoPeek() { return downlink_->do_peek(); }

 protected:
  /// the io context associated with
  asio::io_context* io_context_;
  /// the upstream network config
  const ClientConnectionConfig& remote_config_;
  /// the downstream network config
  const ServerConnectionConfig& local_config_;

  /// selected cipher after negotiated
  cipher_method remote_cipher_;
  /// selected cipher after negotiated
  cipher_method local_cipher_;

  /// service's bound endpoint
  asio::ip::tcp::endpoint endpoint_;
  /// the peer endpoint the connection connects
  asio::ip::tcp::endpoint peer_endpoint_;
  /// the number of connection id
  int connection_id_ = -1;
  /// the tlsext ctx
  std::unique_ptr<tlsext_ctx_t> tlsext_ctx_;
  /// the ssl client data index
  int ssl_socket_data_index_ = -1;
  /// the ssl client context cache
  SSLClientSessionCache* ssl_client_session_cache_ = nullptr;

  /// ssl config such as alpn used for upstream
  const SSLConfig& upstream_ssl_config_;
  /// ssl context for upstream
  SSL_CTX* upstream_ssl_ctx_;
  /// ssl context for downstream
  SSL_CTX* ssl_ctx_;

  std::unique_ptr<Downlink> downlink_;

 protected:
  /// statistics of read bytes
  size_t rbytes_transferred_ = 0;
  /// statistics of written bytes
  size_t wbytes_transferred_ = 0;

 private:
  /// the callback invoked when disconnect event happens
  absl::AnyInvocable<void()> disconnect_cb_;
};

enum ConnectionFactoryType {
  CONNECTION_FACTORY_UNSPEC,
  CONNECTION_FACTORY_CLIENT,
  CONNECTION_FACTORY_SERVER,
  CONNECTION_FACTORY_CONTENT_PROVIDER,
};

#ifdef __cpp_concepts
#include <concepts>
template <typename T>
concept BasicConnection = requires(T t) {
  /// require Name field
  { T::Name } -> std::convertible_to<std::string_view>;
  /// require Type field
  { T::Type } -> std::convertible_to<ConnectionFactoryType>;
};

template <typename T>
concept StartClosableConnection = BasicConnection<T> && requires(T t) {
  /// Enter the start phase, begin to read requests
  { t.start() };
  /// Close the socket and clean up
  { t.close() };
};
#else
#define StartClosableConnection typename
#endif

template <StartClosableConnection T>
class ConnectionFactory {
 public:
  using ConnectionType = T;

  template <typename... Args>
  static scoped_refptr<ConnectionType> Create(Args&&... args) {
    return gurl_base::MakeRefCounted<ConnectionType>(std::forward<Args>(args)...);
  }
  static constexpr const ConnectionFactoryType Type = ConnectionType::Type;
  static constexpr const std::string_view Name = ConnectionType::Name;
};

}  // namespace net

#endif  // H_NET_CONNECTION
