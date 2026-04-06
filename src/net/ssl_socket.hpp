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

/* Copyright (c) 2023-2026 Chilledheart  */

#ifndef H_NET_SSL_SOCKET
#define H_NET_SSL_SOCKET

#include <absl/functional/any_invocable.h>
#include <base/memory/ref_counted.h>
#include <base/memory/scoped_refptr.h>
#include <string_view>
#include "third_party/boringssl/src/include/openssl/ssl.h"

#include "net/asio.hpp"
#include "net/io_buffer.hpp"
#include "net/net_errors.hpp"
#include "net/openssl_util.hpp"
#include "net/protocol.hpp"
#include "net/ssl_client_session_cache.hpp"
#include "net/ssl_config.hpp"

namespace net {

class SSLSocket : public gurl_base::RefCountedThreadSafe<SSLSocket> {
 public:
  SSLSocket(int ssl_socket_data_index,
            SSLClientSessionCache* ssl_client_session_cache,
            asio::io_context* io_context,
            asio::ip::tcp::socket* socket,
            SSL_CTX* ssl_ctx,
            const SSLConfig& ssl_config,
            const std::string& host_name,
            int port);
  ~SSLSocket();

  SSLSocket(SSLSocket&&) = delete;
  SSLSocket& operator=(SSLSocket&&) = delete;

  template <typename... Args>
  static scoped_refptr<SSLSocket> Create(Args&&... args) {
    return gurl_base::MakeRefCounted<SSLSocket>(std::forward<Args>(args)...);
  }

  // StreamSocket implementation
  int Connect(CompletionOnceCallback callback);
  void Disconnect();
  void ConfirmHandshake(CompletionOnceCallback callback);
  int Shutdown(WaitCallback&& cb, bool force = false);

  SSL* native_handle() { return ssl_.get(); }

  // Socket implementation.
  size_t Read(GrowableIOBuffer* buf, asio::error_code& ec);
  size_t Write(GrowableIOBuffer* buf, asio::error_code& ec);
  void WaitRead(WaitCallback&& cb);
  void WaitWrite(WaitCallback&& cb);

  NextProto negotiated_protocol() const { return negotiated_protocol_; }

  int NewSessionCallback(SSL_SESSION* session);

 protected:
  void OnWaitRead(asio::error_code ec);
  void OnWaitWrite(asio::error_code ec);
  void OnReadReady();
  void OnWriteReady();
  void OnDoWaitShutdown(asio::error_code ec);

 private:
  int DoHandshake(int* openssl_result);
  int DoHandshakeComplete(int result);
  void DoConnectCallback(int result);

  void OnVerifyComplete(int result);
  void OnHandshakeIOComplete(int result, int openssl_result);
  void RetryAllOperations();

  int DoHandshakeLoop(int last_io_result, int last_openssl_result);
  int DoPayloadRead(GrowableIOBuffer* buf, int buf_len);
  int DoPayloadWrite(GrowableIOBuffer* buf, int buf_len);
  void DoPeek();
  int MapLastOpenSSLError(int ssl_error);

 private:
  int ssl_socket_data_index_;
  asio::io_context* io_context_;
  asio::ip::tcp::socket* stream_socket_;

  // Returns a session cache key for this socket.
  SSLClientSessionCache::Key GetSessionCacheKey(
      std::optional<asio::ip::address> dest_ip_addr) const;

  // Returns true if renegotiations are allowed.
  bool IsRenegotiationAllowed() const;

  // Returns true when we should be using the ssl_client_session_cache_
  bool IsCachingEnabled() const;

  std::pair<std::string, int> host_and_port_;
  SSLConfig ssl_config_;
  SSLClientSessionCache* ssl_client_session_cache_;

  CompletionOnceCallback user_connect_callback_;
  WaitCallback wait_read_callback_;
  WaitCallback wait_write_callback_;
  WaitCallback wait_shutdown_callback_;

  bool first_post_handshake_write_ = true;

  // True if we've already handled the result of our attempt to use early data.
  bool handled_early_data_result_ = false;

  // Used by DoPayloadRead() when attempting to fill the caller's buffer with
  // as much data as possible without blocking.
  // If DoPayloadRead() encounters an error after having read some data, stores
  // the result to return on the *next* call to DoPayloadRead().  A value > 0
  // indicates there is no pending result, otherwise 0 indicates EOF and < 0
  // indicates an error.
  int pending_read_error_;

  // If there is a pending read result, the OpenSSL result code (output of
  // SSL_get_error) associated with it.
  int pending_read_ssl_error_ = SSL_ERROR_NONE;

  // Set when Connect finishes.
  bool completed_connect_ = false;

  // Set when Read() or Write() successfully reads or writes data to or from the
  // network.
  bool was_ever_used_ = false;

  // OpenSSL stuff
  bssl::UniquePtr<SSL> ssl_;

  enum State {
    STATE_NONE,
    STATE_HANDSHAKE,
    STATE_HANDSHAKE_COMPLETE,
  };
  State next_handshake_state_ = STATE_NONE;

  // True if we are currently confirming the handshake.
  bool in_confirm_handshake_ = false;

  // True if the post-handshake SSL_peek has completed.
  bool peek_complete_ = false;

  // True if the socket has been disconnected.
  bool disconnected_ = false;

  // True if there was a certificate error which should be treated as fatal,
  // and false otherwise.
  bool is_fatal_cert_error_ = false;

  // True if the socket should respond to client certificate requests with
  // |client_cert_| and |client_private_key_|, which may be null to continue
  // with no certificate. If false, client certificate requests will result in
  // ERR_SSL_CLIENT_AUTH_CERT_NEEDED.
  bool send_client_cert_;

  NextProto negotiated_protocol_ = kProtoUnknown;

  // True if SCTs were received via a TLS extension.
  bool signed_cert_timestamps_received_ = false;
  // True if a stapled OCSP response was received.
  bool stapled_ocsp_response_received_ = false;
};

}  // namespace net

#endif  // H_NET_SSL_SOCKET
