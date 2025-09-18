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

#ifndef YASS_MAC_VIEW_CONTROLLER
#define YASS_MAC_VIEW_CONTROLLER

#import <Cocoa/Cocoa.h>

@interface YassViewController : NSViewController
- (void)OnStart;
- (void)OnStop;
- (void)Started;
- (void)StartFailed;
- (void)Stopped;
@property(weak) IBOutlet NSButton* startButton;
@property(weak) IBOutlet NSButton* stopButton;
@property(weak) IBOutlet NSTextField* serverHost;
@property(weak) IBOutlet NSTextField* serverSNI;
@property(weak) IBOutlet NSTextField* serverPort;
@property(weak) IBOutlet NSTextField* username;
@property(weak) IBOutlet NSSecureTextField* password;
@property(weak) IBOutlet NSComboBox* cipherMethod;
@property(weak) IBOutlet NSTextField* localHost;
@property(weak) IBOutlet NSTextField* localPort;
@property(weak) IBOutlet NSTextField* dohURL;
@property(weak) IBOutlet NSTextField* dotHost;
@property(weak) IBOutlet NSTextField* limitRate;
@property(weak) IBOutlet NSTextField* timeout;
@property(weak) IBOutlet NSButton* autoStart;
@property(weak) IBOutlet NSButton* systemProxy;
@property(weak) IBOutlet NSButton* displayStatus;

+ (YassViewController* __weak)instance;
@end

#endif  // YASS_MAC_VIEW_CONTROLLER
