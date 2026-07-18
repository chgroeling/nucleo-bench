/* ffb formatting benchmark — measures ffb::FixedFormatBuffer::Format() on the
   same mixed float / integer / string / hex / char format as algo_sprintf:

       "%0.6f:%04d:%+f:%s:%#x:%c:%%\n"

   fixed_format_buffer (https://github.com/chgroeling/fixed_format_buffer) is
   a header-only, allocation-free snprintf-subset reimplementation — no libc
   vfprintf machinery, no malloc, no exceptions. Compare compute time and the
   `text` delta of `make release ALGO=ffb` against `ALGO=sprintf` to see what
   dropping newlib-nano's printf (and its `-u _printf_float` engine) buys.

   The default StandardPolicy processes floats as `float` — passing a double
   is a compile-time error — so the float arguments are float literals. The
   output still matches sprintf's byte for byte with this format.

   The buffer is a function-local static (like sprintf's) so construction is
   not re-measured every call; the one-time init guard is a few cycles against
   a µs-scale format call. Namespace scope is deliberately avoided: the linker
   KEEPs .init_array*, so a global ctor would survive --gc-sections and leak
   into every other ALGO build's size baseline.

   Optimizer guards (see compiler.hpp):
   - do_not_optimize(len) keeps the return value alive,
   - clobber_memory() marks the buffer as observed, so -O3 cannot drop the
     formatted output as a dead store or hoist the call out of the loop. */

#include "algo_ffb.hpp"

#include "compiler.hpp"
#include "ffb/fixed_format_buffer.h"

void algo_ffb(void)
{
    static ffb::FixedFormatBuffer<64> buf;

    std::size_t len{buf.Format("%0.6f:%04d:%+f:%s:%#x:%c:%%\n",
                               1.234f, 42, 3.13f, "str", 1000U, 'X')};
    do_not_optimize(len);
    clobber_memory();
}
