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

/* Copyright (c) 2023-2025 Chilledheart  */

#ifndef YASS_ANDROID_UTILS
#define YASS_ANDROID_UTILS

#ifdef __ANDROID__

#include <jni.h>
#include <string>

#include <base/files/memory_mapped_file.h>

// returning in host byte order
[[nodiscard]] int32_t GetIpAddress(JNIEnv* env);
int SetJavaThreadName(const std::string& thread_name);
int GetJavaThreadName(std::string* thread_name);
int GetNativeLibraryDirectory(JNIEnv* env, jobject activity_obj, std::string* result);
int GetCacheLibraryDirectory(JNIEnv* env, jobject activity_obj, std::string* result);
int GetDataLibraryDirectory(JNIEnv* env, jobject activity_obj, std::string* result);
int GetCurrentLocale(JNIEnv* env, jobject activity_obj, std::string* result);
int OpenApkAsset(const std::string& file_path, gurl_base::MemoryMappedFile::Region* region);
#ifdef HAVE_C_ARES
int InitializeCares(JNIEnv* env, jobject activity_obj);
#endif  // HAVE_C_ARES

#endif  // __ANDROID__

#endif  //  YASS_ANDROID_UTILS
