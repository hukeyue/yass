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
  if (indexPath.section != 0) {
    return;
  }
  UISplitViewController *svc = self.splitViewController;
  UIViewController *vc = [svc viewControllerForColumn:UISplitViewControllerColumnSecondary];
  [self.splitViewController showDetailViewController:vc sender:self];
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
  YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
  if (section == 3)
    return nil;
  if (section == 0)
    return nil;
  NSString* text = [appDelegate getPaneMessage:section == 2 withLeft: NO];
  return text;
}

- (UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {

  UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"default" forIndexPath:indexPath];
  NSString* text = [self textForRowAt:indexPath];
  NSString* secondaryText = [self secondaryTextForRowAt:indexPath];

  UIListContentConfiguration* content = cell.defaultContentConfiguration;
  content.text = text;
  content.secondaryText = secondaryText;
  cell.selectionStyle = UITableViewCellSelectionStyleNone;
  cell.accessoryType = UITableViewCellAccessoryNone;
#if defined(TARGET_OS_TV) && !TARGET_OS_TV
  if (indexPath.section == 3) {
    content.secondaryText = @"";
    content.image = [UIImage systemImageNamed:@"bonjour"];
    UISwitch* sw = [[UISwitch alloc] init];
    [self swUpdate:sw];
    [sw addTarget:self action:@selector(swChanged:) forControlEvents:UIControlEventValueChanged];
    cell.accessoryView = sw;
  } else
#endif
  if (indexPath.section == 0) {
#if 0
    NSTextAttachment* cRight = [[NSTextAttachment alloc] init];
    cRight.image = [UIImage systemImageNamed:@"chevron.right"];

    NSAttributedString *secondaryDText = [NSAttributedString attributedStringWithAttachment:cRight];
    content.secondaryAttributedText = secondaryDText;
#else
    content.secondaryText = @"";
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
#endif
#if 0
    content.image = [UIImage systemImageNamed:@"gear"];
#else
    if (@available(macOS 26.0, iOS 26.0, tvOS 26.0, *)) {
      content.image = [UIImage systemImageNamed:@"cellularbars.circle.fill"];
    } else {
      content.image = [UIImage systemImageNamed:@"antenna.radiowaves.left.and.right"];
    }
#endif
  } else if (indexPath.row == 0) {
    if (@available(macOS 15.0, iOS 18.0, tvOS 18.0, *)) {
      content.image = [UIImage systemImageNamed:indexPath.section == 1 ? @"arrow.up.circle.dotted" : @"arrow.down.circle.dotted"];
    } else {
      content.image = [UIImage systemImageNamed:indexPath.section == 1 ? @"arrow.up.circle" : @"arrow.down.circle"];
    }
  } else if (indexPath.row == 1) {
    content.image = [UIImage systemImageNamed:indexPath.section == 1 ? @"arrow.up.circle.fill" : @"arrow.down.circle.fill"];
  }
  cell.contentConfiguration = content;

  return cell;
}

- (NSString*)textForRowAt:(NSIndexPath*)indexPath {
  YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
#if defined(TARGET_OS_TV) && !TARGET_OS_TV
  if (indexPath.section == 3) {
    return [appDelegate getSwitchMessage:indexPath.section == 2 withLeft: YES];
  }
#endif
  if (indexPath.section == 0) {
    return [appDelegate getConfigurationMessage:indexPath.section == 2 withLeft: YES];
  }
  if (indexPath.row == 0) {
    return [appDelegate getRateMessage:indexPath.section == 2 withLeft: YES];
  }
  if (indexPath.row == 1) {
    return [appDelegate getTotalMessage:indexPath.section == 2 withLeft: YES];
  }
  return @"text";
}

- (NSString*)secondaryTextForRowAt:(NSIndexPath*)indexPath {
  YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
#if defined(TARGET_OS_TV) && !TARGET_OS_TV
  if (indexPath.section == 3) {
    return [appDelegate getSwitchMessage:indexPath.section == 2 withLeft: NO];
  }
#endif
  if (indexPath.section == 0) {
    return [appDelegate getConfigurationMessage:indexPath.section == 2 withLeft: NO];
  }
  if (indexPath.row == 0) {
    return [appDelegate getRateMessage:indexPath.section == 2 withLeft: NO];
  }
  if (indexPath.row == 1) {
    return [appDelegate getTotalMessage:indexPath.section == 2 withLeft: NO];
  }
  return @"text";
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
#if defined(TARGET_OS_TV) && !TARGET_OS_TV
  return 4;
#else
  return 3;
#endif
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
  if (section == 0)
    return 1;
#if defined(TARGET_OS_TV) && !TARGET_OS_TV
  if (section == 3)
    return 1;
#endif
  return 2;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
  return 52;
}

#if defined(TARGET_OS_TV) && TARGET_OS_TV
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section {
  if (section == 1 || section == 2)
    return 52;
  return 0;
}
#endif // defined(TARGET_OS_TV) && TARGET_OS_TV

- (void)Starting {
  [self.tableView reloadData];
}

- (void)Started {
  [self.tableView reloadData];
}

- (void)StartFailed {
  [self.tableView reloadData];
}

- (void)Stopping {
  [self.tableView reloadData];
}

- (void)Stopped {
  [self.tableView reloadData];
}

- (void)UpdateStatusBar {
  [self.tableView reloadData];
}

#if defined(TARGET_OS_TV) && !TARGET_OS_TV
- (void)swUpdate:(UISwitch*)sw {
  YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
  enum YASSState state = [appDelegate getState];
  switch (state) {
    case STARTED:
      sw.enabled = YES;
      [sw setOn:YES];
      break;
    case STOPPED:
    case START_FAILED:
      sw.enabled = YES;
      [sw setOn:NO];
      break;
    case STARTING:
    case STOPPING:
    default:
      sw.enabled = NO;
      [sw setOn:NO];
      break;
  }
}

- (void)swChanged:(id)sender {
  YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
  enum YASSState state = [appDelegate getState];
  UISwitch* sw = sender;
  if (sw.isOn) {
    if (state == STOPPED || state == START_FAILED) {
      [appDelegate OnStart:FALSE];
    }
  } else {
    if (state == STARTED) {
      [appDelegate OnStop:FALSE];
    }
  }
}
#endif // defined(TARGET_OS_TV) && !TARGET_OS_TV

@end
