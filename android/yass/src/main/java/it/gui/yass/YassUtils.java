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

package it.gui.yass;

public class YassUtils {

    public static native String getServerHost();

    public static native String getServerSNI();

    public static native int getServerPort();

    public static native String getUsername();

    public static native String getPassword();

    public static native int getCipher();

    public static native String[] getCipherStrings();

    public static native String getDoHUrl();

    public static native String getDoTHost();

    public static native String getLimitRate();

    public static native int getTimeout();

    public static native String saveConfig(String serverHost, String serverSNI, String serverPort,
                                     String username, String password, int cipher, String doh_url,
                                     String dot_host, String limit_rate, String timeout);

    public static native void setEnablePostQuantumKyber(boolean enable_post_quantum_kyber);
}
