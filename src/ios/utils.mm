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

/* Copyright (c) 2023-2025 Chilledheart  */

#include "ios/utils.h"

#include <atomic>
#include <thread>

#include <Network/Network.h>

#include "core/logging.hpp"

static std::atomic<bool> connectedNetwork;
static dispatch_queue_t monitorQueue;
static nw_path_monitor_t monitor;
static std::once_flag initFlag;

// https://forums.developer.apple.com/forums/thread/105822
void initNetworkPathMonitor() {
  DCHECK(!monitor);

  std::call_once(initFlag, []() {
    dispatch_queue_attr_t attrs = dispatch_queue_attr_make_with_qos_class(DISPATCH_QUEUE_SERIAL, QOS_CLASS_UTILITY,
                                                                          DISPATCH_QUEUE_PRIORITY_DEFAULT);
    monitorQueue = dispatch_queue_create("it.gui.yass.network-monitor", attrs);
  });

  // monitor = nw_path_monitor_create_with_type(nw_interface_type_wifi);
  monitor = nw_path_monitor_create();
  nw_path_monitor_set_queue(monitor, monitorQueue);
  if (@available(iOS 14, *)) {
    nw_path_monitor_prohibit_interface_type(monitor, nw_interface_type_loopback);
  }
  nw_path_monitor_set_update_handler(monitor, ^(nw_path_t _Nonnull path) {
    nw_path_status_t status = nw_path_get_status(path);
    // Determine the active interface, but how?
    switch (status) {
      case nw_path_status_invalid: {
        // Network path is invalid
        connectedNetwork = false;
        break;
      }
      case nw_path_status_satisfied: {
        // Network is usable
        connectedNetwork = true;
        break;
      }
      case nw_path_status_satisfiable: {
        // Network may be usable
        connectedNetwork = false;
        break;
      }
      case nw_path_status_unsatisfied: {
        // Network is not usable
        connectedNetwork = false;
        break;
      }
    }
  });

  nw_path_monitor_start(monitor);
}

void deinitNetworkPathMonitor() {
  DCHECK(monitor);

  nw_path_monitor_cancel(monitor);
  monitor = nw_path_monitor_t();
}

bool connectedToNetwork() {
  return connectedNetwork;
}
