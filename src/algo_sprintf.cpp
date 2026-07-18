/* sprintf formatting benchmark — measures newlib-nano's sprintf() on a mixed
   float / integer / string / hex / char format:

       "%0.6f:%04d:%+f:%s:%#x:%c:%%\n"

   The format is deliberately identical to algo_ffb's (same specifiers, same
   arguments, byte-identical output) so ALGO=sprintf vs ALGO=ffb is a direct
   head-to-head — it sticks to ffb's supported subset (no %g/%p, precision
   capped at 6).

   Unlike the nop stub this deliberately pulls real libc code (newlib-nano's
   vfprintf machinery) into the image, so it exercises both compute time and
   the code-size delta of a libc dependency: compare `text` of
   `make release ALGO=sprintf` against the empty baseline (`make release`).

   newlib-nano's printf is integer-only by default; the Makefile links
   ALGO=sprintf builds with `-u _printf_float` to force the float engine in.
   Float conversion (_dtoa_r) allocates via malloc, served by _sbrk from
   syscalls.c — so this benchmark also drags in the allocator.

   Optimizer guards (see compiler.hpp):
   - do_not_optimize(len) keeps the return value alive,
   - clobber_memory() marks the static buffer as observed, so -O3 cannot
     drop the formatted output as a dead store or hoist the call out of
     the benchmark loop. */

#include "algo_sprintf.hpp"

#include <stdio.h>

#include "compiler.hpp"

void algo_sprintf(void)
{
    static char buf[64];

    int len{sprintf(buf, "%0.6f:%04d:%+f:%s:%#x:%c:%%\n",
                    1.234, 42, 3.13, "str", 1000U, (int)'X')};
    do_not_optimize(len);
    clobber_memory();
}
