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

#include "support/utils.hpp"

#include <build/build_config.h>

#include "config/config.hpp"
#include "core/logging.hpp"

#ifdef HAVE_TCMALLOC
#include <gperftools/malloc_extension_c.h>
#endif

#ifdef HAVE_MIMALLOC
#include <mimalloc.h>
#endif

#ifdef HAVE_JEMALLOC
#include <jemalloc/jemalloc.h>
#endif

#if defined(ADDRESS_SANITIZER) || defined(THREAD_SANITIZER) || defined(MEMORY_SANITIZER)
#include <sanitizer/allocator_interface.h>
#endif

#if defined(HAVE_MALLINFO) || defined(HAVE_MALLINFO2) || BUILDFLAG(IS_FREEBSD)
#include <malloc.h>
#endif

void PrintMallocStats() {
#if defined(HAVE_TBBMALLOC)
  LOG(ERROR) << "TBBMALLOC: report is not supported";
#elif defined(HAVE_TCMALLOC)
  // clang-format off
  constexpr const char* properties[] = {
    /* generic */
      "generic.current_allocated_bytes",
      "generic.heap_size",
      "generic.total_physical_bytes",
    /* tcmalloc-specified */
      "tcmalloc.max_total_thread_cache_bytes",
      "tcmalloc.min_per_thread_cache_bytes",
      "tcmalloc.current_total_thread_cache_bytes",
      "tcmalloc.central_cache_free_bytes",
      "tcmalloc.transfer_cache_free_bytes",
      "tcmalloc.thread_cache_free_bytes",
      "tcmalloc.pageheap_free_bytes",
      "tcmalloc.pageheap_unmapped_bytes",
    /* undocumented */
      "tcmalloc.pageheap_committed_bytes",
      "tcmalloc.pageheap_scavenge_count",
      "tcmalloc.pageheap_commit_count",
      "tcmalloc.pageheap_total_commit_bytes",
      "tcmalloc.pageheap_decommit_count",
      "tcmalloc.pageheap_total_decommit_bytes",
      "tcmalloc.pageheap_reserve_count",
      "tcmalloc.pageheap_total_reserve_bytes",
      "tcmalloc.aggressive_memory_decommit",
      "tcmalloc.heap_limit_mb",
      "tcmalloc.impl.thread_cache_count",
  };
  // clang-format on
  for (auto property : properties) {
    size_t size;
    if (MallocExtension_GetNumericProperty(property, &size)) {
      if (std::string_view(property).ends_with("_bytes")) {
        LOG(ERROR) << "TCMALLOC: " << property << " = " << size << " bytes";
      } else {
        LOG(ERROR) << "TCMALLOC: " << property << " = " << size;
      }
    }
  }
#elif defined(HAVE_MIMALLOC)
  auto printer = [](const char* msg, void* arg) { LOG(ERROR) << "MIMALLOC: " << msg; };
  mi_stats_print_out(printer, nullptr);
#elif defined(HAVE_JEMALLOC)
  auto printer = [](void* arg, const char* msg) { LOG(ERROR) << "JEMALLOC: " << msg; };
  malloc_stats_print(printer, nullptr, nullptr);
#elif defined(ADDRESS_SANITIZER) || defined(THREAD_SANITIZER) || defined(MEMORY_SANITIZER)
  LOG(ERROR) << "SANITIZER: current allocated: " << __sanitizer_get_current_allocated_bytes() << " bytes";
  LOG(ERROR) << "SANITIZER: heap size: " << __sanitizer_get_heap_size() << " bytes";
  LOG(ERROR) << "SANITIZER: free size: " << __sanitizer_get_free_bytes() << " bytes";
  LOG(ERROR) << "SANITIZER: unmap size: " << __sanitizer_get_unmapped_bytes() << " bytes";
#elif defined(HAVE_MALLINFO2) && !defined(MEMORY_SANITIZER)
  struct mallinfo2 info = mallinfo2();
  LOG(ERROR) << "MALLOC: non-mmapped space allocated from system: " << info.arena;
  LOG(ERROR) << "MALLOC: number of free chunks: " << info.ordblks;
  LOG(ERROR) << "MALLOC: number of fastbin blocks: " << info.smblks;
  LOG(ERROR) << "MALLOC: number of mmapped regions: " << info.hblks;
  LOG(ERROR) << "MALLOC: space in mmapped regions: " << info.hblkhd;
  LOG(ERROR) << "MALLOC: space available in freed fastbin blocks: " << info.fsmblks;
  LOG(ERROR) << "MALLOC: total allocated space: " << info.uordblks;
  LOG(ERROR) << "MALLOC: total free space: " << info.fordblks;
  LOG(ERROR) << "MALLOC: top-most, releasable (via malloc_trim) space: " << info.keepcost;
#elif defined(HAVE_MALLINFO) && !defined(MEMORY_SANITIZER)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  struct mallinfo info = mallinfo();
#pragma GCC diagnostic pop
  LOG(ERROR) << "MALLOC: non-mmapped space allocated from system: " << info.arena;
  LOG(ERROR) << "MALLOC: number of free chunks: " << info.ordblks;
  LOG(ERROR) << "MALLOC: number of fastbin blocks: " << info.smblks;
  LOG(ERROR) << "MALLOC: number of mmapped regions: " << info.hblks;
  LOG(ERROR) << "MALLOC: space in mmapped regions: " << info.hblkhd;
  LOG(ERROR) << "MALLOC: space available in freed fastbin blocks: " << info.fsmblks;
  LOG(ERROR) << "MALLOC: total allocated space: " << info.uordblks;
  LOG(ERROR) << "MALLOC: total free space: " << info.fordblks;
  LOG(ERROR) << "MALLOC: top-most, releasable (via malloc_trim) space: " << info.keepcost;
#elif BUILDFLAG(IS_FREEBSD)
  auto printer = [](void* data, const char* msg) { LOG(ERROR) << "MALLOC: " << msg; };
  malloc_stats_print(printer, nullptr, nullptr);
#endif
}
