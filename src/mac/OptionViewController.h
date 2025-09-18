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

#ifndef YASS_OPTION_VIEW_CONTROLLER
#define YASS_OPTION_VIEW_CONTROLLER

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface OptionViewController : NSViewController
@property(weak) IBOutlet NSButton* tcpKeepAlive;
@property(weak) IBOutlet NSTextField* tcpKeepAliveCnt;
@property(weak) IBOutlet NSTextField* tcpKeepAliveIdleTimeout;
@property(weak) IBOutlet NSTextField* tcpKeepAliveInterval;
@property(weak) IBOutlet NSButton* enablePostQuantumKyber;

@end

NS_ASSUME_NONNULL_END

#endif  // YASS_OPTION_VIEW_CONTROLLER
