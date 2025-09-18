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

#include "qt6/tray_icon.hpp"

#include <QAction>
#include <QMenu>

#include "qt6/option_dialog.hpp"
#include "qt6/yass.hpp"
#include "qt6/yass_window.hpp"

TrayIcon::TrayIcon(QObject* parent) : QSystemTrayIcon(parent) {
  setIcon(QIcon(":/res/images/io.github.chilledheart.yass.png"));

  // create action
  QAction* show_action = new QAction(tr("&Show"), this);
  connect(show_action, &QAction::triggered, this, &TrayIcon::OnShow);

  QAction* option_action = new QAction(tr("&Option..."), this);
  connect(option_action, &QAction::triggered, this, &TrayIcon::OnOption);

  QAction* exit_action = new QAction(tr("E&xit"), this);
  connect(exit_action, &QAction::triggered, this, [&]() { App()->quit(); });

  QMenu* menu = new QMenu;
  menu->addAction(show_action);
  menu->addAction(option_action);
  menu->addSeparator();
  menu->addAction(exit_action);

  setContextMenu(menu);

  // connect signal
  connect(this, &TrayIcon::activated, this, &TrayIcon::OnActivated);
}

void TrayIcon::OnActivated(QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
    case QSystemTrayIcon::Trigger:  // single click
    case QSystemTrayIcon::MiddleClick:
    case QSystemTrayIcon::DoubleClick:
      OnShow();
      break;
    default:
      break;
  }
}

void TrayIcon::OnShow() {
  App()->mainWindow()->showWindow();
}

void TrayIcon::OnOption() {
  App()->mainWindow()->showWindow();
  OptionDialog dialog(App()->mainWindow());
  dialog.exec();
}
