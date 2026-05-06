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

#include "core/utils_socket.hpp"

#ifndef _WIN32
#include <unistd.h>
#endif

bool DuplicateSocket(socket_t fd, socket_t* dup_fd) {
#ifdef _WIN32
  WSAPROTOCOL_INFOW pi{};
  if (::WSADuplicateSocketW(fd, ::GetCurrentProcessId(), &pi) != 0) {
    return false;
  }
  socket_t fd2 = ::WSASocketW(pi.iAddressFamily, pi.iSocketType, pi.iProtocol, &pi, 0, 0);
  if (fd2 == INVALID_SOCKET) {
    return false;
  }
#else
  socket_t fd2 = dup(fd);
  if (fd2 < 0) {
    return false;
  }
#endif
  *dup_fd = fd2;
  return true;
}
