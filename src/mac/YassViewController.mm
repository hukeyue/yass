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

#import "mac/YassViewController.h"

#include <iomanip>
#include <sstream>

#include <absl/flags/flag.h>
#include <base/strings/sys_string_conversions.h>

#include "config/config.hpp"
#include "core/logging.hpp"
#include "core/utils.hpp"
#include "crypto/crypter_export.hpp"
#include "mac/YassAppDelegate.h"
#include "mac/YassWindowController.h"
#include "mac/utils.h"

static YassViewController* __weak _instance;

@interface YassViewController ()
@end

@implementation YassViewController {
  bool should_startup_at_app_load;
}

+ (YassViewController* __weak)instance {
  return _instance;
}

- (void)viewDidLoad {
  [super viewDidLoad];
  _instance = self;

  [self.cipherMethod removeAllItems];
  NSString* methodStrings[] = {
#define XX(num, name, string) @string,
      CIPHER_METHOD_VALID_MAP(XX)
#undef XX
  };
  for (NSString* methodString : methodStrings) {
    [self.cipherMethod addItemWithObjectValue:methodString];
  }
  self.cipherMethod.numberOfVisibleItems = sizeof(methodStrings) / sizeof(methodStrings[0]);
  should_startup_at_app_load = CheckLoginItemStatus(nullptr);
  [self.autoStart setState:(should_startup_at_app_load ? NSControlStateValueOn : NSControlStateValueOff)];
  [self.systemProxy setState:(GetSystemProxy() ? NSControlStateValueOn : NSControlStateValueOff)];
  [self LoadChanges];
}

- (void)viewWillAppear {
  [super viewWillAppear];
  // vc might be dismissed in starting/stopping state, refresh the state
  YassAppDelegate* appDelegate = (YassAppDelegate*)NSApplication.sharedApplication.delegate;
  switch ([appDelegate getState]) {
    case STARTED:
      [self Started];
      break;
    case START_FAILED:
      [self StartFailed];
      break;
    case STOPPED:
      [self Stopped];
      break;
    default:
      break;
  }

  [self.view.window center];
}

- (void)viewDidAppear {
  [super viewDidAppear];
  if (should_startup_at_app_load) {
    should_startup_at_app_load = false;
    [self OnStart];
  }
}

- (IBAction)OnStartButtonClicked:(id)sender {
  [self OnStart];
}

- (IBAction)OnStopButtonClicked:(id)sender {
  [self OnStop];
}

- (IBAction)OnAutoStartChecked:(id)sender {
  bool enable = self.autoStart.state == NSControlStateValueOn;
  if (enable && !CheckLoginItemStatus(nullptr)) {
    AddToLoginItems(true);
  } else {
    RemoveFromLoginItems();
  }
}

- (IBAction)OnSystemProxyChecked:(id)sender {
  bool enable = self.systemProxy.state == NSControlStateValueOn;
  [self.systemProxy setEnabled:FALSE];
  // this operation might be slow, moved to background
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    SetSystemProxy(enable);
    dispatch_async(dispatch_get_main_queue(), ^{
      [self.systemProxy setEnabled:TRUE];
    });
  });
}

- (IBAction)OnDisplayStatusClicked:(id)sender {
  YassWindowController* windowController = [YassWindowController instance];
  bool enable = self.displayStatus.state == NSControlStateValueOn;
  [windowController toggleDisplayStatus:enable];
}

- (void)OnStart {
  [self.startButton setEnabled:FALSE];
  [self.stopButton setEnabled:FALSE];

  [self.serverHost setEnabled:FALSE];
  [self.serverSNI setEnabled:FALSE];
  [self.serverPort setEnabled:FALSE];
  [self.username setEnabled:FALSE];
  [self.password setEnabled:FALSE];
  [self.cipherMethod setEnabled:FALSE];
  [self.localHost setEnabled:FALSE];
  [self.localPort setEnabled:FALSE];
  [self.dohURL setEnabled:FALSE];
  [self.dotHost setEnabled:FALSE];
  [self.limitRate setEnabled:FALSE];
  [self.timeout setEnabled:FALSE];

  YassWindowController* windowController = [YassWindowController instance];
  [windowController OnStart];
}

- (void)OnStop {
  [self.startButton setEnabled:FALSE];
  [self.stopButton setEnabled:FALSE];

  YassWindowController* windowController = [YassWindowController instance];
  [windowController OnStop];
}

- (void)Started {
  [self.startButton setEnabled:FALSE];
  [self.stopButton setEnabled:TRUE];
}

- (void)StartFailed {
  [self.startButton setEnabled:TRUE];
  [self.stopButton setEnabled:FALSE];

  [self.serverHost setEnabled:TRUE];
  [self.serverSNI setEnabled:TRUE];
  [self.serverPort setEnabled:TRUE];
  [self.username setEnabled:TRUE];
  [self.password setEnabled:TRUE];
  [self.cipherMethod setEnabled:TRUE];
  [self.localHost setEnabled:TRUE];
  [self.localPort setEnabled:TRUE];
  [self.dohURL setEnabled:TRUE];
  [self.dotHost setEnabled:TRUE];
  [self.limitRate setEnabled:TRUE];
  [self.timeout setEnabled:TRUE];
}

- (void)Stopped {
  [self.startButton setEnabled:TRUE];
  [self.stopButton setEnabled:FALSE];

  [self.serverHost setEnabled:TRUE];
  [self.serverSNI setEnabled:TRUE];
  [self.serverPort setEnabled:TRUE];
  [self.username setEnabled:TRUE];
  [self.password setEnabled:TRUE];
  [self.cipherMethod setEnabled:TRUE];
  [self.localHost setEnabled:TRUE];
  [self.localPort setEnabled:TRUE];
  [self.dohURL setEnabled:TRUE];
  [self.dotHost setEnabled:TRUE];
  [self.limitRate setEnabled:TRUE];
  [self.timeout setEnabled:TRUE];
}

- (void)LoadChanges {
  self.serverHost.stringValue = SysUTF8ToNSString(absl::GetFlag(FLAGS_server_host));
  self.serverSNI.stringValue = SysUTF8ToNSString(absl::GetFlag(FLAGS_server_sni));
  self.serverPort.intValue = absl::GetFlag(FLAGS_server_port);
  self.username.stringValue = SysUTF8ToNSString(absl::GetFlag(FLAGS_username));
  self.password.stringValue = SysUTF8ToNSString(absl::GetFlag(FLAGS_password));
  self.cipherMethod.stringValue = SysUTF8ToNSString(std::string_view(absl::GetFlag(FLAGS_method)));
  self.localHost.stringValue = SysUTF8ToNSString(absl::GetFlag(FLAGS_local_host));
  self.localPort.intValue = absl::GetFlag(FLAGS_local_port);
  self.dohURL.stringValue = SysUTF8ToNSString(absl::GetFlag(FLAGS_doh_url));
  self.dotHost.stringValue = SysUTF8ToNSString(absl::GetFlag(FLAGS_dot_host));
  self.limitRate.stringValue = SysUTF8ToNSString(std::string(absl::GetFlag(FLAGS_limit_rate)));
  self.timeout.intValue = absl::GetFlag(FLAGS_connect_timeout);

  BOOL enable_status_bar = absl::GetFlag(FLAGS_ui_display_realtime_status) ? TRUE : FALSE;
  self.displayStatus.state = enable_status_bar ? NSControlStateValueOn : NSControlStateValueOff;
}

@end
