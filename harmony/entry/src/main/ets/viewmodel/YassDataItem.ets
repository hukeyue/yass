// SPDX-License-Identifier: GPL-2.0 OR LGPL-2.1 OR CDDL-1.0
/* Copyright (c) 2024-2026 Chilledheart  */

/**
 * Basic list item data bean.
 */
export class YassDataItem {
  serverHost: string = '';
  serverSNI: string = '';
  serverPort: number = 0;
  username: string = '';
  password: string = '';
  cipher: string = '';
  dohUrl: string = '';
  dotHost: string = '';
  limitRate: string = '';
  timeout: number = 0;
}
