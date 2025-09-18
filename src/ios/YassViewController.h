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

#ifndef YASS_IOS_VIEW_CONTROLLER
#define YASS_IOS_VIEW_CONTROLLER

#import <UIKit/UIKit.h>

@interface YassViewController : UIViewController
- (void)OnStart;
- (void)OnStop;
- (void)Starting;
- (void)Started;
- (void)StartFailed;
- (void)Stopping;
- (void)Stopped;
- (void)UpdateStatusBar;
@property(weak, nonatomic) IBOutlet UIButton* startButton;
@property(weak, nonatomic) IBOutlet UIButton* stopButton;
@property(weak, nonatomic) IBOutlet UITextField* serverHost;
@property(weak, nonatomic) IBOutlet UITextField* serverSNI;
@property(weak, nonatomic) IBOutlet UITextField* serverPort;
@property(weak, nonatomic) IBOutlet UITextField* username;
@property(weak, nonatomic) IBOutlet UITextField* password;
@property(weak, nonatomic) IBOutlet UIPickerView* cipherMethod;
@property(weak, nonatomic) IBOutlet UITextField* dohURL;
@property(weak, nonatomic) IBOutlet UITextField* dotHost;
@property(weak, nonatomic) IBOutlet UITextField* limitRate;
@property(weak, nonatomic) IBOutlet UITextField* timeout;

@property(strong, nonatomic) NSString* currentCiphermethod;

@property(weak, nonatomic) IBOutlet UILabel* status;
@property(weak, nonatomic) IBOutlet NSLayoutConstraint* bottomConstraint;
@property(weak, nonatomic) IBOutlet UIScrollView* contentView;

@end

#endif  // YASS_IOS_VIEW_CONTROLLER
