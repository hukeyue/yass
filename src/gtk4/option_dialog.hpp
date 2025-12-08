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

/* Copyright (c) 2021-2025 Chilledheart  */

#ifndef OPTION_DIALOG_H
#define OPTION_DIALOG_H

#include <gtk/gtk.h>

#include <string>
#include <vector>

extern "C" {
#define OPTION_DIALOG_TYPE (option_dialog_get_type())
#define OPTION_DIALOG(dialog) (G_TYPE_CHECK_INSTANCE_CAST((dialog), OPTION_DIALOG_TYPE, OptionGtkDialog))
G_DECLARE_FINAL_TYPE(OptionGtkDialog, option_dialog, OPTIONGtk, DIALOG, GtkDialog)

OptionGtkDialog* option_dialog_new(const gchar* title, GtkWindow* parent, GtkDialogFlags flags);
}

class OptionDialog {
 public:
  explicit OptionDialog(const std::string& title, GtkWindow* parent, bool modal = false);
  ~OptionDialog();

  void OnOkayButtonClicked();
  void OnCancelButtonClicked();

  void run();

 private:
  void LoadChanges();
  bool OnSave();

 private:
  friend class YASSWindow;
  OptionGtkDialog* impl_;
  std::vector<std::string> algorithms_;
};  // OptionDialog

#endif  // OPTION_DIALOG_H
