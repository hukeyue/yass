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

#import "ios/YassViewController.h"

#include <sstream>

#include <absl/flags/flag.h>
#include <base/strings/sys_string_conversions.h>

#include "config/config.hpp"
#include "core/logging.hpp"
#include "core/utils.hpp"
#include "crypto/crypter_export.hpp"
#include "ios/YassAppDelegate.h"

#ifndef TLSEXT_MAXLEN_host_name
#define TLSEXT_MAXLEN_host_name 255
#endif
#if defined(TARGET_OS_TV) && !TARGET_OS_TV
@interface YassViewController () <UIPickerViewDataSource, UIPickerViewDelegate, UITextFieldDelegate>
@end
#else
@interface YassViewController () <UITextFieldDelegate>
@end
#endif

@implementation YassViewController {
  NSArray* cipher_methods_;
#if defined(TARGET_OS_TV) && !TARGET_OS_TV
  __weak UIBarButtonItem* startBarButton_;
  __weak UIBarButtonItem* stopBarButton_;
#endif
}

- (void)viewDidLoad {
  [super viewDidLoad];

  cipher_methods_ = @[
#define XX(num, name, string) @string,
    YASS_CIPHER_METHOD_VALID_MAP(XX)
#undef XX
  ];
  self.currentCiphermethod = @(CRYPTO_DEFAULT_CSTR);
#if defined(TARGET_OS_TV) && !TARGET_OS_TV
  self.cipherMethod = [[UIPickerView alloc] init];
  [self.cipherMethod setDelegate:self];
  [self.cipherMethod setDataSource:self];
  [self.cipherMethod reloadAllComponents];
  CGFloat multiplier = 0.333333f;
#else
  self.cipherMethod = [[UILabel alloc] init];
  CGFloat multiplier = 1.0f;
  [self.cipherMethod setTextAlignment:NSTextAlignmentCenter];
#endif
  [self.cipherMethod setContentMode:UIViewContentModeScaleToFill];
  [self.cipherMethodStack addArrangedSubview:self.cipherMethod];
  NSLayoutConstraint *constraint = [NSLayoutConstraint constraintWithItem:self.cipherMethodStack.arrangedSubviews[0] attribute:NSLayoutAttributeWidth relatedBy:NSLayoutRelationEqual toItem:self.cipherMethodStack.arrangedSubviews[1] attribute:NSLayoutAttributeWidth multiplier:multiplier constant:20.0f];
  [NSLayoutConstraint activateConstraints:@[constraint]];
  [self.serverHost setDelegate:self];
  [self.serverSNI setDelegate:self];
  [self.serverPort setDelegate:self];
  [self.username setDelegate:self];
  [self.password setDelegate:self];
  [self.dohURL setDelegate:self];
  [self.dotHost setDelegate:self];
  [self.limitRate setDelegate:self];
  [self.timeout setDelegate:self];

  [self LoadChanges];
  [self UpdateStatusBar];
  [self.startButton setEnabled:TRUE];
  [self.stopButton setEnabled:FALSE];

  UIButtonConfiguration* bbConfig = [UIButtonConfiguration borderedButtonConfiguration];
  bbConfig.title = self.startButton.configuration.title;
#if defined(TARGET_OS_TV) && !TARGET_OS_TV
  bbConfig.image = [UIImage systemImageNamed:@"play.fill"];
  bbConfig.imagePadding = 9;
#endif
  self.startButton.configuration = bbConfig;

  bbConfig = [UIButtonConfiguration borderedButtonConfiguration];
  bbConfig.title = self.stopButton.configuration.title;
#if defined(TARGET_OS_TV) && !TARGET_OS_TV
  bbConfig.image = [UIImage systemImageNamed:@"stop.fill"];
  bbConfig.imagePadding = 9;
#endif
  self.stopButton.configuration = bbConfig;

  UIBarButtonItem* lBarButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:self action:nil];
  UIBarButtonItem* startBarButton = [[UIBarButtonItem alloc] initWithTitle:NSLocalizedString(@"Start", @"Start") style:UIBarButtonItemStylePlain target:self action:@selector(OnStartButtonClicked:)];
  UIBarButtonItem* stopBarButton = [[UIBarButtonItem alloc] initWithTitle:NSLocalizedString(@"Stop", @"Stop") style:UIBarButtonItemStylePlain target:self action:@selector(OnStopButtonClicked:)];
  UIBarButtonItem* rBarButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:self action:nil];
  /* currently iPhone Duo only */
  if (@available(iOS 27.1, *)) {
    startBarButton.image = [UIImage systemImageNamed:@"play.fill"];
    stopBarButton.image = [UIImage systemImageNamed:@"stop.fill"];
  }
  self.toolbarItems = @[lBarButton, startBarButton, stopBarButton, rBarButton];
  startBarButton_ = startBarButton;
  stopBarButton_ = stopBarButton;

  [startBarButton setEnabled:TRUE];
  [stopBarButton setEnabled:FALSE];
}

- (void)UpdateNavigationBar {
#if defined(TARGET_OS_TV) && !TARGET_OS_TV
  bool is_ios_duo_only = false;
  bool is_compact_vc = self.traitCollection.horizontalSizeClass == UIUserInterfaceSizeClassCompact;
  if (@available(iOS 27.1, *)) {
    is_ios_duo_only = true;
  }
  if (!is_compact_vc && !is_ios_duo_only) {
    self.navigationController.toolbarHidden = YES;
    self.controlStackView.hidden = NO;
    self.status.hidden = NO;
  } else {
    self.navigationController.toolbarHidden = NO;
    self.controlStackView.hidden = YES;
    self.status.hidden = YES;
  }
#endif // defined(TARGET_OS_TV) && !TARGET_OS_TV
}

- (void)traitCollectionDidChange:(UITraitCollection *)previousTraitCollection {
  [super traitCollectionDidChange:previousTraitCollection];

  YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
  [appDelegate reloadState];

  [self UpdateNavigationBar];
}

- (void)viewWillAppear:(BOOL)animated {
  [super viewWillAppear:animated];

  YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
  [appDelegate reloadState];

  [self UpdateStatusBar];
  [self UpdateNavigationBar];
#if defined(TARGET_OS_TV) && !TARGET_OS_TV
  [[NSNotificationCenter defaultCenter]
      addObserverForName:UIKeyboardWillShowNotification
                  object:nil
                   queue:[NSOperationQueue mainQueue]
              usingBlock:^(NSNotification* _Nonnull notification) {
                NSDictionary* userInfo = notification.userInfo;
                NSTimeInterval duration = [userInfo[UIKeyboardAnimationDurationUserInfoKey] doubleValue];
                CGRect keyboardEndFrame = [self.view convertRect:[userInfo[UIKeyboardFrameEndUserInfoKey] CGRectValue]
                                                          toView:self.view.window];

                CGRect windowFrame = [self.view convertRect:self.view.window.frame toView:self.view.window];
                CGFloat heightOffset = (windowFrame.size.height - keyboardEndFrame.origin.y) - self.view.frame.origin.y;

                self.bottomConstraint.constant = heightOffset + 20;

                [UIView animateWithDuration:duration
                                 animations:^{
                                   [self.view layoutIfNeeded];
                                 }];
              }];

  [[NSNotificationCenter defaultCenter] addObserverForName:UIKeyboardWillHideNotification
                                                    object:nil
                                                     queue:[NSOperationQueue mainQueue]
                                                usingBlock:^(NSNotification* _Nonnull notification) {
                                                  NSDictionary* userInfo = notification.userInfo;
                                                  NSTimeInterval duration =
                                                      [userInfo[UIKeyboardAnimationDurationUserInfoKey] doubleValue];
                                                  self.bottomConstraint.constant = 20;
                                                  [UIView animateWithDuration:duration
                                                                   animations:^{
                                                                     [self.view layoutIfNeeded];
                                                                   }];
                                                }];
#endif
}

#if defined(TARGET_OS_TV) && TARGET_OS_TV
- (NSArray<id<UIFocusEnvironment>> *)preferredFocusEnvironments {
    return @[self.serverHost, self.serverSNI, self.serverPort, self.username, self.password, self.cipherMethod, self.dohURL, self.dotHost, self.limitRate, self.timeout];
}
#endif

#if defined(TARGET_OS_TV) && !TARGET_OS_TV
- (BOOL)textFieldShouldReturn:(UITextField*)textField {
  if (textField == self.serverHost) {
    [textField resignFirstResponder];
    [self.serverSNI becomeFirstResponder];
  } else if (textField == self.serverSNI) {
    [textField resignFirstResponder];
    [self.serverPort becomeFirstResponder];
  } else if (textField == self.serverPort) {
    [textField resignFirstResponder];
    [self.username becomeFirstResponder];
  } else if (textField == self.username) {
    [textField resignFirstResponder];
    [self.password becomeFirstResponder];
  } else if (textField == self.password) {
    [textField resignFirstResponder];
    [self.dohURL becomeFirstResponder];
  } else if (textField == self.dohURL) {
    [textField resignFirstResponder];
    [self.dotHost becomeFirstResponder];
  } else if (textField == self.dotHost) {
    [textField resignFirstResponder];
    [self.limitRate becomeFirstResponder];
  } else if (textField == self.limitRate) {
    [textField resignFirstResponder];
    [self.timeout becomeFirstResponder];
  } else if (textField == self.timeout) {
    [textField resignFirstResponder];
    [self OnStart];
  }

  return NO;
}
#endif // defined(TARGET_OS_TV) && !TARGET_OS_TV

- (BOOL)textFieldShouldEndEditing:(UITextField*)textField {
  if (textField == self.serverHost) {
    if (textField.text.length > TLSEXT_MAXLEN_host_name) {
      return NO;
    }
  }
  if (textField == self.serverSNI) {
    if (textField.text.length > TLSEXT_MAXLEN_host_name) {
      return NO;
    }
  }
  if (textField == self.serverPort) {
    int port;
    return StringToInt(SysNSStringToUTF8(textField.text), &port) && port > 0 && port <= UINT16_MAX ? YES : NO;
  }
  if (textField == self.dohURL && [textField.text length]) {
    NSURL* url = [NSURL URLWithString:textField.text];
    return url.scheme && [url.scheme isEqualToString:@"https"] && url.host ? YES : NO;
  }
  if (textField == self.dotHost) {
    if (textField.text.length > TLSEXT_MAXLEN_host_name) {
      return NO;
    }
  }
  if (textField == self.limitRate) {
    auto _limit_rate = SysNSStringToUTF8(textField.text);
    std::string err;
    RateFlag limit_rate;
    if (!AbslParseFlag(_limit_rate, &limit_rate, &err)) {
      return NO;
    }
  }
  if (textField == self.timeout) {
    int timeout;
    return StringToInt(SysNSStringToUTF8(textField.text), &timeout) && timeout >= 0 ? YES : NO;
  }
  return YES;
}

#if defined(TARGET_OS_TV) && !TARGET_OS_TV
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView*)pickerView {
  return 1;
}

- (NSInteger)pickerView:(UIPickerView*)pickerView numberOfRowsInComponent:(NSInteger)component {
  return [cipher_methods_ count];
}

- (NSString*)pickerView:(UIPickerView*)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component {
  return [cipher_methods_ objectAtIndex:row];
}

- (void)pickerView:(UIPickerView*)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component {
  self.currentCiphermethod = [cipher_methods_ objectAtIndex:row];
}
#endif

- (IBAction)OnStartButtonClicked:(id)sender {
  [self OnStart];
}

- (IBAction)OnStopButtonClicked:(id)sender {
  [self OnStop];
}

- (void)OnStart {
  YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
  [appDelegate OnStart:FALSE];
}

- (void)OnStop {
  YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
  [appDelegate OnStop:FALSE];
}

- (void)Starting {
  [self UpdateStatusBar];
  [self.serverHost setUserInteractionEnabled:FALSE];
  [self.serverSNI setUserInteractionEnabled:FALSE];
  [self.serverPort setUserInteractionEnabled:FALSE];
  [self.username setUserInteractionEnabled:FALSE];
  [self.password setUserInteractionEnabled:FALSE];
  [self.cipherMethod setUserInteractionEnabled:FALSE];
  [self.dohURL setUserInteractionEnabled:FALSE];
  [self.dotHost setUserInteractionEnabled:FALSE];
  [self.limitRate setUserInteractionEnabled:FALSE];
  [self.timeout setUserInteractionEnabled:FALSE];
  [self.startButton setEnabled:FALSE];
  [self.stopButton setEnabled:FALSE];

  [startBarButton_ setEnabled:FALSE];
  [stopBarButton_ setEnabled:FALSE];
}

- (void)Started {
  [self UpdateStatusBar];
  [self.serverHost setUserInteractionEnabled:FALSE];
  [self.serverSNI setUserInteractionEnabled:FALSE];
  [self.serverPort setUserInteractionEnabled:FALSE];
  [self.username setUserInteractionEnabled:FALSE];
  [self.password setUserInteractionEnabled:FALSE];
  [self.cipherMethod setUserInteractionEnabled:FALSE];
  [self.dohURL setUserInteractionEnabled:FALSE];
  [self.dotHost setUserInteractionEnabled:FALSE];
  [self.limitRate setUserInteractionEnabled:FALSE];
  [self.timeout setUserInteractionEnabled:FALSE];
  [self.startButton setEnabled:FALSE];
  [self.stopButton setEnabled:TRUE];

  [startBarButton_ setEnabled:FALSE];
  [stopBarButton_ setEnabled:TRUE];
}

- (void)StartFailed {
  [self UpdateStatusBar];
  [self.serverHost setUserInteractionEnabled:TRUE];
  [self.serverSNI setUserInteractionEnabled:TRUE];
  [self.serverPort setUserInteractionEnabled:TRUE];
  [self.username setUserInteractionEnabled:TRUE];
  [self.password setUserInteractionEnabled:TRUE];
  [self.cipherMethod setUserInteractionEnabled:TRUE];
  [self.dohURL setUserInteractionEnabled:TRUE];
  [self.dotHost setUserInteractionEnabled:TRUE];
  [self.limitRate setUserInteractionEnabled:TRUE];
  [self.timeout setUserInteractionEnabled:TRUE];
  [self.startButton setEnabled:TRUE];
  [self.stopButton setEnabled:FALSE];

  [startBarButton_ setEnabled:TRUE];
  [stopBarButton_ setEnabled:FALSE];

}

- (void)Stopping {
  [self UpdateStatusBar];
  [self.serverHost setUserInteractionEnabled:FALSE];
  [self.serverSNI setUserInteractionEnabled:FALSE];
  [self.serverPort setUserInteractionEnabled:FALSE];
  [self.username setUserInteractionEnabled:FALSE];
  [self.password setUserInteractionEnabled:FALSE];
  [self.cipherMethod setUserInteractionEnabled:FALSE];
  [self.dohURL setUserInteractionEnabled:FALSE];
  [self.dotHost setUserInteractionEnabled:FALSE];
  [self.limitRate setUserInteractionEnabled:FALSE];
  [self.timeout setUserInteractionEnabled:FALSE];
  [self.startButton setEnabled:FALSE];
  [self.stopButton setEnabled:FALSE];

  [startBarButton_ setEnabled:FALSE];
  [stopBarButton_ setEnabled:FALSE];
}

- (void)Stopped {
  [self UpdateStatusBar];
  [self.serverHost setUserInteractionEnabled:TRUE];
  [self.serverSNI setUserInteractionEnabled:TRUE];
  [self.serverPort setUserInteractionEnabled:TRUE];
  [self.username setUserInteractionEnabled:TRUE];
  [self.password setUserInteractionEnabled:TRUE];
  [self.cipherMethod setUserInteractionEnabled:TRUE];
  [self.dohURL setUserInteractionEnabled:TRUE];
  [self.dotHost setUserInteractionEnabled:TRUE];
  [self.limitRate setUserInteractionEnabled:TRUE];
  [self.timeout setUserInteractionEnabled:TRUE];
  [self.startButton setEnabled:TRUE];
  [self.stopButton setEnabled:FALSE];

  [startBarButton_ setEnabled:TRUE];
  [stopBarButton_ setEnabled:FALSE];
}

- (void)UpdateStatusBar {
  YassAppDelegate* appDelegate = (YassAppDelegate*)UIApplication.sharedApplication.delegate;
  self.status.text = [appDelegate getStatusMessage];
}

- (void)LoadChanges {
  self.serverHost.text = SysUTF8ToNSString(absl::GetFlag(FLAGS_server_host));
  self.serverSNI.text = SysUTF8ToNSString(absl::GetFlag(FLAGS_server_sni));
  self.serverPort.text = SysUTF8ToNSString(std::to_string(absl::GetFlag(FLAGS_server_port)));
  self.username.text = SysUTF8ToNSString(absl::GetFlag(FLAGS_username));
  self.password.text = SysUTF8ToNSString(absl::GetFlag(FLAGS_password));
  NSString* cipherMethod = SysUTF8ToNSString(std::string_view(absl::GetFlag(FLAGS_method)));
#if defined(TARGET_OS_TV) && !TARGET_OS_TV
  NSUInteger row = [cipher_methods_ indexOfObject:cipherMethod];
  if (row != NSNotFound) {
    self.currentCiphermethod = cipherMethod;
    [self.cipherMethod selectRow:row inComponent:0 animated:NO];
  }
#else
  self.cipherMethod.text = cipherMethod;
#endif

  self.dohURL.text = SysUTF8ToNSString(absl::GetFlag(FLAGS_doh_url));
  self.dotHost.text = SysUTF8ToNSString(absl::GetFlag(FLAGS_dot_host));
  self.limitRate.text = SysUTF8ToNSString(std::string(absl::GetFlag(FLAGS_limit_rate)));
  self.timeout.text = SysUTF8ToNSString(std::to_string(absl::GetFlag(FLAGS_connect_timeout)));
}

@end
