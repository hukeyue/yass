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

#ifndef OPTION_DIALOG
#define OPTION_DIALOG

#include <gtk/gtk.h>

#include <string>
#include <vector>

class OptionDialog {
 public:
  explicit OptionDialog(const std::string& title, GtkWindow* parent, bool modal = false);
  ~OptionDialog();

  void OnOkayButtonClicked();
  void OnCancelButtonClicked();

  gint run();

 private:
  void LoadChanges();
  bool OnSave();

  GtkCheckButton* tcp_keep_alive_;
  GtkEntry* tcp_keep_alive_cnt_;
  GtkEntry* tcp_keep_alive_idle_timeout_;
  GtkEntry* tcp_keep_alive_interval_;
  GtkCheckButton* enable_post_quantum_kyber_;
  GtkComboBoxText* tcp_congestion_algorithm_;
  std::vector<std::string> algorithms_;

  GtkButton* okay_button_;
  GtkButton* cancel_button_;

 private:
  GtkDialog* impl_;
};  // OptionDialog

#endif  // OPTION_DIALOG
