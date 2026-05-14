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

#include "core/utils.hpp"

#include <build/build_config.h>

#include "core/logging.hpp"

#if BUILDFLAG(IS_FREEBSD)

#include <locale.h>
#include <pthread_np.h>
#include <time.h>  // For clock_gettime

#include <mutex>

// TBD
bool SetCurrentThreadPriority(ThreadPriority /*priority*/) {
  return true;
}

bool SetCurrentThreadName(const std::string& name) {
  pthread_set_name_np(pthread_self(), name.c_str());
  return true;
}

bool SetCurrentThreadAffinityToCpu(int cpuid) {
  int ret;
  auto self = pthread_self();
  cpu_set_t affinity, previous_affinity;
  ret = pthread_getaffinity_np(self, sizeof(previous_affinity), &previous_affinity);
  if (ret != 0) {
    return false;
  }
  memcpy(&affinity, &previous_affinity, sizeof(affinity));
  int j = -1;
  for (int i = 0; i < CPU_SETSIZE; ++i) {
    if (CPU_ISSET(i, &affinity)) {
      ++j;
      if (j != cpuid) {
        CPU_CLR(i, &affinity);
      }
    }
  }
  if (j == -1) {
    errno = EINVAL;
    return false;
  }
  DCHECK_EQ(1, CPU_COUNT(&affinity));
  if (CPU_COUNT(&affinity) == 0) {
    errno = EINVAL;
    return false;
  }
  ret = pthread_setaffinity_np(self, sizeof(affinity), &affinity);
  if (ret != 0) {
    return false;
  }
  return true;
}

uint64_t GetMonotonicTime() {
  static struct timespec start_ts;
  static std::once_flag started_flag;
  struct timespec ts;
  int ret;
  std::call_once(started_flag, [](){
    int ret = clock_gettime(CLOCK_MONOTONIC, &start_ts);
    if (ret < 0) {
      RAW_LOG(FATAL, "clock_gettime failed");
    }
  });
  // Activity to be timed

  ret = clock_gettime(CLOCK_MONOTONIC, &ts);
  if (ret < 0) {
    RAW_LOG(FATAL, "clock_gettime failed");
    return 0;
  }
  ts.tv_sec -= start_ts.tv_sec;
  ts.tv_nsec -= start_ts.tv_nsec;
  return static_cast<uint64_t>(ts.tv_sec) * NS_PER_SECOND + ts.tv_nsec;
}

#endif  // BUILDFLAG(IS_FREEBSD)
