/* ----------------------------------------------------------------------------
Copyright (c) 2018-2020 Microsoft Research, Daan Leijen
This is free software; you can redistribute it and/or modify it under the
terms of the MIT license. A copy of the license can be found in the file
"LICENSE" at the root of this distribution.
-----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// This source provides convenient overrides for the new and
// delete operations in C++.
//
// On Windows, or when linking dynamically with tbbmalloc, these
// can be more performant than the standard new-delete operations.
// See <https://en.cppreference.com/w/cpp/memory/new/operator_new>
// ---------------------------------------------------------------------------
#pragma GCC visibility push(default)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-prototypes"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#if defined(__cplusplus)
  #include <new>
  #include "tbb/scalable_allocator.h"

  #if defined(__MINGW32__)
    #define tbb_decl_restrict
  #else
    #if (_MSC_VER >= 1900) && !defined(__EDG__)
      #define tbb_decl_restrict          __declspec(allocator) __declspec(restrict)
    #else
      #define tbb_decl_restrict          __declspec(restrict)
    #endif
  #endif

  #if defined(_MSC_VER) && defined(_Ret_notnull_) && defined(_Post_writable_byte_size_)
  // stay consistent with VCRT definitions
  #define tbb_decl_new(n)          [[nodiscard]] tbb_decl_restrict _Ret_notnull_ _Post_writable_byte_size_(n)
  #define tbb_decl_new_nothrow(n)  [[nodiscard]] tbb_decl_restrict _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(n)
  #else
  #define tbb_decl_new(n)          [[nodiscard]] tbb_decl_restrict
  #define tbb_decl_new_nothrow(n)  [[nodiscard]] tbb_decl_restrict
  #endif

  void operator delete(void* p) noexcept              { scalable_free(p); }
  void operator delete[](void* p) noexcept            { scalable_free(p); }

  void operator delete  (void* p, const std::nothrow_t&) noexcept { scalable_free(p); }
  void operator delete[](void* p, const std::nothrow_t&) noexcept { scalable_free(p); }

  tbb_decl_new(n) void* operator new(std::size_t n) noexcept(false) { return scalable_malloc(n); }
  tbb_decl_new(n) void* operator new[](std::size_t n) noexcept(false) { return scalable_malloc(n); }

  tbb_decl_new_nothrow(n) void* operator new  (std::size_t n, const std::nothrow_t& tag) noexcept { (void)(tag); return scalable_malloc(n); }
  tbb_decl_new_nothrow(n) void* operator new[](std::size_t n, const std::nothrow_t& tag) noexcept { (void)(tag); return scalable_malloc(n); }

  #if (__cplusplus >= 201402L || _MSC_VER >= 1916)
  void operator delete  (void* p, std::size_t n) noexcept { scalable_free(p); }
  void operator delete[](void* p, std::size_t n) noexcept { scalable_free(p); }
  #endif

  #if (__cplusplus > 201402L || defined(__cpp_aligned_new))
  void operator delete  (void* p, std::align_val_t al) noexcept { scalable_aligned_free(p); }
  void operator delete[](void* p, std::align_val_t al) noexcept { scalable_aligned_free(p); }
  void operator delete  (void* p, std::size_t n, std::align_val_t al) noexcept { scalable_aligned_free(p); }
  void operator delete[](void* p, std::size_t n, std::align_val_t al) noexcept { scalable_aligned_free(p); }
  void operator delete  (void* p, std::align_val_t al, const std::nothrow_t&) noexcept { scalable_aligned_free(p); }
  void operator delete[](void* p, std::align_val_t al, const std::nothrow_t&) noexcept { scalable_aligned_free(p); }

  void* operator new  (std::size_t n, std::align_val_t al) noexcept(false) { return scalable_aligned_malloc(n, static_cast<size_t>(al)); }
  void* operator new[](std::size_t n, std::align_val_t al) noexcept(false) { return scalable_aligned_malloc(n, static_cast<size_t>(al)); }
  void* operator new  (std::size_t n, std::align_val_t al, const std::nothrow_t&) noexcept { return scalable_aligned_malloc(n, static_cast<size_t>(al)); }
  void* operator new[](std::size_t n, std::align_val_t al, const std::nothrow_t&) noexcept { return scalable_aligned_malloc(n, static_cast<size_t>(al)); }
  #endif
#endif

#pragma GCC visibility pop
#pragma GCC diagnostic pop
