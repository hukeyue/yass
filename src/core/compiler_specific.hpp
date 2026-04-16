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

// Annotates a function or class data member indicating it can lead to
// out-of-bounds accesses (OOB) if given incorrect inputs.
//
// For functions, this commonly includes functions which take pointers, sizes,
// iterators, sentinels, etc. and cannot fully check their preconditions (e.g.
// that the provided pointer actually points to an allocation of at least the
// provided size). Useful to diagnose potential misuse via
// `-Wunsafe-buffer-usage`, as well as to mark functions potentially in need of
// safer alternatives.
//
// For fields, this would be used to annotate both pointer and size fields that
// have not yet been converted to a span.
//
// All functions or fields annotated with this macro should come with a
// `// PRECONDITIONS: ` comment that explains what the caller must guarantee
// to ensure safe operation. Callers can then write `// SAFETY: ` comments
// explaining why the specific preconditions have been met.
//
// Ideally, unsafe functions should also be paired with a safer version, e.g.
// one that replaces pointer parameters with `span`s; otherwise, document safer
// replacement coding patterns callers can migrate to.
//
// Annotating a function `UNSAFE_BUFFER_USAGE` means all call sites (that do not
// disable the warning) must wrap calls in `UNSAFE_BUFFERS()`; see documentation
// there. Annotating a field `UNSAFE_BUFFER_USAGE` means that `UNSAFE_BUFFERS()`
// must wrap expressions that mutate of the field.
//
// See also:
//   https://chromium.googlesource.com/chromium/src/+/main/docs/unsafe_buffers.md
//   https://clang.llvm.org/docs/SafeBuffers.html
//   https://clang.llvm.org/docs/DiagnosticsReference.html#wunsafe-buffer-usage
//
// Usage:
// ```
//   // Calls to this function must be wrapped in `UNSAFE_BUFFERS()`.
//   UNSAFE_BUFFER_USAGE void Func(T* input, T* end);
//
//   struct S {
//     // Changing this pointer requires `UNSAFE_BUFFERS()`.
//     UNSAFE_BUFFER_USAGE int* p;
//   };
// ```
#if __has_cpp_attribute(clang::unsafe_buffer_usage)
#define UNSAFE_BUFFER_USAGE [[clang::unsafe_buffer_usage]]
#else
#define UNSAFE_BUFFER_USAGE
#endif

// Annotates code indicating that it should be permanently exempted from
// `-Wunsafe-buffer-usage`. For temporary cases such as migrating callers to
// safer patterns, use `UNSAFE_TODO()` instead; see documentation there.
//
// All calls to functions annotated with `UNSAFE_BUFFER_USAGE` must be marked
// with one of these two macros; they can also be used around pointer
// arithmetic, pointer subscripting, and the like.
//
// ** USE OF THIS MACRO SHOULD BE VERY RARE.** Using this macro indicates that
// the compiler cannot verify that the code avoids OOB, and manual review is
// required. Even with manual review, it's easy for assumptions to change and
// security bugs to creep in over time. Prefer safer patterns instead.
//
// Usage should wrap the minimum necessary code, and *must* include a
// `// SAFETY: ...` comment that explains how the code guarantees safety or
// meets the requirements of called `UNSAFE_BUFFER_USAGE` functions. Guarantees
// must be manually verifiable by the Chrome security team using only local
// invariants; contact security@chromium.org to schedule such a review. Valid
// invariants include:
// - Runtime conditions or `CHECK()`s nearby
// - Invariants guaranteed by types in the surrounding code
// - Invariants guaranteed by function calls in the surrounding code
// - Caller requirements, if the containing function is itself annotated with
//   `UNSAFE_BUFFER_USAGE`; this is less safe and should be a last resort
//
// See also:
//   https://chromium.googlesource.com/chromium/src/+/main/docs/unsafe_buffers.md
//   https://clang.llvm.org/docs/SafeBuffers.html
//   https://clang.llvm.org/docs/DiagnosticsReference.html#wunsafe-buffer-usage
//
// Usage:
// ```
//   // The following call will not trigger a compiler warning even if `Func()`
//   // is annotated `UNSAFE_BUFFER_USAGE`.
//   return UNSAFE_BUFFERS(Func(input, end));
// ```
//
// Test for `__clang__` directly, as there's no `__has_pragma` or similar (see
// https://github.com/llvm/llvm-project/issues/51887).
#if defined(__clang__)
// Disabling `clang-format` allows each `_Pragma` to be on its own line, as
// recommended by https://gcc.gnu.org/onlinedocs/cpp/Pragmas.html.
// clang-format off
#define UNSAFE_BUFFERS(...)                  \
  _Pragma("clang unsafe_buffer_usage begin") \
  __VA_ARGS__                                \
  _Pragma("clang unsafe_buffer_usage end")
// clang-format on
#else
#define UNSAFE_BUFFERS(...) __VA_ARGS__
#endif

#endif  // YASS_CORE_COMPILER_SPECIFIC_H
