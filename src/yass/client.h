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

#ifndef YASS_CLIENT_H
#define YASS_CLIENT_H

#include "yass/config.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* yass_client_instance;

/// Create Instance of YASS Client
YASS_DLL yass_client_instance yass_client_instance_create();
/// Initialize
YASS_DLL int yass_client_instance_init(yass_client_instance instance);
/// Add proxy and listen pair
YASS_DLL int yass_client_instance_add_server_uri(yass_client_instance instance, int64_t server_tag, const char* proxy_uri, const char* listen_uri, uint16_t* listen_port);
/// Add proxy and listen pair
YASS_DLL int yass_client_instance_add_server(yass_client_instance instance, int64_t server_tag, const char* remote_host_name, const char* remote_host_sni, uint16_t remote_port, const char* remote_username, const char* remote_password, int remote_cipher, bool remote_padding_support, const char* local_host_name, uint16_t local_port, bool redir_mode, uint16_t* listen_port);
/// Run loop (block API)
YASS_DLL int yass_client_instance_run(yass_client_instance instance);
/// Gracefully shutdown (Thread-Safe)
YASS_DLL int yass_client_instance_shutdown(yass_client_instance instance);
/// Cancel current I/O and leave the run-loop (Thread-Safe)
YASS_DLL int yass_client_instance_cancel(yass_client_instance instance);
/// Destroy Instance of YASS Client (not Thread-Safe)
YASS_DLL void yass_client_instance_destroy(yass_client_instance instance);
/// Get Last Error (number, not Thread-Safe)
YASS_DLL int yass_client_instance_get_last_error(yass_client_instance instance);
/// Get Last Error (string, not Thread-Safe)
YASS_DLL const char* yass_client_instance_get_last_error_str(yass_client_instance instance);

#ifdef __cplusplus
} // extern "C"
#endif

#endif  // YASS_CLIENT_H
