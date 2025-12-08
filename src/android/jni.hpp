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

#ifndef YASS_ANDROID_JNI_HPP
#define YASS_ANDROID_JNI_HPP

#ifdef __ANDROID__

#include <jni.h>

extern JavaVM* g_jvm;
extern jobject g_activity_obj;

extern "C" JNIEXPORT jobject JNICALL Java_it_gui_yass_YassUtils_getServerHost(JNIEnv* env, jobject obj);
extern "C" JNIEXPORT jobject JNICALL Java_it_gui_yass_YassUtils_getServerSNI(JNIEnv* env, jobject obj);
extern "C" JNIEXPORT jint JNICALL Java_it_gui_yass_YassUtils_getServerPort(JNIEnv* env, jobject obj);
extern "C" JNIEXPORT jobject JNICALL Java_it_gui_yass_YassUtils_getUsername(JNIEnv* env, jobject obj);
extern "C" JNIEXPORT jobject JNICALL Java_it_gui_yass_YassUtils_getPassword(JNIEnv* env, jobject obj);
extern "C" JNIEXPORT jint JNICALL Java_it_gui_yass_YassUtils_getCipher(JNIEnv* env, jobject obj);
extern "C" JNIEXPORT jobjectArray JNICALL Java_it_gui_yass_YassUtils_getCipherStrings(JNIEnv* env, jobject obj);
extern "C" JNIEXPORT jobject JNICALL Java_it_gui_yass_YassUtils_getDoHUrl(JNIEnv* env, jobject obj);
extern "C" JNIEXPORT jobject JNICALL Java_it_gui_yass_YassUtils_getDoTHost(JNIEnv* env, jobject obj);
extern "C" JNIEXPORT jobject JNICALL Java_it_gui_yass_YassUtils_getLimitRate(JNIEnv* env, jobject obj);
extern "C" JNIEXPORT jint JNICALL Java_it_gui_yass_YassUtils_getTimeout(JNIEnv* env, jobject obj);

extern "C" JNIEXPORT jobject JNICALL Java_it_gui_yass_YassUtils_saveConfig(JNIEnv* env,
                                                                           jobject obj,
                                                                           jobject server_host,
                                                                           jobject server_sni,
                                                                           jobject server_port,
                                                                           jobject username,
                                                                           jobject password,
                                                                           jint method_idx,
                                                                           jobject doh_url,
                                                                           jobject dot_host,
                                                                           jobject limit_rate,
                                                                           jobject connect_timeout);

extern "C" JNIEXPORT void JNICALL
Java_it_gui_yass_YassUtils_setEnablePostQuantumKyber(JNIEnv* env, jobject obj, jboolean enable_post_quantum_kyber);

#endif  // __ANDROID__

#endif  // YASS_ANDROID_JNI_HPP
