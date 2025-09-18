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

#ifndef H_CONFIG_CONFIG_IMPL_APPLE
#define H_CONFIG_CONFIG_IMPL_APPLE

#include "config/config_impl.hpp"

#if defined(__APPLE__)

#include <string>
#include <string_view>

#include <CoreFoundation/CoreFoundation.h>
#include <base/apple/scoped_cftyperef.h>

namespace config {

class ConfigImplApple : public ConfigImpl {
 public:
  ~ConfigImplApple() override {}

 protected:
  bool OpenImpl(bool dontread) override;

  bool CloseImpl() override;

  bool HasKeyStringImpl(const std::string& key) override;
  bool HasKeyBoolImpl(const std::string& key) override;
  bool HasKeyUint32Impl(const std::string& key) override;
  bool HasKeyUint64Impl(const std::string& key) override;
  bool HasKeyInt32Impl(const std::string& key) override;
  bool HasKeyInt64Impl(const std::string& key) override;

  bool ReadImpl(const std::string& key, std::string* value) override;
  bool ReadImpl(const std::string& key, bool* value) override;
  bool ReadImpl(const std::string& key, uint32_t* value) override;
  bool ReadImpl(const std::string& key, int32_t* value) override;
  bool ReadImpl(const std::string& key, uint64_t* value) override;
  bool ReadImpl(const std::string& key, int64_t* value) override;

  bool WriteImpl(const std::string& key, std::string_view value) override;
  bool WriteImpl(const std::string& key, bool value) override;
  bool WriteImpl(const std::string& key, uint32_t value) override;
  bool WriteImpl(const std::string& key, int32_t value) override;
  bool WriteImpl(const std::string& key, uint64_t value) override;
  bool WriteImpl(const std::string& key, int64_t value) override;

  bool DeleteImpl(const std::string& key) override;

 private:
  gurl_base::apple::ScopedCFTypeRef<CFDictionaryRef> root_;
  gurl_base::apple::ScopedCFTypeRef<CFMutableDictionaryRef> write_root_;
};

}  // namespace config

#endif  // defined(__APPLE__)
#endif  // H_CONFIG_CONFIG_IMPL_POSIX
