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

/* Copyright (c) 2019-2025 Chilledheart  */

#ifndef YASS_WINDOW_H
#define YASS_WINDOW_H

#include <gtk/gtk.h>

#include <string>

#include "gtk4/yass.hpp"

extern "C" {
#define YASS_WINDOW_TYPE (yass_window_get_type())
#define YASS_WINDOW(window) (G_TYPE_CHECK_INSTANCE_CAST((window), YASS_WINDOW_TYPE, YASSGtkWindow))
G_DECLARE_FINAL_TYPE(YASSGtkWindow, yass_window, YASSGtk, WINDOW, GtkApplicationWindow)

YASSGtkWindow* yass_window_new(YASSGtkApp* app);
}

class OptionDialog;
class YASSWindow {
 public:
  YASSWindow(GApplication* app);
  ~YASSWindow();

  YASSGtkWindow* impl() { return impl_; }

 private:
  GApplication* app_;
  YASSGtkWindow* impl_;
  bool close_requested_ = false;
  std::string last_status_msg_;

 public:
  void show();
  void present();
  void close();

  void OnStartButtonClicked();
  void OnStopButtonClicked();
  void OnAutoStartClicked();
  void OnSystemProxyClicked();

 public:
  void OnAbout();
  void OnOption();

  void OnAboutDialogClose();
  void OnOptionDialogClose();

 private:
  GtkAboutDialog* about_dialog_ = nullptr;
  OptionDialog* option_dialog_ = nullptr;

 public:
  std::string GetServerHost();
  std::string GetServerSNI();
  std::string GetServerPort();
  std::string GetUsername();
  std::string GetPassword();
  std::string GetMethod();
  std::string GetLocalHost();
  std::string GetLocalPort();
  std::string GetDoHUrl();
  std::string GetDoTHost();
  std::string GetLimitRate();
  std::string GetTimeout();
  std::string GetStatusMessage();

  void Started();
  void Stopped();
  void StartFailed();

  void LoadChanges();
  void UpdateStatusBar();

 private:
  void OnClose();

  friend class YASSApp;

 private:
  uint64_t last_sync_time_ = 0;
  uint64_t last_rx_bytes_ = 0;
  uint64_t last_tx_bytes_ = 0;
  uint64_t rx_rate_ = 0;
  uint64_t tx_rate_ = 0;
};

#endif  // YASS_WINDOW_H
