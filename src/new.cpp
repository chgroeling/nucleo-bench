/* C++ operator new / delete wrappers for algorithms that allocate
   (see syscalls.c for the underlying _sbrk / malloc / free plumbing).

   None of this is linked into the baseline: every symbol here is pulled
   in only on demand by --gc-sections. Built with -fno-exceptions, so
   operator new returns nullptr on failure instead of throwing
   std::bad_alloc -- which keeps the C++ exception runtime entirely out
   of the image. */

#include <stddef.h>

extern "C" void *malloc(size_t);
extern "C" void  free(void *);

void *operator new(size_t sz)            { return malloc(sz); }
void *operator new[](size_t sz)          { return malloc(sz); }
void  operator delete(void *p) noexcept  { free(p); }
void  operator delete[](void *p) noexcept{ free(p); }
void  operator delete(void *p, size_t) noexcept   { free(p); }
void  operator delete[](void *p, size_t) noexcept { free(p); }
