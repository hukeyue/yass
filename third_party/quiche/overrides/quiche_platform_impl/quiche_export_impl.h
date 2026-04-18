// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_THIRD_PARTY_QUICHE_OVERRIDES_QUICHE_PLATFORM_IMPL_QUICHE_EXPORT_IMPL_H_
#define NET_THIRD_PARTY_QUICHE_OVERRIDES_QUICHE_PLATFORM_IMPL_QUICHE_EXPORT_IMPL_H_

#include "base/component_export.h"

// These macros are documented in:
// net/third_party/quiche/src/quiche/common/platform/api/quiche_export.h

#if defined(_WIN32)
#  if defined(QUICHE_BUILD_DLL)
#     define QUICHE_EXPORT_IMPL __declspec(dllexport)
#     define QUICHE_NO_EXPORT_IMPL
#  elif defined(QUICHE_CONSUME_DLL)
#     define QUICHE_EXPORT_IMPL __declspec(dllimport)
#     define QUICHE_NO_EXPORT_IMPL
#  else
#    define QUICHE_EXPORT_IMPL
#    define QUICHE_NO_EXPORT_IMPL
#  endif
#else
#  if defined(QUICHE_BUILD_DLL)
#     define QUICHE_EXPORT_IMPL __attribute__((visibility("default")))
#     define QUICHE_NO_EXPORT_IMPL
#  elif defined(QUICHE_CONSUME_DLL)
#     define QUICHE_EXPORT_IMPL
#     define QUICHE_NO_EXPORT_IMPL
#  else
#    define QUICHE_EXPORT_IMPL
#    define QUICHE_NO_EXPORT_IMPL
#  endif
#endif  // defined(_WIN32)

#endif  // NET_THIRD_PARTY_QUICHE_OVERRIDES_QUICHE_PLATFORM_IMPL_QUICHE_EXPORT_IMPL_H_
