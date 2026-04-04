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

#ifndef H_NET_SSL_CONFIG
#define H_NET_SSL_CONFIG

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

namespace net {

constexpr const std::string_view kSSLDefaultCiphersList = "ALL:!aPSK:!ECDSA+SHA1:!3DES";

// This enum is persisted into histograms. Values may not be renumbered.
enum class SSLHandshakeDetails {
  // TLS 1.2 (or earlier) full handshake (2-RTT)
  kTLS12Full = 0,
  // TLS 1.2 (or earlier) resumption (1-RTT)
  kTLS12Resume = 1,
  // TLS 1.2 full handshake with False Start (1-RTT)
  kTLS12FalseStart = 2,
  // 3 was previously used for TLS 1.3 full handshakes with or without HRR.
  // 4 was previously used for TLS 1.3 resumptions with or without HRR.
  // TLS 1.3 0-RTT handshake (0-RTT)
  kTLS13Early = 5,
  // TLS 1.3 full handshake without HelloRetryRequest (1-RTT)
  kTLS13Full = 6,
  // TLS 1.3 resumption handshake without HelloRetryRequest (1-RTT)
  kTLS13Resume = 7,
  // TLS 1.3 full handshake with HelloRetryRequest (2-RTT)
  kTLS13FullWithHelloRetryRequest = 8,
  // TLS 1.3 resumption handshake with HelloRetryRequest (2-RTT)
  kTLS13ResumeWithHelloRetryRequest = 9,
  kMaxValue = kTLS13ResumeWithHelloRetryRequest,
};

// This constant can be any non-negative/non-zero value (eg: it does not
// overlap with any value of the net::Error range, including net::OK).
const int kSSLClientSocketNoPendingResult = 1;

// This constant can be any non-negative/non-zero value (eg: it does not
// overlap with any value of the net::Error range, including net::OK).
const int kCertVerifyPending = 1;

// Default size of the internal BoringSSL buffers.
const int kDefaultOpenSSLBufferSize = 17 * 1024;

// A OnceCallback specialization that takes a single int parameter. Usually this
// is used to report a byte count or network error code.
using CompletionOnceCallback = absl::AnyInvocable<void(int)>;
using WaitCallback = absl::AnyInvocable<void(asio::error_code ec)>;

std::vector<uint8_t> SerializeNextProtos(const NextProtoVector& next_protos);

struct SSLConfig {
  bool allow_fallback_to_http11 = true;

  // The list of application level protocols supported with ALPN (Application
  // Layer Protocol Negotiation), in decreasing order of preference.  Protocols
  // will be advertised in this order during TLS handshake.
  NextProtoVector alpn_protos;

#if 0
  // True if renegotiation should be allowed for the default application-level
  // protocol when the peer does not negotiate ALPN.
  bool renego_allowed_default = false;

  // The list of application-level protocols to enable renegotiation for.
  NextProtoVector renego_allowed_for_protos;

  // If non-empty, a serialized ECHConfigList to use to encrypt the ClientHello.
  // If this field is non-empty, callers should handle |ERR_ECH_NOT_NEGOTIATED|
  // errors from Connect() by calling GetECHRetryConfigs() to determine how to
  // retry the connection.
  std::vector<uint8_t> ech_config_list;
#endif
};

}  // namespace net

#endif  // H_NET_SSL_CONFIG
