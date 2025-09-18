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

/* Copyright (c) 2024-2025 Chilledheart  */

#include "net/dot_request.hpp"

#include "net/dns_addrinfo_helper.hpp"
#include "net/dns_message_request.hpp"
#include "net/dns_message_response_parser.hpp"

namespace net {

using namespace dns_message;

DoTRequest::~DoTRequest() {
  VLOG(1) << "DoT Request freed memory";

  close();
}

void DoTRequest::close() {
  if (closed_) {
    return;
  }
  closed_ = true;
  cb_ = nullptr;
  if (ssl_socket_) {
    ssl_socket_->Disconnect();
  } else if (socket_.is_open()) {
    asio::error_code ec;
    socket_.close(ec);
  }
}

void DoTRequest::DoRequest(dns_message::DNStype dns_type, const std::string& host, int port, AsyncResolveCallback cb) {
  dns_type_ = dns_type;
  host_ = host;
  port_ = port;
  cb_ = std::move(cb);

  if (is_localhost(host_)) {
    VLOG(3) << "DoT Request: localhost host: " << host_;
    scoped_refptr<DoTRequest> self(this);
    asio::post(io_context_, [this, self]() {
      struct addrinfo* addrinfo = addrinfo_loopback(dns_type_ == dns_message::DNS_TYPE_AAAA, port_);
      OnDoneRequest({}, addrinfo);
    });
    return;
  }

  dns_message::request msg;
  if (!msg.init(host, dns_type)) {
    OnDoneRequest(asio::error::host_unreachable, nullptr);
    return;
  }
  auto buf = gurl_base::MakeRefCounted<GrowableIOBuffer>();

  int payload_size = 0;
  for (auto buffer : msg.buffers()) {
    payload_size += buffer.size();
  }

  {
    uint16_t length = htons(payload_size);
    buf->appendBytesAtEnd(&length, sizeof(length));
  }

  for (auto buffer : msg.buffers()) {
    buf->appendBytesAtEnd(buffer.data(), buffer.size());
  }

  send_buf_ = buf;

  asio::error_code ec;
  socket_.open(endpoint_.protocol(), ec);
  if (ec) {
    OnDoneRequest(ec, nullptr);
    return;
  }
  socket_.non_blocking(true, ec);
  scoped_refptr<DoTRequest> self(this);
  socket_.async_connect(endpoint_, [this, self](asio::error_code ec) {
    // Cancelled, safe to ignore
    if (UNLIKELY(ec == asio::error::bad_descriptor || ec == asio::error::operation_aborted)) {
      return;
    }
    if (ec) {
      OnDoneRequest(ec, nullptr);
      return;
    }
    VLOG(3) << "DoT Remote Server Connected: " << endpoint_;
    // tcp socket connected
    OnSocketConnect();
  });
}

void DoTRequest::OnSocketConnect() {
  scoped_refptr<DoTRequest> self(this);
  asio::error_code ec;
  SetTCPCongestion(socket_.native_handle(), ec);
  SetTCPKeepAlive(socket_.native_handle(), ec);
  SetSocketTcpNoDelay(&socket_, ec);
  ssl_socket_ = SSLSocket::Create(ssl_socket_data_index_, nullptr, &io_context_, &socket_, ssl_ctx_,
                                  /*https_fallback*/ true, dot_host_, dot_port_);

  ssl_socket_->Connect([this, self](int rv) {
    asio::error_code ec;
    if (rv < 0) {
      ec = asio::error::connection_refused;
      OnDoneRequest(ec, nullptr);
      return;
    }
    VLOG(3) << "DoT Remote SSL Server Connected: " << endpoint_;
    // ssl socket connected
    OnSSLConnect();
  });
}

void DoTRequest::OnSSLConnect() {
  scoped_refptr<DoTRequest> self(this);

  // Also queue a ConfirmHandshake. It should also be blocked on ServerHello.
  absl::AnyInvocable<void(int)> cb = [this, self](int rv) {
    if (rv < 0) {
      asio::error_code ec = asio::error::connection_refused;
      OnDoneRequest(ec, nullptr);
    }
  };
  ssl_socket_->ConfirmHandshake(std::move(cb));

  if (!cb_) {
    return;
  }

  recv_buf_ = gurl_base::MakeRefCounted<GrowableIOBuffer>();
  ssl_socket_->WaitWrite([this, self](asio::error_code ec) { OnSSLWritable(ec); });
  ssl_socket_->WaitRead([this, self](asio::error_code ec) { OnSSLReadable(ec); });
}

void DoTRequest::OnSSLWritable(asio::error_code ec) {
  if (ec) {
    OnDoneRequest(ec, nullptr);
    return;
  }
  size_t written = ssl_socket_->Write(send_buf_.get(), ec);
  if (ec) {
    OnDoneRequest(ec, nullptr);
    return;
  }
  send_buf_->set_offset(send_buf_->offset() + written);
  VLOG(3) << "DoT Request Sent: " << written << " bytes Remaining: " << send_buf_->RemainingCapacity() << " bytes";
  if (UNLIKELY(!send_buf_->empty())) {
    scoped_refptr<DoTRequest> self(this);
    ssl_socket_->WaitWrite([this, self](asio::error_code ec) { OnSSLWritable(ec); });
    return;
  }
  VLOG(3) << "DoT Request Fully Sent";
}

void DoTRequest::OnSSLReadable(asio::error_code ec) {
  if (UNLIKELY(ec)) {
    OnDoneRequest(ec, nullptr);
    return;
  }
  size_t read;
  auto buf = gurl_base::MakeRefCounted<GrowableIOBuffer>();
  buf->SetCapacity(sizeof(uint16_t) + UINT16_MAX);
  do {
    ec = asio::error_code();
    read = ssl_socket_->Read(buf.get(), ec);
    if (ec == asio::error::interrupted) {
      continue;
    }
  } while (false);

  if (UNLIKELY(ec && ec != asio::error::try_again && ec != asio::error::would_block)) {
    OnDoneRequest(ec, nullptr);
    return;
  }
  // append buf to the end of recv_buf
  int previous_capacity = recv_buf_->capacity();
  recv_buf_->SetCapacity(previous_capacity + read);
  memcpy(recv_buf_->StartOfBuffer() + previous_capacity, buf->data(), read);

  VLOG(3) << "DoT Response Received: " << read << " bytes";

  switch (read_state_) {
    case Read_Header:
      OnReadHeader();
      break;
    case Read_Body:
      OnReadBody();
      break;
  }
}

void DoTRequest::OnReadHeader() {
  DCHECK_EQ(read_state_, Read_Header);
  uint16_t length;

  if (recv_buf_->RemainingCapacity() < (int)sizeof(length)) {
    LOG(WARNING) << "DoT Response Invalid DNS Response";
    OnDoneRequest(asio::error::operation_not_supported, nullptr);
    return;
  }

  memcpy(&length, recv_buf_->data(), sizeof(length));

  VLOG(3) << "DoT Response Header Parsed: " << sizeof(length) << " bytes";
  recv_buf_->set_offset(recv_buf_->offset() + sizeof(length));

  read_state_ = Read_Body;
  body_length_ = ntohs(length);

  OnReadBody();
}

void DoTRequest::OnReadBody() {
  DCHECK_EQ(read_state_, Read_Body);
  if (UNLIKELY(recv_buf_->RemainingCapacity() < body_length_)) {
    VLOG(3) << "DoT Response Expected Data: " << body_length_ << " bytes Current: " << recv_buf_->RemainingCapacity()
            << " bytes";

    scoped_refptr<DoTRequest> self(this);
    ssl_socket_->WaitRead([this, self](asio::error_code ec) { OnSSLReadable(ec); });
    return;
  }

  OnParseDnsResponse();
}

void DoTRequest::OnParseDnsResponse() {
  DCHECK_EQ(read_state_, Read_Body);

  dns_message::response_parser response_parser;
  dns_message::response response;

  dns_message::response_parser::result_type result;
  std::tie(result, std::ignore) = response_parser.parse(response, recv_buf_->data(), recv_buf_->data(),
                                                        recv_buf_->data() + recv_buf_->RemainingCapacity());
  if (result != dns_message::response_parser::good) {
    LOG(WARNING) << "DoT Response Bad Format";
    OnDoneRequest(asio::error::operation_not_supported, {});
    return;
  }
  VLOG(3) << "DoT Response Body Parsed: " << recv_buf_->RemainingCapacity() << " bytes";

  struct addrinfo* addrinfo = addrinfo_dup(dns_type_ == dns_message::DNS_TYPE_AAAA, response, port_);

  OnDoneRequest({}, addrinfo);
}

void DoTRequest::OnDoneRequest(asio::error_code ec, struct addrinfo* addrinfo) {
  if (auto cb = std::move(cb_)) {
    cb(ec, addrinfo);
  } else {
    addrinfo_freedup(addrinfo);
  }
}

}  // namespace net
