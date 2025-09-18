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

#import "ios/YassSceneDelegate.h"

@interface YassSceneDelegate ()

@end

@implementation YassSceneDelegate

- (void)scene:(UIScene*)scene
    willConnectToSession:(UISceneSession*)session
                 options:(UISceneConnectionOptions*)connectionOptions {
  // Use this method to optionally configure and attach the UIWindow `window` to the provided UIWindowScene `scene`.
  // If using a storyboard, the `window` property will automatically be initialized and attached to the scene.
  // This delegate does not imply the connecting scene or session are new (see
  // `application:configurationForConnectingSceneSession` instead).
}

- (void)sceneDidDisconnect:(UIScene*)scene {
  // Called as the scene is being released by the system.
  // This occurs shortly after the scene enters the background, or when its session is discarded.
  // Release any resources associated with this scene that can be re-created the next time the scene connects.
  // The scene may re-connect later, as its session was not necessarily discarded (see
  // `application:didDiscardSceneSessions` instead).
}

- (void)sceneDidBecomeActive:(UIScene*)scene {
  // Called when the scene has moved from an inactive state to an active state.
  // Use this method to restart any tasks that were paused (or not yet started) when the scene was inactive.
}

- (void)sceneWillResignActive:(UIScene*)scene {
  // Called when the scene will move from an active state to an inactive state.
  // This may occur due to temporary interruptions (ex. an incoming phone call).
}

- (void)sceneWillEnterForeground:(UIScene*)scene {
  // Called as the scene transitions from the background to the foreground.
  // Use this method to undo the changes made on entering the background.
}

- (void)sceneDidEnterBackground:(UIScene*)scene {
  // Called as the scene transitions from the foreground to the background.
  // Use this method to save data, release shared resources, and store enough scene-specific state information
  // to restore the scene back to its current state.
}

@end
