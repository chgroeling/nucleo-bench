/* Heap support for algorithms that allocate (malloc/free/new/delete).

   None of this is linked into the baseline: every symbol here is only pulled
   in on demand by --gc-sections. If your algorithm never allocates, _sbrk and
   the operator new/delete below are unreferenced and stripped, so the empty
   baseline stays libc-free. The moment your code calls malloc or new, the
   linker pulls newlib-nano's allocator, which calls _sbrk() below to obtain
   memory from the heap region reserved in the linker script — and only then
   does that cost show up in the `text` size.

   We deliberately reuse newlib-nano's malloc/free rather than writing our own:
   it is small, well-tested, and already on the linker search path via
   nano.specs. We supply only the one thing it needs from the platform, _sbrk,
   plus thin operator new/delete wrappers so C++ allocation maps onto it. */

#include <stddef.h>

/* newlib-nano's allocator; declared here to avoid pulling in <stdlib.h>. */
extern "C" void *malloc(size_t);
extern "C" void  free(void *);

/* Provided by the linker script (linker/stm32f446re.ld). */
extern char _sheap;            /* first byte of the heap                     */
extern char _estack;           /* top of RAM; stack grows down from here     */
extern char _Min_Stack_Size;   /* stack reservation kept clear of the heap   */

/* Grow the program break. newlib's malloc calls this to get more memory.
   Bumps a pointer up through the heap region; returns (void*)-1 (which malloc
   treats as out-of-memory) if the request would run into the reserved stack.
   No errno is set, keeping the dependency footprint to a single symbol. */
extern "C" void *_sbrk(ptrdiff_t incr)
{
    static char *heap_end = &_sheap;
    char *limit = &_estack - (ptrdiff_t)&_Min_Stack_Size;

    if (heap_end + incr > limit) {
        return (void *)-1;
    }

    char *prev = heap_end;
    heap_end += incr;
    return prev;
}

/* C++ allocation on top of malloc/free. Built with -fno-exceptions, so these
   return nullptr on failure instead of throwing std::bad_alloc — which keeps
   the exception runtime (libstdc++/libsupc++) out of the image entirely. */
void *operator new(size_t sz)            { return malloc(sz); }
void *operator new[](size_t sz)          { return malloc(sz); }
void  operator delete(void *p) noexcept  { free(p); }
void  operator delete[](void *p) noexcept{ free(p); }
void  operator delete(void *p, size_t) noexcept   { free(p); }
void  operator delete[](void *p, size_t) noexcept { free(p); }
