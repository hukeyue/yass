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

/* Copyright (c) 2022-2025 Chilledheart  */

#import "mac/OptionViewController.h"

#include <absl/flags/flag.h>

#include "config/config.hpp"
#include "core/logging.hpp"
#include "core/utils.hpp"

@interface OptionViewController ()

@end

@implementation OptionViewController

- (void)viewDidLoad {
  [super viewDidLoad];

  [self.tcpKeepAlive setState:(absl::GetFlag(FLAGS_tcp_keep_alive) ? NSControlStateValueOn : NSControlStateValueOff)];
  self.tcpKeepAliveCnt.intValue = absl::GetFlag(FLAGS_tcp_keep_alive_cnt);
  self.tcpKeepAliveIdleTimeout.intValue = absl::GetFlag(FLAGS_tcp_keep_alive_idle_timeout);
  self.tcpKeepAliveInterval.intValue = absl::GetFlag(FLAGS_tcp_keep_alive_interval);
  [self.enablePostQuantumKyber
      setState:(absl::GetFlag(FLAGS_enable_post_quantum_kyber) ? NSControlStateValueOn : NSControlStateValueOff)];
}

- (void)viewDidDisappear {
  [NSApp stopModalWithCode:NSModalResponseCancel];
}

- (IBAction)OnOkButtonClicked:(id)sender {
  absl::SetFlag(&FLAGS_tcp_keep_alive, self.tcpKeepAlive.state == NSControlStateValueOn);
  absl::SetFlag(&FLAGS_tcp_keep_alive_cnt, self.tcpKeepAliveCnt.intValue);
  absl::SetFlag(&FLAGS_tcp_keep_alive_idle_timeout, self.tcpKeepAliveIdleTimeout.intValue);
  absl::SetFlag(&FLAGS_tcp_keep_alive_interval, self.tcpKeepAliveInterval.intValue);
  absl::SetFlag(&FLAGS_enable_post_quantum_kyber, self.enablePostQuantumKyber.state == NSControlStateValueOn);
  config::SaveConfig();
  [self dismissViewController:self];
}

- (IBAction)OnCancelButtonClicked:(id)sender {
  [self dismissViewController:self];
}

@end
