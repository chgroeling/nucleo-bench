# nucleo-bench

A bare-metal benchmarking environment for STM32F446RE. I built this to
measure compute time and code size of algorithms running directly on a
Cortex-M4F — no OS, no HAL, no hidden overhead. If you need cycle-accurate
timings of small embedded routines, this should save you the setup work.

Output goes over ARM semihosting through the on-board ST-LINK/V2-1. You
read the numbers in an OpenOCD terminal — no UART wiring required.

## Hardware

| What                | Which                           |
|---------------------|---------------------------------|
| Board               | STM32 Nucleo-64 (NUCLEO-F446RE) |
| MCU                 | STM32F446RE (Cortex-M4F)        |
| Max clock           | 180 MHz via HSI-PLL             |
| Debug probe         | ST-LINK/V2-1 (on-board)         |
| USB ID              | `0483:374b`                     |
| Flash / RAM         | 512 KiB / 128 KiB               |

Connect the board via USB. Verify with:

```bash
lsusb | grep STMicro
# → Bus 002 Device 017: ID 0483:374b STMicroelectronics ST-LINK/V2.1
```

## Prerequisites

- `arm-none-eabi-gcc` / `arm-none-eabi-g++` (Cortex-M4, hard-float ABI)
- `openocd` (debug server, flash programming)
- `gdb-multiarch`

Install with your distribution's package manager.

### udev rules (Linux)

```bash
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="0483", MODE="0666"' | sudo tee /etc/udev/rules.d/99-stlink.rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

Reconnect the board afterwards.

## Usage

### Build

```bash
make release      # optimized build (-O3) → build/firmware.bin, build/firmware.elf
                  # (make / make debug build unoptimized -O0 for stepping)
```

By default `algo()` is empty, so a build measures only the benchmark loop's
own overhead. This is the intended starting point: you drop in your own
algorithm (see [Bring your own algorithm](#bring-your-own-algorithm)) and
compare against this empty baseline.

The repo also ships one ready-made algorithm, `src/algo_nop.cpp` (1000 nops),
purely as a working example. It is **not** compiled in unless you ask for it
with `TEST_ALGO=1`, which defines `USE_TEST_ALGO` and makes `algo()` call it:

```bash
make release TEST_ALGO=1      # optimized build (-O3) including the nop example
                              # (make / make debug do the same at -O0)
```

`TEST_ALGO=1` works the same way on the run targets — see
[Run & measure](#run--measure).

Every build prints the firmware size. Here is the empty baseline
(`make release`, no algorithm selected):

```
   text    data     bss     dec     hex filename
   1392       4       4    1400     578 build/firmware.elf
```

- `text` — code + read‑only data (FLASH)
- `data` — initialized read/write data (RAM, copied from FLASH at startup)
- `bss`  — zero‑initialized read/write data (RAM, zeroed at startup)
- `dec` / `hex` — total (text + data + bss) in decimal / hex

The STM32F446RE has 512 KiB FLASH and 128 KiB RAM. To measure the code size
of an algorithm, compare its build against the empty baseline above; the
difference in the `text` column is the algorithm's code footprint. Changes in
`data` or `bss` reflect global/static variables it introduces.

For example, building the bundled nop example (`make release TEST_ALGO=1`):

```
   text    data     bss     dec     hex filename
   3400       4       4    3408     d50 build/firmware.elf
```

The `text` grows from 1392 to 3400 bytes, so the 1000‑nop example costs
**3400 − 1392 = 2008 bytes** of FLASH — the 1000 nops (2 bytes each = 2000 B)
plus the call/return glue around `algo_nop()`.

The build uses `-ffunction-sections -fdata-sections` and link‑time garbage
collection (`--gc-sections`) so unreferenced code and data are stripped
from the final binary — the measured footprint reflects only what your
algorithm actually pulls in.

### No library dependencies

The image is fully freestanding: it links **no** support library — not libc
(newlib-nano), not libgcc. `nano.specs` puts newlib-nano on the linker search
path, but nothing is actually pulled from it. Startup uses private
`_memcpy` / `_memset` (so the compiler's loop-idiom pass doesn't drag in libc),
and 64-bit division goes through a hand-written `udivmod64()` (so GCC doesn't
call libgcc's `__aeabi_uldivmod`). Verify with `nm`:

```console
$ arm-none-eabi-nm -C -n build/firmware.elf   # (weak ISR aliases omitted)
08000000 R isr_vector
08000188 T _semihost_write0
08000194 T Default_Handler
08000198 t _memset.constprop.0
080001b4 t _memcpy.constprop.0
080001d8 T Reset_Handler
08000240 T _sysclk_180mhz
080002d0 T _dwt_init
080002f4 T _dwt_cyccnt
08000300 T _dwt_zero
0800030c t _semihost_write_uint(unsigned long)
0800034c t _semihost_write_seconds(unsigned long long)
08000438 T main
08000518 t _GLOBAL__sub_I_main
...
```

Every symbol is defined in this repo; there are no undefined (externally
provided) symbols. Because nothing is linked implicitly, the size delta you
measure for an algorithm is honest end-to-end: if your code calls a libc
function (say `memcpy` or `printf`), that function is linked from newlib-nano
*then*, and its cost shows up in the `text` delta — you pay only for what you
use, and you can see exactly what that is.

### Heap allocation (malloc / free / new / delete)

Algorithms that need dynamic memory can use `malloc`/`free` and C++
`new`/`delete` — but **only if they actually allocate**. Nothing about the heap
appears in the baseline; it is all pulled in on demand by `--gc-sections`.

How it is wired:

- The linker script reserves a heap region after `.bss` (`_Min_Heap_Size`,
  default 100 KiB) and a stack reservation (`_Min_Stack_Size`, default 10 KiB).
  No section is emitted for them, so the reported `.bss` is unchanged — the
  reservation is enforced only by a link-time `ASSERT` that fails the build if
  RAM can't hold `.data + .bss + heap + stack`. Tune the two sizes at the top
  of `linker/stm32f446re.ld` to your algorithm.
- `src/heap.cpp` supplies `_sbrk()` (the one platform hook newlib-nano's
  allocator needs) and thin `operator new`/`delete` wrappers over
  `malloc`/`free`. We reuse newlib-nano's small, well-tested allocator rather
  than shipping our own.
- Built with `-fno-exceptions`, so `operator new` returns `nullptr` on failure
  instead of throwing `std::bad_alloc`. That keeps the C++ exception runtime
  (libstdc++/libsupc++) out of the image entirely.

The key property is that this costs **nothing until you use it**. Prove it with
`nm`. In the empty baseline there is no allocator at all:

```console
$ arm-none-eabi-nm build/firmware.elf | grep -iE 'malloc|_sbrk|operator new'
$          # → no output: stripped by --gc-sections
```

Add a single `new int[16]` / `delete[]` to your algorithm and rebuild, and the
allocator is linked — `_sbrk`, `malloc`/`free`, and the C++ `operator new[]`
(`_Znaj`) / `operator delete[]` (`_ZdaPv`) now appear, and `text` grows
accordingly (≈ +670 bytes here, all of it newlib-nano's allocator):

```console
$ arm-none-eabi-nm build/firmware.elf | grep -iE 'malloc|_sbrk|_Zna|_Zda'
08000188 T malloc
08000198 T free
080001f0 T _malloc_r
080002f0 T _free_r
08000790 T _sbrk
080007bc T _Znaj          # operator new[]
080007c0 T _ZdaPv         # operator delete[]
```

So the heap is available when an algorithm needs it, yet the libc dependency
stays at zero for algorithms that don't allocate.

### Run & measure

Benchmarking uses two terminals: one runs the OpenOCD server, the other builds,
flashes and runs the firmware. Use the `-O3` `run_release` target — it reflects
real algorithm performance (`run_debug` builds `-O0` for stepping).

**Terminal 1** — OpenOCD server (semihosting output lands here):

```bash
openocd -d1 -f openocd.cfg    # GDB on :3333, -d1 suppresses driver noise
```

**Terminal 2** — build, flash and run in one step:

```bash
make run_release              # empty baseline
make run_release TEST_ALGO=1  # include the bundled nop example
```

Connects GDB, flashes, resets and runs. Benchmark output prints in the
OpenOCD terminal. When `main()` returns a breakpoint at `_exit_breakpoint`
catches the exit, the target is reset and GDB quits.

Example output for the 1000‑nop throughput test (3 000 000 runs):

```
=== make run_release (-O3 -g3) ===
--- start ---
wrap 23.861 s
runs 3000000
dt = 16.833 s  avg = 5611 ns
```

At -O3 the effective frequency is 1000 nops / 5611 ns ≈ 178 MHz —
close to the 180 MHz core clock. The remaining gap is loop-counter overhead
that even -O3 cannot fully eliminate (3 M branches and increments).

- `wrap` — DWT cycle counter wraparound limit (2**32 cycles at 180 MHz). The
  total measured time (`dt`) must **never** exceed this: the 32-bit cycle
  counter silently wraps past it, making the reading wrong. Keep `kBenchRuns` ×
  per-run time under `wrap`; if you approach it, lower `kBenchRuns`.
- `runs` — repetition count (`g_runner(N)`)
- `dt` — total elapsed time
- `avg` — per-run average in nanoseconds

## Bring your own algorithm

`src/algo_nop.cpp` is a test stub that lets you verify execution frequency
(cycles-per-nop → effective CPI). It is an example only and is deactivated by
default; build with `make TEST_ALGO=1` to include it. To measure your own
algorithm, add a new source file, update `src/main.cpp` to call it, and adjust
`kBenchRuns` for the desired repetition count. No need to touch `algo_nop.cpp`.

### Keeping your algorithm alive under -O3

If an algorithm's result is never observed, `-O3` deletes the computation as
dead code, or hoists a loop-invariant call out of the loop — so you end up
timing nothing. `compiler.hpp` provides three zero-overhead barriers; two of
them, described here, keep your algorithm alive (the third, `compiler_barrier()`,
is for the empty baseline — see [below](#the-empty-baseline)). All emit **zero**
instructions — they only constrain the optimizer, so they don't distort the
measured cycles.

`do_not_optimize(value)` forces `value` into a register/memory as a side
effect, so the code producing it cannot be discarded. Apply it to an
algorithm's result (or a mutated input):

```cpp
#include "compiler.hpp"

void algo(void)
{
    int result = my_algorithm(input);
    do_not_optimize(result);   // result is "used" → computation is kept
}
```

`clobber_memory()` tells the compiler: "assume every byte of memory may have
just been read and written here." That single assumption has two effects — it
must finish all pending writes before this point (so they can't be optimized
away), and it must re-read any value it had kept in a register afterwards (since
memory might have changed). In short, it forces all memory writes to really
happen. Use it when the algorithm's result lives in a buffer rather than a
return value:

```cpp
#include "compiler.hpp"

static uint8_t buf[256];

void algo(void)
{
    fill_buffer(buf, sizeof(buf));   // writes into buf
    clobber_memory();                // force those writes to actually happen
}
```

Without it, `-O3` may notice `buf` is never read afterwards and drop the writes
(or the whole call) as pointless. `clobber_memory()` makes the compiler treat
that memory as observed, so the writes stay. It is the same one-line trick as
the Linux kernel's `barrier()` (`asm volatile("" ::: "memory")`).

### The empty baseline

When no algorithm is selected, `algo()` calls `compiler_barrier()` from
`compiler.hpp`, which wraps a single empty `__asm volatile("")`. It emits **zero**
instructions, yet counts as an observable side effect the optimizer must
preserve. Placing it in a code path stops the compiler from proving that path
has no effect and deleting it — here, the empty benchmark loop body. Without
it, `-O3` would delete the entire `for` loop (dead-code elimination), so the
DWT delta would read ~0 ns and the harness would look broken. Because it
generates no code, it adds no cycles — the measured time then reflects the pure
loop overhead (counter increment, compare, branch) alone. 
