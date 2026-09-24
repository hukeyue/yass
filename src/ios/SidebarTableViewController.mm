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

@interface SidebarTableViewController () <UITableViewDelegate, UITableViewDataSource>
@end

@implementation SidebarTableViewController {
}

- (void)viewDidLoad {
  [super viewDidLoad];
}

- (void)viewWillAppear:(BOOL)animated {
  [super viewWillAppear:animated];
  [self UpdateStatusBar];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
  UISplitViewController *svc = self.splitViewController;
  UIViewController *vc = [svc viewControllerForColumn:UISplitViewControllerColumnSecondary];
  [self.splitViewController showDetailViewController:vc sender:self];
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
  NSString* text = [appDelegate getPaneMessage:section != 0 withLeft: NO];
    return text;
}

- (UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {

  UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"default" forIndexPath:indexPath];
  NSString* text = [self textForRowAt:indexPath];
  NSString* secondaryText = [self secondaryTextForRowAt:indexPath];

  UIListContentConfiguration* content = cell.defaultContentConfiguration;
  content.text = text;
  content.secondaryText = secondaryText;
  if (indexPath.row == 0) {
    if (@available(macOS 14.0, iOS 17.0, *)) {
      content.image = [UIImage systemImageNamed:indexPath.section == 0 ? @"arrow.up.circle.dotted" : @"arrow.down.circle.dotted"];
    } else {
      content.image = [UIImage systemImageNamed:indexPath.section == 0 ? @"arrow.up.circle" : @"arrow.down.circle"];
    }
  } else if (indexPath.row == 1) {
    content.image = [UIImage systemImageNamed:indexPath.section == 0 ? @"arrow.up.circle.fill" : @"arrow.down.circle.fill"];
  }
  cell.contentConfiguration = content;

  return cell;
}

- (NSString*)textForRowAt:(NSIndexPath*)indexPath {
  YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
  if (indexPath.row == 0) {
    return [appDelegate getRateMessage:indexPath.section != 0 withLeft: YES];
  }
  if (indexPath.row == 1) {
    return [appDelegate getTotalMessage:indexPath.section != 0 withLeft: YES];
  }
  return @"text";
}

- (NSString*)secondaryTextForRowAt:(NSIndexPath*)indexPath {
  YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
  if (indexPath.row == 0) {
    return [appDelegate getRateMessage:indexPath.section != 0 withLeft: NO];
  }
  if (indexPath.row == 1) {
    return [appDelegate getTotalMessage:indexPath.section != 0 withLeft: NO];
  }
  return @"text";
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
  return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
  return 2;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
  return 52;
}

- (void)UpdateStatusBar {
  [self.tableView reloadData];
}

@end
