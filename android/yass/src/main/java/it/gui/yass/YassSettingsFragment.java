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

import android.os.Bundle;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.util.Log;

import androidx.annotation.Nullable;

// TODO use PreferenceFragementCompact once use minimum sdk version 28
// https://developer.android.com/develop/ui/views/components/settings#java
public class YassSettingsFragment extends PreferenceFragment {
    public static String EnablePostQuantumKyberPreferenceKey = "enable_post_quantum_kyber";

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // below line is used to add preference
        // fragment from our xml folder.
        addPreferencesFromResource(R.xml.preferences);

        Preference enablePostQuantumKyberPref = findPreference(EnablePostQuantumKyberPreferenceKey);

        if (enablePostQuantumKyberPref != null) {
            enablePostQuantumKyberPref.setOnPreferenceChangeListener((preference, newValue) -> {
                Log.d("Preferences", String.format("Setting Preferences: %s -> %s", EnablePostQuantumKyberPreferenceKey, newValue));
                YassUtils.setEnablePostQuantumKyber((Boolean) newValue);
                return true; // Return true if the event is handled.
            });
        }
    }
}
