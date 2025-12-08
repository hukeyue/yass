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

#ifndef YASS_CORE_COMPILER_SPECIFIC_H
#define YASS_CORE_COMPILER_SPECIFIC_H

#include <base/compiler_specific.h>
#include <build/build_config.h>

#ifdef __GNUC__
#define PACK(__Declaration__) __Declaration__ __attribute__((packed, aligned(1)))
#endif

#ifdef _MSC_VER
#define PACK(__Declaration__) __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#endif

// clang 14 doesn't recognize the newer NOINLINE definitions
#if defined(__clang__) && HAS_ATTRIBUTE(noinline) &&                      \
    ((defined(_BASE_APPLE_CLANG_VER) && _BASE_APPLE_CLANG_VER <= 1500) || \
     (defined(_BASE_CLANG_VER) && _BASE_CLANG_VER <= 1400))
#undef NOINLINE
#define NOINLINE __attribute__((noinline))
#endif

// clang 13 doesn't recognize the newer ALWAYS_INLINE definitions
#if defined(__clang__) && HAS_ATTRIBUTE(always_inline) &&                 \
    ((defined(_BASE_APPLE_CLANG_VER) && _BASE_APPLE_CLANG_VER <= 1400) || \
     (defined(_BASE_CLANG_VER) && _BASE_CLANG_VER <= 1300))
#undef ALWAYS_INLINE
#define ALWAYS_INLINE inline __attribute__((__always_inline__))
#endif

#if defined(__apple_build_version__)
// Given AppleClang XX.Y.Z, _BASE_COMPILER_CLANG_BASED is XXYZ (e.g. AppleClang 14.0.3 => 1403)
#define _BASE_COMPILER_CLANG_BASED
#define _BASE_APPLE_CLANG_VER (__apple_build_version__ / 10000)
#elif defined(__clang__)
#define _BASE_COMPILER_CLANG_BASED
#define _BASE_CLANG_VER (__clang_major__ * 100 + __clang_minor__)
#elif defined(__GNUC__)
#define _BASE_COMPILER_GCC
#define _BASE_GCC_VER (__GNUC__ * 100 + __GNUC_MINOR__)
#endif

// MemorySanitizer annotations.
#if defined(MEMORY_SANITIZER)
#define NO_SANITIZE_MEMORY NO_SANITIZE("memory")
#else  // MEMORY_SANITIZER
#define NO_SANITIZE_MEMORY
#endif  // MEMORY_SANITIZER

// Use nomerge attribute to disable optimization of merging multiple same calls.
#if defined(__clang__) && HAS_ATTRIBUTE(nomerge) &&                       \
    ((defined(_BASE_APPLE_CLANG_VER) && _BASE_APPLE_CLANG_VER >= 1300) || \
     (defined(_BASE_CLANG_VER) && _BASE_CLANG_VER >= 1200))
#undef NOMERGE
#define NOMERGE [[clang::nomerge]]
#else
#define NOMERGE
#endif

#endif  // YASS_CORE_COMPILER_SPECIFIC_H
