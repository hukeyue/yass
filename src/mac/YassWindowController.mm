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

/* Copyright (c) 2024-2026 Chilledheart  */

#import "mac/YassWindowController.h"

#include <sstream>

#include <absl/flags/flag.h>
#include <base/strings/sys_string_conversions.h>

#include "config/config.hpp"
#include "core/logging.hpp"
#include "core/utils.hpp"
#include "crypto/crypter_export.hpp"
#include "mac/YassAppDelegate.h"
#import "mac/YassViewController.h"
#include "mac/utils.h"

static YassWindowController* __weak _instance;

@interface YassWindowController ()
- (void)toggleDisplayStatusInternal:(bool)enable;
@end

@implementation YassWindowController {
  NSStatusItem* y_status_bar_item_;
  NSStatusItem* status_bar_item_;
  enum YASSState y_status_bar_state_;
  NSString* status_bar_text_;
  BOOL hide_on_closed_;
}

+ (YassWindowController* __weak)instance {
  return _instance;
}

- (void)windowDidLoad {
  [super windowDidLoad];
  _instance = self;

  y_status_bar_item_ = [[NSStatusBar systemStatusBar] statusItemWithLength:NSSquareStatusItemLength];

  y_status_bar_state_ = STOPPING;
  if (@available(macOS 11.0, iOS 14.0, *)) {
    // noop
  } else {
    y_status_bar_item_.button.title = @"Y";
  }
  y_status_bar_item_.button.target = self;
  y_status_bar_item_.button.action = @selector(statusItemClicked);
  [y_status_bar_item_.button
      sendActionOn:NSEventMaskLeftMouseDown | NSEventMaskRightMouseDown | NSEventMaskOtherMouseDown];
  [self UpdateStatusYBar];

  bool enable_status_bar = absl::GetFlag(FLAGS_ui_display_realtime_status);
  [self toggleDisplayStatusInternal:enable_status_bar];

  hide_on_closed_ = FALSE;
}

- (BOOL)windowShouldClose:(NSWindow*)sender {
  if (sender != self.window) {
    return TRUE;
  }
  SetDockIconStyle(true);
  hide_on_closed_ = TRUE;

  [self.window orderOut:nil];
  return FALSE;
}

- (void)statusItemClicked {
  if (hide_on_closed_) {
    SetDockIconStyle(false);
    hide_on_closed_ = FALSE;
  }

  NSWindow* window = self.window;
  [window performSelector:@selector(orderFrontRegardless) withObject:nil afterDelay:0.10];
  [window makeKeyAndOrderFront:nil];

  if (![NSApp isActive]) {
    [NSApp activateIgnoringOtherApps:YES];
  }
}

- (void)UpdateStatusYBar {
  YassAppDelegate* appDelegate = (YassAppDelegate*)NSApplication.sharedApplication.delegate;
  enum YASSState state = [appDelegate getState];
  if (state == y_status_bar_state_) {
    return;
  }
  if (@available(macOS 11.0, iOS 14.0, *)) {
    NSImage* imageToReplaced;
    if (state == STARTED) {
      imageToReplaced = [NSImage imageWithSystemSymbolName:@"network" accessibilityDescription:nil];
    } else {
      imageToReplaced = [NSImage imageWithSystemSymbolName:@"network.slash" accessibilityDescription:nil];
    }
    if (imageToReplaced != nil) {
      y_status_bar_item_.button.title = @"";
      y_status_bar_item_.button.image = imageToReplaced;
    } else {
      y_status_bar_item_.button.title = @"Y";
    }
  }
  y_status_bar_state_ = state;
}

- (void)UpdateStatusBar {
  [self UpdateStatusYBar];
  if (status_bar_item_ == nil) {
    status_bar_text_ = nil;
    return;
  }
  YassAppDelegate* appDelegate = (YassAppDelegate*)NSApplication.sharedApplication.delegate;
  NSString* next_status_bar_text = [appDelegate getStatusMessage];
  // prevent deep copy on unchanged title
  if ([next_status_bar_text isEqualToString:status_bar_text_]) {
    return;
  }
  status_bar_text_ = next_status_bar_text;
  status_bar_item_.button.title = next_status_bar_text;
}

- (void)OnStart {
  YassAppDelegate* appDelegate = (YassAppDelegate*)NSApplication.sharedApplication.delegate;

  [appDelegate OnStart:FALSE];
}

- (void)OnStop {
  YassAppDelegate* appDelegate = (YassAppDelegate*)NSApplication.sharedApplication.delegate;
  [appDelegate OnStop:FALSE];
}

- (void)Starting {
  [self UpdateStatusBar];
  YassViewController* viewController = [YassViewController instance];
  [viewController Starting];
}

- (void)Started {
  [self UpdateStatusBar];
  YassViewController* viewController = [YassViewController instance];
  [viewController Started];
}

- (void)StartFailed {
  [self UpdateStatusBar];
  YassViewController* viewController = [YassViewController instance];
  [viewController StartFailed];
}

- (void)Stopping {
  [self UpdateStatusBar];
  YassViewController* viewController = [YassViewController instance];
  [viewController Stopping];
}

- (void)Stopped {
  [self UpdateStatusBar];
  YassViewController* viewController = [YassViewController instance];
  [viewController Stopped];
}

- (void)toggleDisplayStatus:(bool)enable {
  [self toggleDisplayStatusInternal:enable];
  absl::SetFlag(&FLAGS_ui_display_realtime_status, enable);
  config::SaveConfig();
}

- (void)toggleDisplayStatusInternal:(bool)enable {
  if (enable && status_bar_item_ == nil) {
    status_bar_item_ = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    [self UpdateStatusBar];
  } else if (!enable && status_bar_item_ != nil) {
    status_bar_item_ = nil;
    status_bar_text_ = nil;
  }
}

@end
