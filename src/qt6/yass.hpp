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

/* Copyright (c) 2024-2025 Chilledheart  */

#ifndef _YASS_QT6_YASS_HPP
#define _YASS_QT6_YASS_HPP

#include <QApplication>
#ifndef _WIN32
#include <QSocketNotifier>
#else
#include <QSessionManager>
#endif

#include "cli/cli_worker.hpp"

class TrayIcon;
class QTimer;
class QTranslator;
class YASSWindow;
class YASSApp : public QApplication {
  Q_OBJECT
 public:
  YASSApp(int& argc, char** argv);
  ~YASSApp();

  bool Init();

 private slots:
  void OnIdle();

 public:
  void OnStart(bool quiet = false);
  void OnStop(bool quiet = false);

  std::string GetStatus() const;
  enum YASSState { STARTED, STARTING, START_FAILED, STOPPING, STOPPED, MAX_STATE };
  YASSState GetState() const { return state_; }

  YASSWindow* mainWindow() { return main_window_.get(); }

 signals:
  void OnStartedSignal();
  void OnStartFailedSignal(const std::string& error_msg);
  void OnStoppedSignal();

 private slots:
  void OnQuit();
  void OnStarted();
  void OnStartFailed(const std::string& error_msg);
  void OnStopped();

 private:
  std::string SaveConfig();

#ifndef _WIN32
 public:
  static void SigIntSignalHandler(int s);

 private:
  static int sigintFd[2];
  QSocketNotifier* snInt = nullptr;

 private slots:
  void ProcessSigInt();
#else
 private slots:
  void commitData(QSessionManager& manager);
#endif

 private:
  QTimer* idle_timer_;
  QTranslator* qt_translator_;
  QTranslator* my_translator_;

 private:
  YASSState state_ = STOPPED;

  friend class YASSWindow;
  std::unique_ptr<YASSWindow> main_window_;
  TrayIcon* tray_icon_ = nullptr;

  std::unique_ptr<Worker> worker_;
  std::string error_msg_;
};

inline YASSApp* App() {
  return static_cast<YASSApp*>(qApp);
}

#endif  //  _YASS_QT6_YASS_HPP
