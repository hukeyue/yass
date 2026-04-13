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

#ifndef YASS_CLIENT_H
#define YASS_CLIENT_H

#include "yass/config.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define YASS_CLIENT_ABI_VERSION 1
#define YASS_CLIENT_ABI_STABLE  0

typedef void* yass_client_instance;

/*!
 *  \brief Construct an instance of YassClient.
 *
 *  return NULL if error occurs
 */
YASS_DLL yass_client_instance yass_client_instance_create();
/*!
 *  \brief Initialize specified instance of YassClient.
 *
 *  \param instance specified instance of YassClient.
 *
 *  return non-zero if error occurs
 */
YASS_DLL int yass_client_instance_init(yass_client_instance instance);
/*!
 *  \brief Add server configuration to specified instance of YassClient (variant 1).
 *
 *  \param instance    specified instance.
 *  \param server_tag  the number to distinguish the server configuration
 *  \param proxy_uri   the URI format of upstream including username and password where server proxies network traffic to
 *  \param listen_uri  the URI format of downstream where server listens at locally
 *  \param listen_port if not NULL, the pointer will be assigned the real port where server listens
 *
 *  return non-zero if error occurs
 */
YASS_DLL int yass_client_instance_add_server_uri(yass_client_instance instance, int64_t server_tag, const char* proxy_uri,
                                                 const char* listen_uri, uint16_t* listen_port);

YASS_DLL int yass_client_instance_add_server_uri_v1(yass_client_instance instance, int64_t server_tag, const char* proxy_uri,
                                                    const char* listen_uri, uint16_t* listen_port,
                                                    char* remote_server_ips_cstr, size_t* remote_server_ips_cstr_len,
                                                    char* remote_server_ips_v4_cstr, size_t* remote_server_ips_v4_cstr_len,
                                                    char* remote_server_ips_v6_cstr, size_t* remote_server_ips_v6_cstr_len);
/*!
 *  \brief Add server configuration to specified instance of YassClient (variant 2).
 *
 *  \param instance                specified instance.
 *  \param server_tag              the number to distinguish the server configuration
 *  \param remote_host_name        the hostname of upstream where server proxies network traffic to, cannot be NULL
 *  \param remote_host_sni         the SNI to override hostname of upstream where server proxies network traffic to, can be NULL
 *  \param remote_port             the port number of upstream where server proxies network traffic to, cannot be zero
 *  \param remote_username         the username of upstream where server proxies network traffic to, can be NULL
 *  \param remote_password         the password of upstream where server proxies network traffic to, can be NULL
 *  \param remote_cipher           the cipher number form of upstream where server proxies network traffic to, cannot be invalid
 *  \param remote_padding_support  the padding support of upstream where server proxies network traffic to, only valid on YASS_CRYPTO_HTTP2
 *  \param local_host_name         the hostname of downstream where server listens at locally, cannot be NULL
 *  \param local_port              the port number of downstream where server listens at locally, cannot be zero
 *  \param redir_mode              the redir mode of downstream where server listens at locally, usually be false
 *  \param listen_port if not NULL, the pointer will be assigned the real port where server listens
 *
 *  return non-zero if error occurs
 */
YASS_DLL int yass_client_instance_add_server(yass_client_instance instance, int64_t server_tag, const char* remote_host_name,
                                             const char* remote_host_sni, uint16_t remote_port, const char* remote_username,
                                             const char* remote_password, int remote_cipher, bool remote_padding_support,
                                             const char* local_host_name, uint16_t local_port, bool redir_mode, uint16_t* listen_port);

YASS_DLL int yass_client_instance_add_server_v1(yass_client_instance instance, int64_t server_tag, const char* remote_host_name,
                                                const char* remote_host_sni, uint16_t remote_port, const char* remote_username,
                                                const char* remote_password, int remote_cipher, bool remote_padding_support,
                                                const char* local_host_name, uint16_t local_port, bool redir_mode, uint16_t* listen_port,
                                                char* remote_server_ips_cstr, size_t* remote_server_ips_cstr_len,
                                                char* remote_server_ips_v4_cstr, size_t* remote_server_ips_v4_cstr_len,
                                                char* remote_server_ips_v6_cstr, size_t* remote_server_ips_v6_cstr_len);
/*!
 *  \brief Run the internal loop to poll the request
 *
 *  \param instance specified instance of YassClient.
 *
 *  block current thread, all configuration will be dropped after this call
 */
YASS_DLL int yass_client_instance_run(yass_client_instance instance);
/*!
 *  \brief Count current number of connections
 *
 *  \param instance specified instance of YassClient.
 *
 *  thread-safe
 */
YASS_DLL int yass_client_instance_num_of_connections(yass_client_instance instance);
/*!
 *  \brief Shutdown Instance of YASS Client Gracefully
 *
 *  \param instance specified instance of YassClient.
 *
 *  thread-safe, all pending I/O will be handled
 */
YASS_DLL int yass_client_instance_shutdown(yass_client_instance instance);
/*!
 *  \brief Stop Instance of YASS Client Immediately
 *
 *  \param instance specified instance of YassClient.
 *
 *  thread-safe, all pending I/O will be cancelled
 */
YASS_DLL int yass_client_instance_cancel(yass_client_instance instance);
/*!
 *  \brief Destroy Instance of YASS Client
 *
 *  \param instance specified instance of YassClient.
 *
 *  not thread-safe
 */
YASS_DLL void yass_client_instance_destroy(yass_client_instance instance);
/*!
 *  \brief Get Last Error (Number) from the given instance of YassClient
 *
 *  \param instance specified instance of YassClient.
 *
 *  not thread-safe
 */
YASS_DLL int yass_client_instance_get_last_error(yass_client_instance instance);
/*!
 *  \brief Get Last Error (String) from the given instance of YassClient
 *
 *  \param instance specified instance of YassClient.
 *
 *  not thread-safe
 */
YASS_DLL const char* yass_client_instance_get_last_error_str(yass_client_instance instance);

#ifdef __cplusplus
} // extern "C"
#endif

#endif  // YASS_CLIENT_H
