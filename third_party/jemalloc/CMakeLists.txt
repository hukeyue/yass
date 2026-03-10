set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

set(jemalloc_C_HDRS
  include/jemalloc/jemalloc.h)

set(jemalloc_C_SRCS
  src/src/jemalloc.c
  src/src/arena.c
  src/src/background_thread.c
  src/src/base.c
  src/src/batcher.c
  src/src/bin.c
  src/src/bin_info.c
  src/src/bitmap.c
  src/src/buf_writer.c
  src/src/cache_bin.c
  src/src/ckh.c
  src/src/counter.c
  src/src/ctl.c
  src/src/decay.c
  src/src/div.c
  src/src/ecache.c
  src/src/edata.c
  src/src/edata_cache.c
  src/src/ehooks.c
  src/src/emap.c
  src/src/eset.c
  src/src/exp_grow.c
  src/src/extent.c
  src/src/extent_dss.c
  src/src/extent_mmap.c
  src/src/fxp.c
  src/src/san.c
  src/src/san_bump.c
  src/src/hook.c
  src/src/hpa.c
  src/src/hpa_hooks.c
  src/src/hpdata.c
  src/src/inspect.c
  src/src/large.c
  src/src/log.c
  src/src/malloc_io.c
  src/src/mutex.c
  src/src/nstime.c
  src/src/pa.c
  src/src/pa_extra.c
  src/src/pai.c
  src/src/pac.c
  src/src/pages.c
  src/src/peak_event.c
  src/src/prof.c
  src/src/prof_data.c
  src/src/prof_log.c
  src/src/prof_recent.c
  src/src/prof_stack_range.c
  src/src/prof_stats.c
  src/src/prof_sys.c
  src/src/prof_threshold.c
  src/src/psset.c
  src/src/rtree.c
  src/src/safety_check.c
  src/src/sc.c
  src/src/sec.c
  src/src/stats.c
  src/src/sz.c
  src/src/tcache.c
  src/src/test_hooks.c
  src/src/thread_event.c
  src/src/ticker.c
  src/src/tsd.c
  src/src/util.c
  src/src/witness.c
)

if (APPLE)
  set(jemalloc_C_SRCS
    ${jemalloc_C_SRCS}
    src/src/zone.c)
endif()

set(jemalloc_version_major 5)
set(jemalloc_version_minor 3)
set(jemalloc_version_bugfix 0)
set(jemalloc_version_nrev 298)
set(jemalloc_version_gid 1972241cd204c60fb5b66f23c48a117879636161)
set(jemalloc_version 5.3.0-298-g1972241cd204c60fb5b66f23c48a117879636161)
set(je_ je_)
set(install_suffix)
set(private_namespace je_)

include(CheckTypeSize)
check_type_size(int CMAKE_SIZEOF_INT LANGUAGE C)
check_type_size(long CMAKE_SIZEOF_LONG LANGUAGE C)
check_type_size("long long" CMAKE_SIZEOF_LONGLONG LANGUAGE C)
check_type_size(intmax_t CMAKE_SIZEOF_INTMAX_T LANGUAGE C)

if (CMAKE_SIZEOF_INT EQUAL 8)
  set(jemalloc_LG_SIZEOF_INT 3)
else()
  set(jemalloc_LG_SIZEOF_INT 2)
endif()

if (CMAKE_SIZEOF_LONG EQUAL 8)
  set(jemalloc_LG_SIZEOF_LONG 3)
else()
  set(jemalloc_LG_SIZEOF_LONG 2)
endif()

if (CMAKE_SIZEOF_LONGLONG EQUAL 8)
  set(jemalloc_LG_SIZEOF_LONGLONG 3)
else()
  set(jemalloc_LG_SIZEOF_LONGLONG 2)
endif()

if (CMAKE_SIZEOF_INTMAX_T EQUAL 16)
  set(jemalloc_LG_SIZEOF_INTMAX_T 4)
elseif(CMAKE_SIZEOF_INTMAX_T EQUAL 8)
  set(jemalloc_LG_SIZEOF_INTMAX_T 3)
else()
  set(jemalloc_LG_SIZEOF_INTMAX_T 2)
endif()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(jemalloc_LG_SIZEOF_PTR 3)
else()
  set(jemalloc_LG_SIZEOF_PTR 2)
endif()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(jemalloc_LG_VADDR 48)
else()
  set(jemalloc_LG_VADDR 32)
endif()

configure_file(jemalloc.h.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/${jemalloc_C_HDRS}" @ONLY)
configure_file(src/include/jemalloc/internal/jemalloc_preamble.h.in
  "${CMAKE_CURRENT_BINARY_DIR}/include/jemalloc/internal/jemalloc_preamble.h" @ONLY)
configure_file(jemalloc_internal_defs.h
  "${CMAKE_CURRENT_BINARY_DIR}/include/jemalloc/internal/jemalloc_internal_defs.h" @ONLY)
configure_file(public_namespace.h
  "${CMAKE_CURRENT_BINARY_DIR}/include/jemalloc/internal/public_namespace.h" @ONLY)
configure_file(private_namespace.h
  "${CMAKE_CURRENT_BINARY_DIR}/include/jemalloc/internal/private_namespace.h" @ONLY)

add_library(jemalloc-obj OBJECT
  ${jemalloc_C_HDRS}
  ${jemalloc_C_SRCS})

#target_compile_definitions(jemalloc-obj PUBLIC -UJEMALLOC_EXPORT)
target_include_directories(jemalloc-obj PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
target_include_directories(jemalloc-obj PRIVATE src/include)

target_link_libraries(jemalloc-obj PRIVATE ${CMAKE_DL_LIBS})

