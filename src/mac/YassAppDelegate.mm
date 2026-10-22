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

/* Copyright (c) 2022-2026 Chilledheart  */

#import "mac/YassAppDelegate.h"

#ifdef BUILD_NETWORK_EXTENSION
#import <NetworkExtension/NetworkExtension.h>
#else
#include "cli/cli_connection_stats.hpp"
#include "cli/cli_worker.hpp"
#endif

#include <stdexcept>
#include <string>
#include <string_view>

#include <absl/flags/flag.h>
#include <base/strings/sys_string_conversions.h>

#include "apple/utils.h"
#include "config/config.hpp"
#include "_yass/gui_variant.h"
#include "core/logging.hpp"
#include "core/utils.hpp"
#include "crypto/crypter_export.hpp"
#include "mac/OptionViewController.h"
#include "mac/YassViewController.h"
#include "mac/YassWindowController.h"
#include "mac/utils.h"
#include "yass/feature.h"
#include "yass/version.h"

#define PI_DOUBLE 3.14159265357

@interface YassAppDelegate ()
- (std::string)SaveConfig;
- (void)OnStarting;
- (void)OnStarted;
- (void)OnStartFailed:(std::string)error_msg;
- (void)OnStopping;
- (void)OnStopped;
#ifdef BUILD_NETWORK_EXTENSION
- (void)OnStartSaveAndLoadInstance:(NETunnelProviderManager*)vpn_manager;
- (void)OnStartFailedWithNSError:(NSError* _Nullable)error;
#endif
@end

@implementation YassAppDelegate {
  enum YASSState state_;

  NSString* status_;
  std::string error_msg_;
  struct TelemetryObserver telemetry_observer_;
  NSTimer* refresh_timer_;

  NSString* server_host_;
  NSString* server_sni_;
  NSString* server_port_;
  NSString* username_;
  NSString* password_;
  NSString* method_string_;
  NSString* local_host_;
  NSString* local_port_;
  NSString* doh_url_;
  NSString* dot_host_;
  NSString* limit_rate_;
  NSString* connect_timeout_;
  BOOL enable_post_quantum_kyber_;
#ifdef BUILD_NETWORK_EXTENSION
  NETunnelProviderManager* vpn_manager_;
#else
  Worker worker_;
#endif
  NSInteger effective_port_;
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
  SetDockIconStyle(false);
  initNetworkPathMonitor();
  effective_port_ = -1;

#ifdef BUILD_NETWORK_EXTENSION
  reinstallPlugin();
  [self didDefaultsChanged:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(didDefaultsChanged:)
                                               name:NSUserDefaultsDidChangeNotification
                                             object:nil];

  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(didChangeVpnStatus:)
                                               name:NEVPNStatusDidChangeNotification
                                             object:nil];
#endif
  if (![NSApp isActive]) {
    [NSApp activateIgnoringOtherApps:YES];
  }
}

- (void)applicationWillTerminate:(NSNotification*)aNotification {
  LOG(WARNING) << "Application exiting";
  [self OnStop:TRUE];
  deinitNetworkPathMonitor();
#ifdef BUILD_NETWORK_EXTENSION
#ifndef NDEBUG
  uninstallPlugin();
#endif
#endif
}

- (BOOL)applicationSupportsSecureRestorableState:(NSApplication*)app {
  return YES;
}

- (IBAction)OnAboutMenuClicked:(id)sender {
  NSMutableParagraphStyle* paragraphStyle = [[NSMutableParagraphStyle alloc] init];
  paragraphStyle.alignment = NSTextAlignmentCenter;

  NSDictionary* attributes = @{
    NSForegroundColorAttributeName : [NSColor labelColor],
    NSFontAttributeName : [NSFont systemFontOfSize:[NSFont labelFontSize]],
    NSParagraphStyleAttributeName : paragraphStyle
  };

  NSAttributedString* enabled_features =
      [[NSAttributedString alloc] initWithString:@("Enabled Features: " YASS_APP_FEATURES "\n") attributes:attributes];
  NSAttributedString* gui_variant = [[NSAttributedString alloc] initWithString:@("GUI Variant: " YASS_GUI_FLAVOUR)
                                                                    attributes:attributes];

  NSMutableAttributedString* credits = [[NSMutableAttributedString alloc] init];
  [credits appendAttributedString:enabled_features];
  [credits appendAttributedString:gui_variant];

  NSDictionary<NSAboutPanelOptionKey, id>* dict = @{
    NSAboutPanelOptionApplicationName : @(YASS_APP_PRODUCT_NAME),
    NSAboutPanelOptionApplicationVersion : @(YASS_APP_PRODUCT_VERSION),
    NSAboutPanelOptionVersion : @(YASS_APP_LAST_CHANGE),
    NSAboutPanelOptionCredits : credits,
  };
  [[NSApplication sharedApplication] orderFrontStandardAboutPanelWithOptions:dict];
}

- (IBAction)OnOptionMenuClicked:(id)sender {
  NSStoryboard* storyboard = [NSStoryboard storyboardWithName:@"Main" bundle:nil];
  OptionViewController* optionViewController =
      (OptionViewController*)[storyboard instantiateControllerWithIdentifier:@"OptionViewController"];
  YassViewController* viewController = [self getViewController];
  [viewController presentViewControllerAsModalWindow:optionViewController];
}

- (IBAction)OnQuitMenuClicked:(id)sender {
  [[NSApplication sharedApplication] terminate:nil];
}

#pragma mark - Application

- (void)reloadState {
#ifdef BUILD_NETWORK_EXTENSION
  [NETunnelProviderManager loadAllFromPreferencesWithCompletionHandler:^(
                               NSArray<NETunnelProviderManager*>* _Nullable managers, NSError* _Nullable error) {
    if (error) {
      vpn_manager_ = nil;
      [self didChangeVpnStatus:nil];
      return;
    }
    NETunnelProviderManager* vpn_manager;
    if ([managers count] == 0) {
      vpn_manager_ = nil;
      [self didChangeVpnStatus:nil];
      return;
    } else {
      vpn_manager = managers[0];
    }
    vpn_manager_ = vpn_manager.enabled ? vpn_manager : nil;
    [self didChangeVpnStatus:nil];
  }];
#else
  YassWindowController* windowController = [self getWindowController];
  switch ([self getState]) {
    case STARTING:
      [windowController Starting];
      break;
    case STARTED:
      [windowController Started];
      break;
    case START_FAILED:
      [windowController StartFailed];
      break;
    case STOPPING:
      [windowController Stopping];
      break;
    case STOPPED:
      [windowController Stopped];
      break;
    default:
      break;
  }
#endif // BUILD_NETWORK_EXTENSION
}

- (enum YASSState)getState {
  return state_;
}

- (YassWindowController*)getWindowController {
  for (NSWindow *window in NSApplication.sharedApplication.windows) {
    NSWindowController *windowController = [window windowController];
    if ([windowController isKindOfClass:[YassWindowController class]]) {
      return (YassWindowController*)windowController;
    }
  }
  return nil;
}

- (YassViewController*)getViewController {
  NSWindowController *windowController = [self getWindowController];
  NSViewController *viewController = [windowController contentViewController];
  if (![viewController isKindOfClass:[YassViewController class]]) {
    NSLog(@"viewController mismatched");
    return nil;
  }
  return (YassViewController*)viewController;
}

- (NSString*)getStatus {
  std::ostringstream ss;
  if (state_ == STARTED) {
    NSString* prefixMessage = NSLocalizedString(@"CONNECTED", @"Connected");
    ss << SysNSStringToUTF8(prefixMessage) << ":";
  } else if (state_ == STARTING) {
    ss << SysNSStringToUTF8(NSLocalizedString(@"CONNECTING", @"Connecting"));
  } else if (state_ == START_FAILED) {
    NSString* prefixMessage = NSLocalizedString(@"FAILED_TO_CONNECT_DUE_TO", @"Failed to connect due to");
    ss << SysNSStringToUTF8(prefixMessage) << " " << error_msg_.c_str();
  } else if (state_ == STOPPING) {
    ss << SysNSStringToUTF8(NSLocalizedString(@"DISCONNECTING", @"Disconnecting"));
  } else {
    NSString* prefixMessage = NSLocalizedString(@"DISCONNECTED_WITH", @"Disconnected with");
#ifdef BUILD_NETWORK_EXTENSION
    ss << SysNSStringToUTF8(prefixMessage) << " " << absl::GetFlag(FLAGS_server_host);
#else
    ss << SysNSStringToUTF8(prefixMessage) << " " << worker_.GetRemoteDomain();
#endif
  }

  return SysUTF8ToNSString(ss.str());
}

#ifdef BUILD_NETWORK_EXTENSION
- (void)OnStart:(BOOL)quiet {
  [self OnStarting];

  if (!connectedToNetwork()) {
    NSString* message = NSLocalizedString(@"NETWORK_UNREACHABLE", @"Network unreachable");
    [self OnStartFailed:SysNSStringToUTF8(message)];
    return;
  }
  auto err_msg = [self SaveConfig];
  if (!err_msg.empty()) {
    [self OnStartFailed:err_msg];
    return;
  }

  config::SaveConfig();

  [NETunnelProviderManager loadAllFromPreferencesWithCompletionHandler:^(
                               NSArray<NETunnelProviderManager*>* _Nullable managers, NSError* _Nullable error) {
    if (error != nil) {
      [self OnStartFailedWithNSError:error];
      return;
    }
    NETunnelProviderManager* vpn_manager;
    if ([managers count] == 0) {
      vpn_manager = [[NETunnelProviderManager alloc] init];
    } else {
      vpn_manager = managers[0];
    }
    [self OnStartSaveAndLoadInstance:vpn_manager];
  }];
}

- (void)OnStop:(BOOL)quiet {
  [self OnStopping];

  if (vpn_manager_ != nil) {
    // state will reflect in NEVPNStatusDidChangeNotification
    [vpn_manager_.connection stopVPNTunnel];
    // [self didChangeVpnStatus:nil];
  } else {
    [self OnStopped];
  }
}

- (void)FetchTelemetryData {
  if (vpn_manager_ != nil && state_ == STARTED) {
    NETunnelProviderSession* session = (NETunnelProviderSession*)vpn_manager_.connection;
    NSData* requestData = [@(kAppMessageGetTelemetry) dataUsingEncoding:NSUTF8StringEncoding];
    NSError* error;
    [session sendProviderMessage:requestData
                     returnError:&error
                 responseHandler:^(NSData* _Nullable responseData) {
                   if (!responseData) {
                     return;
                   }
                   std::string_view response((const char*)responseData.bytes, responseData.length);
                   uint64_t total_rx_bytes;
                   uint64_t total_tx_bytes;
                   if (!parseTelemetryJson(response, &total_rx_bytes, &total_tx_bytes)) {
                     LOG(WARNING) << "telemetry: Invalid response: " << response;
                     return;
                   }
                   dispatch_async(dispatch_get_main_queue(), ^{
                     [self UpdateTelemetryObserver:total_rx_bytes with:total_tx_bytes];
                   });
                 }];
    if (error) {
      std::string err_msg = SysNSStringToUTF8([error localizedDescription]);
      LOG(WARNING) << "telemetry: send Request failed: " << err_msg;
    }
  }
}

- (void)UpdateTelemetryObserver:(uint64_t)total_rx_bytes with:(uint64_t)total_tx_bytes {
  if ([self getState] != STARTED) {
    finalizeTelemetryObserver(&telemetry_observer_);
  } else {
    updateTelemetryObserver(&telemetry_observer_, total_rx_bytes, total_tx_bytes);
  }
  YassWindowController* windowController = [self getWindowController];
  [windowController UpdateStatusBar];

#ifndef NDEBUG
  NSLog(@"status message: %@", [self getStatusMessage]);
#endif
}

- (void)FetchListenPortData {
  if (vpn_manager_ != nil && state_ == STARTED) {
    NETunnelProviderSession* session = (NETunnelProviderSession*)vpn_manager_.connection;
    NSData* requestData = [@(kAppMessageGetEffectivePort) dataUsingEncoding:NSUTF8StringEncoding];
    NSError* error;
    [session sendProviderMessage:requestData
                     returnError:&error
                 responseHandler:^(NSData* _Nullable responseData) {
                   if (!responseData) {
                     NSLog(@"empty response from %s", __func__);
                     return;
                   }
                   if (responseData.length != sizeof(NSInteger)) {
                     NSLog(@"bad response from %s", __func__);
                   }
                   NSInteger effective_port;
                   memcpy(&effective_port, (const char*)responseData.bytes, responseData.length);
                   NSLog(@"listen port response %ld from rpc", effective_port);
                   dispatch_async(dispatch_get_main_queue(), ^{
                      [self UpdateEffectivePort:effective_port];
                   });
                 }];
    if (error) {
      std::string err_msg = SysNSStringToUTF8([error localizedDescription]);
      LOG(WARNING) << "listen port: send Request failed: " << err_msg;
    }
  }
}

- (void)UpdateEffectivePort:(NSInteger)effective_port {
  if ([self getState] == STARTED) {
    effective_port_ = effective_port;
    YassWindowController* windowController = [self getWindowController];
    [windowController UpdateStatusBar];
    LOG(WARNING) << "listen port: " << effective_port;
  }
}

- (void)OnStartSaveAndLoadInstance:(NETunnelProviderManager*)vpn_manager {
  NETunnelProviderProtocol* tunnelProtocol;
  if (!vpn_manager.protocolConfiguration) {
    tunnelProtocol = [[NETunnelProviderProtocol alloc] init];
  } else {
    tunnelProtocol = (NETunnelProviderProtocol*)vpn_manager.protocolConfiguration;
  }

  tunnelProtocol.providerBundleIdentifier = @"it.gui.ios.yass.network-extension";
  tunnelProtocol.disconnectOnSleep = FALSE;
  tunnelProtocol.serverAddress = @"Yet Another Shadow Socket";
#if 0
  if (@available(iOS 15.1, *)) {
    LOG(INFO) << "Activating includeAllNetworks";
    tunnelProtocol.includeAllNetworks = TRUE;
    tunnelProtocol.excludeLocalNetworks = TRUE;
    if (@available(iOS 16.4, *)) {
      // By default, APNs are excluded from the VPN tunnel on 16.4 and later
      tunnelProtocol.excludeAPNs = FALSE;
    }
  }
#endif

  tunnelProtocol.providerConfiguration = @{
    @(kServerHostFieldName) : server_host_,
    @(kServerSNIFieldName) : server_sni_,
    @(kServerPortFieldName) : server_port_,
    @(kUsernameFieldName) : username_,
    @(kPasswordFieldName) : password_,
    @(kLocalHostFieldName) : local_host_,
    @(kLocalPortFieldName) : local_port_,
    @(kMethodStringFieldName) : method_string_,
    @(kDoHURLFieldName) : doh_url_,
    @(kDoTHostFieldName) : dot_host_,
    @(kLimitRateFieldName) : limit_rate_,
    @(kConnectTimeoutFieldName) : connect_timeout_,
    @(kEnablePostQuantumKyberKey) : @(enable_post_quantum_kyber_),
  };
  tunnelProtocol.username = @"";
  tunnelProtocol.identityDataPassword = @"";
  vpn_manager.protocolConfiguration = tunnelProtocol;
  vpn_manager.localizedDescription = @"YASS";
  vpn_manager.enabled = TRUE;

  [vpn_manager saveToPreferencesWithCompletionHandler:^(NSError* _Nullable error) {
    if (error != nil) {
      [self OnStartFailedWithNSError:error];
      return;
    }
    [vpn_manager loadFromPreferencesWithCompletionHandler:^(NSError* _Nullable error) {
      if (error != nil) {
        [self OnStartFailedWithNSError:error];
        return;
      }
      BOOL ret = [vpn_manager.connection startVPNTunnelAndReturnError:&error];
      if (ret == TRUE) {
        vpn_manager_ = vpn_manager;
        // state will reflect in NEVPNStatusDidChangeNotification
        // [self didChangeVpnStatus:nil];
      } else {
        [self OnStartFailedWithNSError:error];
      }
    }];
  }];
}

#pragma mark - Notification

- (void)didDefaultsChanged:(NSNotification*)notification {
  // Use standard one for data that is only for Host App.
  // Use suiteName for data that you want to share between Extension and Host App.
  enable_post_quantum_kyber_ = [[NSUserDefaults standardUserDefaults] boolForKey:@(kEnablePostQuantumKyberKey)];
  absl::SetFlag(&FLAGS_enable_post_quantum_kyber, enable_post_quantum_kyber_);
}

- (void)didChangeVpnStatus:(NSNotification*)notification {
  if (!vpn_manager_) {
    status_ = NSLocalizedString(@"DISCONNECTED", @"Disconnected");
    [self OnStopped];
    return;
  }
  NEVPNStatus status = vpn_manager_.connection.status;
  switch (status) {
    case NEVPNStatusConnecting:
      NSLog(@"Connecting...");
      status_ = NSLocalizedString(@"CONNECTING", @"Connecting");
      [self OnStarting];
      break;
    case NEVPNStatusConnected:
      NSLog(@"Connected...");
      status_ = NSLocalizedString(@"CONNECTED", @"Connected");
      [self OnStarted];
      break;
    case NEVPNStatusDisconnecting:
      NSLog(@"Disconnecting...");
      status_ = NSLocalizedString(@"DISCONNECTING", @"Disconnecting");
      [self OnStopping];
      break;
    case NEVPNStatusDisconnected:
      NSLog(@"Disconnected...");
      status_ = NSLocalizedString(@"DISCONNECTED", @"Disconnected");
      [self OnStopped];
      break;
    case NEVPNStatusInvalid:
      NSLog(@"Invalid");
      status_ = NSLocalizedString(@"INVALID", @"Invalid");
      [self OnStopped];
      break;
    case NEVPNStatusReasserting:
      NSLog(@"Reasserting...");
      status_ = NSLocalizedString(@"REASSERTING", @"Reasserting");
      [self OnStopped];
      break;
    default:
      NSLog(@"Unknown status... %@", @(status));
      status_ = NSLocalizedString(@"UNKNOWN", @"Unknown");
      [self OnStopped];
      break;
  }
}

- (void)OnStarting {
  state_ = STARTING;
  effective_port_ = -1;

  YassWindowController* windowController = [self getWindowController];
  [windowController Starting];
}

- (void)OnStarted {
  state_ = STARTED;
  effective_port_ = -1;

  [self FetchListenPortData];

  YassWindowController* windowController = [self getWindowController];
  [windowController Started];

  refresh_timer_ = [NSTimer scheduledTimerWithTimeInterval:NSTimeInterval(1.0/PI_DOUBLE)
                                                    target:self
                                                  selector:@selector(FetchTelemetryData)
                                                  userInfo:nil
                                                   repeats:YES];
}

- (void)OnStartFailedWithNSError:(NSError* _Nullable)error {
  std::string err_msg = SysNSStringToUTF8([error localizedDescription]);
  [self OnStartFailed:err_msg];
}

- (void)OnStartFailed:(std::string)error_msg {
  state_ = START_FAILED;
  error_msg_ = error_msg;  // required by viewController
  effective_port_ = -1;

  YassWindowController* windowController = [self getWindowController];
  [windowController StartFailed];
  NSAlert* alert = [[NSAlert alloc] init];
  alert.messageText = NSLocalizedString(@"START_FAILED", @"Start Failed");
  alert.informativeText = @(error_msg.c_str());
  alert.icon = [NSImage imageNamed:NSImageNameCaution];
  alert.alertStyle = NSAlertStyleWarning;
  [alert runModal];
}

- (void)OnStopping {
  state_ = STOPPING;
  effective_port_ = -1;

  YassWindowController* windowController = [self getWindowController];
  [windowController Stopping];
}

- (void)OnStopped {
  state_ = STOPPED;
  effective_port_ = -1;

  [refresh_timer_ invalidate];
  refresh_timer_ = nil;

  finalizeTelemetryObserver(&telemetry_observer_);

  YassWindowController* windowController = [self getWindowController];
  [windowController Stopped];
}

#else

- (void)OnStart:(BOOL)quiet {
  [self OnStarting];

  if (!connectedToNetwork()) {
    NSString* message = NSLocalizedString(@"NETWORK_UNREACHABLE", @"Network unreachable");
    [self OnStartFailed:SysNSStringToUTF8(message)];
    return;
  }
  auto err_msg = [self SaveConfig];
  if (!err_msg.empty()) {
    [self OnStartFailed:err_msg];
    return;
  }

  absl::AnyInvocable<void(asio::error_code)> callback;
  callback = [=](asio::error_code ec) {
    bool successed = false;
    std::string msg;

    if (ec) {
      if (ec.category() == asio::error::get_netdb_category() || ec.category() == asio::error::get_addrinfo_category()) {
        NSError* urlError = [[NSError alloc] initWithDomain:NSURLErrorDomain
                                                       code:NSURLErrorCannotFindHost
                                                   userInfo:nil];
        msg = SysNSStringToUTF8((urlError.localizedDescription));
        NSLog(@"Translated URL error: %@", urlError.localizedDescription);
      } else if (ec.category() == asio::error::get_system_category()) {
        NSError* posixError = [[NSError alloc] initWithDomain:NSPOSIXErrorDomain code:ec.value() userInfo:nil];
        msg = SysNSStringToUTF8((posixError.localizedDescription));
        NSLog(@"Translated POSIX error: %@", posixError.localizedDescription);
      } else {
        msg = ec.message();
      }
      successed = false;
    } else {
      successed = true;
    }

    dispatch_async(dispatch_get_main_queue(), ^{
      if (successed) {
        [self OnStarted];
      } else {
        [self OnStartFailed:msg];
      }
    });
  };
  worker_.Start(std::move(callback));
}

- (void)OnStop:(BOOL)quiet {
  [self OnStopping];

  absl::AnyInvocable<void()> callback;
  if (!quiet) {
    callback = [=]() {
      dispatch_async(dispatch_get_main_queue(), ^{
        [self OnStopped];
      });
    };
  }
  worker_.Stop(std::move(callback));
}

- (void)FetchTelemetryData {
  uint64_t total_rx_bytes = net::cli::total_rx_bytes;
  uint64_t total_tx_bytes = net::cli::total_tx_bytes;
  dispatch_async(dispatch_get_main_queue(), ^{
    [self UpdateTelemetryObserver:total_rx_bytes with:total_tx_bytes];
  });
}

- (void)UpdateTelemetryObserver:(uint64_t)total_rx_bytes with:(uint64_t)total_tx_bytes {
  if ([self getState] != STARTED) {
    finalizeTelemetryObserver(&telemetry_observer_);
  } else {
    updateTelemetryObserver(&telemetry_observer_, total_rx_bytes, total_tx_bytes);
  }
  YassWindowController* windowController = [self getWindowController];
  [windowController UpdateStatusBar];

#ifndef NDEBUG
  NSLog(@"status message: %@", [self getStatusMessage]);
#endif
}

- (void)OnStarting {
  state_ = STARTING;
  effective_port_ = -1;

  YassWindowController* windowController = [self getWindowController];
  [windowController Starting];
}

- (void)OnStarted {
  state_ = STARTED;
  effective_port_ = worker_.GetLocalPort();

  config::SaveConfig();

  YassWindowController* windowController = [self getWindowController];
  [windowController Started];

  refresh_timer_ = [NSTimer scheduledTimerWithTimeInterval:NSTimeInterval(1.0/PI_DOUBLE)
                                                    target:self
                                                  selector:@selector(FetchTelemetryData)
                                                  userInfo:nil
                                                   repeats:YES];
}

- (void)OnStartFailed:(std::string)error_msg {
  state_ = START_FAILED;
  effective_port_ = -1;

  error_msg_ = error_msg;
  YassWindowController* windowController = [self getWindowController];
  [windowController StartFailed];
  NSAlert* alert = [[NSAlert alloc] init];
  alert.messageText = NSLocalizedString(@"START_FAILED", @"Start Failed");
  alert.informativeText = @(error_msg.c_str());
  alert.icon = [NSImage imageNamed:NSImageNameCaution];
  alert.alertStyle = NSAlertStyleWarning;
  [alert runModal];
}

- (void)OnStopping {
  state_ = STOPPING;
  effective_port_ = -1;

  YassWindowController* windowController = [self getWindowController];
  [windowController Stopping];
}

- (void)OnStopped {
  state_ = STOPPED;
  effective_port_ = -1;

  [refresh_timer_ invalidate];
  refresh_timer_ = nil;

  finalizeTelemetryObserver(&telemetry_observer_);

  YassWindowController* windowController = [self getWindowController];
  [windowController Stopped];
}

#endif

- (NSString*)getPaneMessage:(BOOL)isReceiveSide {
  return !isReceiveSide ? NSLocalizedString(@"TXRATE", @"tx rate:") : NSLocalizedString(@"RXRATE", @"rx rate:");
}

- (NSString*)getRateMessage:(BOOL)isReceiveSide {
  std::ostringstream ss;

  if (!isReceiveSide) {
    HumanReadableByteCountBin(&ss, telemetry_observer_.rx_rate_);
    ss << "/s";
  } else {
    HumanReadableByteCountBin(&ss, telemetry_observer_.tx_rate_);
    ss << "/s";
  }
  return SysUTF8ToNSString(ss.str());
}


- (NSString*)getStatusMessage {
  if ([self getState] != STARTED) {
    return [self getStatus];
  }
  std::ostringstream ss;
  NSString* message = [self getStatus];
  ss << SysNSStringToUTF8(message);
  message = NSLocalizedString(@"TXRATE", @"tx rate:");
  ss << " " << SysNSStringToUTF8(message) << " ";
  HumanReadableByteCountBin(&ss, telemetry_observer_.rx_rate_);
  ss << "/s";
  message = NSLocalizedString(@"RXRATE", @"rx rate:");
  ss << " " << SysNSStringToUTF8(message) << " ";
  HumanReadableByteCountBin(&ss, telemetry_observer_.tx_rate_);
  ss << "/s";

  if (effective_port_ != -1) {
    message = NSLocalizedString(@"PORT", @"port:");
    ss << " " << SysNSStringToUTF8(message) << " " << effective_port_;
  }

  return SysUTF8ToNSString(ss.str());
}

- (std::string)SaveConfig {
  YassViewController* viewController = [self getViewController];

  server_host_ = viewController.serverHost.stringValue;
  server_sni_ = viewController.serverSNI.stringValue;
  server_port_ = viewController.serverPort.stringValue;
  username_ = viewController.username.stringValue;
  password_ = viewController.password.stringValue;
  method_string_ = viewController.cipherMethod.stringValue;
  local_host_ = viewController.localHost.stringValue;
  local_port_ = viewController.localPort.stringValue;
  doh_url_ = viewController.dohURL.stringValue;
  dot_host_ = viewController.dotHost.stringValue;
  limit_rate_ = viewController.limitRate.stringValue;
  connect_timeout_ = viewController.timeout.stringValue;

  auto server_host = SysNSStringToUTF8(server_host_);
  auto server_sni = SysNSStringToUTF8(server_sni_);
  auto server_port = SysNSStringToUTF8(server_port_);
  auto username = SysNSStringToUTF8(username_);
  auto password = SysNSStringToUTF8(password_);
  auto method_string = SysNSStringToUTF8(method_string_);
  auto local_host = SysNSStringToUTF8(local_host_);
  auto local_port = SysNSStringToUTF8(local_port_);
  auto doh_url = SysNSStringToUTF8(doh_url_);
  auto dot_host = SysNSStringToUTF8(dot_host_);
  auto limit_rate = SysNSStringToUTF8(limit_rate_);
  auto connect_timeout = SysNSStringToUTF8(connect_timeout_);

  return config::ReadConfigFromArgument(server_host, server_sni, server_port, username, password, method_string,
                                        local_host, local_port, doh_url, dot_host, limit_rate, connect_timeout);
}

@end
