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

#ifndef _H_HARMONY_TUN2PROXY_HPP
#define _H_HARMONY_TUN2PROXY_HPP

extern "C"
void* tun2proxy_init(const char* proxy_url,
                     int tun_fd,
                     int tun_mtu,
                     int log_level,
                     int dns_over_tcp);

extern "C"
int tun2proxy_run(void* ptr);

extern "C"
int tun2proxy_shutdown(void* ptr);

extern "C"
void tun2proxy_destroy(void* ptr);

#endif //  _H_HARMONY_TUN2PROXY_HPP
