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

/* Copyright (c) 2023-2026 Chilledheart  */

#import "ios/SidebarTableViewController.h"
#import "ios/YassAppDelegate.h"

#include "core/logging.hpp"
#include "core/utils.hpp"

@interface SidebarTableViewController () <UITableViewDelegate, UITableViewDataSource>
@end

@implementation SidebarTableViewController {
  uint64_t last_sync_time_;
  uint64_t last_rx_bytes_;
  uint64_t last_tx_bytes_;
  uint64_t rx_rate_;
  uint64_t tx_rate_;
  uint64_t rx_bytes_;
  uint64_t tx_bytes_;
}

- (void)viewDidLoad {
  [super viewDidLoad];
  if (@available(iOS 15.0, *)) {
    self.tableView.sectionHeaderHeight = 0;
  }
}

- (UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {

  UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:indexPath.row == 0 ? @"title" : @"default" forIndexPath:indexPath];
  NSString* text = [self textForRowAt:indexPath];
  cell.textLabel.text = text;
  return cell;
}

- (NSString*)textForRowAt:(NSIndexPath*)indexPath {
  if (indexPath.row == 0) {
    return [self getPaneMessage:indexPath.section != 0];
  }
  if (indexPath.row == 1) {
    return [self getRateMessage:indexPath.section != 0];
  }
  if (indexPath.row == 2) {
    return [self getTotalMessage:indexPath.section != 0];
  }
  return @"text";
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
  return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
  return 3;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
  if (indexPath.row == 0)
    return 28;
  return 42;
}

- (void)UpdateStatusBar {
  [self UpdateStatusInternalStage];
  [self.tableView reloadData];
}

- (void)UpdateStatusInternalStage {
  YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
  if ([appDelegate getState] != STARTED) {
    return;
  }
  uint64_t sync_time = GetMonotonicTime();
  uint64_t delta_time = sync_time - last_sync_time_;
  if (delta_time > NS_PER_SECOND) {
    uint64_t rx_bytes = appDelegate.total_rx_bytes;
    uint64_t tx_bytes = appDelegate.total_tx_bytes;
    rx_rate_ = static_cast<double>(rx_bytes - last_rx_bytes_) / delta_time * NS_PER_SECOND;
    tx_rate_ = static_cast<double>(tx_bytes - last_tx_bytes_) / delta_time * NS_PER_SECOND;
    last_sync_time_ = sync_time;
    last_rx_bytes_ = rx_bytes;
    last_tx_bytes_ = tx_bytes;
    rx_bytes_ = rx_bytes;
    tx_bytes_ = tx_bytes;
  }
}

- (NSString*)getPaneMessage:(BOOL)isReceiveSide {
  return !isReceiveSide ? NSLocalizedString(@"TX Pane", @"Send Pane") : NSLocalizedString(@"RX Pane", @"Receive Pane");
}

- (NSString*)getRateMessage:(BOOL)isReceiveSide {
  std::ostringstream ss;
  NSString *message;
  if (!isReceiveSide) {
    message = NSLocalizedString(@"TXRATE", @"tx rate:");
    ss << " " << SysNSStringToUTF8(message) << " ";
    HumanReadableByteCountBin(&ss, rx_rate_);
    ss << "/s";
  } else {
    message = NSLocalizedString(@"RXRATE", @"rx rate:");
    ss << " " << SysNSStringToUTF8(message) << " ";
    HumanReadableByteCountBin(&ss, tx_rate_);
    ss << "/s";
  }
  return SysUTF8ToNSString(ss.str());
}

- (NSString*)getTotalMessage:(BOOL)isReceiveSide {
  std::ostringstream ss;
  NSString *message;
  if (!isReceiveSide) {
    message = NSLocalizedString(@"TX", @"tx:");
    ss << " " << SysNSStringToUTF8(message) << " ";
    HumanReadableByteCountBin(&ss, rx_bytes_);
  } else {
    message = NSLocalizedString(@"RX", @"rx:");
    ss << " " << SysNSStringToUTF8(message) << " ";
    HumanReadableByteCountBin(&ss, tx_bytes_);
  }
  return SysUTF8ToNSString(ss.str());
}

@end
