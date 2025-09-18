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

package it.gui.yass;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.ProxyInfo;
import android.net.VpnService;
import android.os.Build;
import android.os.ParcelFileDescriptor;
import android.util.Log;

public class YassVpnService extends VpnService {
    private static final String TAG = "YassVpnService";
    public static final int DEFAULT_MTU = 1500;
    private static final String PRIVATE_VLAN4_CLIENT = "172.19.0.1";
    private static final String PRIVATE_VLAN4_GATEWAY = "172.19.0.2";
    private static final String PRIVATE_VLAN6_CLIENT = "fdfe:dcba:9876::1";
    private static final String PRIVATE_VLAN6_GATEWAY = "fdfe:dcba:9876::2";

    public ParcelFileDescriptor connect(String session_name, Context context, int local_port) {
        Builder builder = new Builder();

        builder.setConfigureIntent(PendingIntent.getActivity(context, 0,
                new Intent(context, MainActivity.class).setFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT),
                PendingIntent.FLAG_IMMUTABLE));
        builder.setSession(session_name);
        builder.setMtu(DEFAULT_MTU);
        builder.addAddress(PRIVATE_VLAN4_CLIENT, 30);
        builder.addAddress(PRIVATE_VLAN6_CLIENT, 126);
        builder.addRoute("0.0.0.0", 0);
        builder.addRoute("::", 0);
        // builder.setUnderlyingNetworks(underlyingNetworks);
        // builder.setMetered(metered);
        builder.addDnsServer(PRIVATE_VLAN4_GATEWAY);
        builder.addDnsServer(PRIVATE_VLAN6_GATEWAY);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            builder.setHttpProxy(ProxyInfo.buildDirectProxy("127.0.0.1", local_port));
        }
        try {
            builder.addDisallowedApplication(context.getPackageName());
        } catch (PackageManager.NameNotFoundException e) {
            Log.e(TAG, "Cannot add self to disallowed package list");
            // nop
        }
        ParcelFileDescriptor tunFd = null;
        try {
            tunFd = builder.establish();
        } catch (Exception c) {
            // nop
        }
        return tunFd;
    }

    // might not be invoked in main thread
    @Override
    public void onRevoke() {
        Log.e(TAG, "onRevoke");
        MainActivity.self.runOnUiThread(() -> MainActivity.self.onStopVpn());
        // super.onRevoke() is invoked in onStopVpn
    }
}
