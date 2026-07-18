# nucleo-bench

[![build](https://github.com/chgroeling/nucleo-bench/actions/workflows/main.yaml/badge.svg)](https://github.com/chgroeling/nucleo-bench/actions/workflows/main.yaml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Changelog](https://img.shields.io/badge/changelog-Keep%20a%20Changelog-orange.svg)](CHANGELOG.md)

A bare-metal benchmarking environment for STM32F446RE. I built this to
measure compute time and code size of algorithms running directly on a
Cortex-M4F — no OS, no HAL, no hidden overhead. If you need cycle-accurate
timings of embedded routines, this should save you the setup work.

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

The repo also ships ready-made algorithms, selected at build time via the
`ALGO` make variable (default: `none` — the empty baseline). The selection
defines `USE_ALGO_<NAME>` and makes `algo()` call the chosen routine:

| `ALGO=`   | Source                 | What it measures                                  |
|-----------|------------------------|---------------------------------------------------|
| `none`    | —                      | empty baseline (pure loop overhead)               |
| `nop`     | `src/algo_nop.cpp`     | 1000 nops — CPI / clock-frequency validation      |
| `sprintf` | `src/algo_sprintf.cpp` | newlib-nano `sprintf` (float/int/string/pointer formatting) |

```bash
make release ALGO=nop         # optimized build (-O3) including the nop example
make release ALGO=sprintf     # optimized build (-O3) including the sprintf test
                              # (make / make debug do the same at -O0)
```

`ALGO=...` works the same way on the run targets — see
[Run & measure](#run--measure).

Every build prints the firmware size. Here is the empty baseline
(`make release`, no algorithm selected):

```
   text    data     bss     dec     hex filename
   1580       4       4    1588     634 build/firmware.elf
```

- `text` — code + read‑only data (FLASH)
- `data` — initialized read/write data (RAM, copied from FLASH at startup)
- `bss`  — zero‑initialized read/write data (RAM, zeroed at startup)
- `dec` / `hex` — total (text + data + bss) in decimal / hex

The STM32F446RE has 512 KiB FLASH and 128 KiB RAM. To measure the code size
of an algorithm, compare its build against the empty baseline above; the
difference in the `text` column is the algorithm's code footprint. Changes in
`data` or `bss` reflect global/static variables it introduces.

For example, building the bundled nop example (`make release ALGO=nop`):

```
   text    data     bss     dec     hex filename
   3588       4       4    3596     e0c build/firmware.elf
```

The `text` grows from 1580 to 3588 bytes, so the 1000‑nop example costs
**3588 − 1580 = 2008 bytes** of FLASH — the 1000 nops (2 bytes each = 2000 B)
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
00002800 A _Min_Stack_Size
00019000 A _Min_Heap_Size
08000000 R isr_vector
08000188 T _semihost_write0
08000194 T Default_Handler
08000198 t _fault_report
080001bc W HardFault_Handler
080001c8 W MemManage_Handler
080001d4 W BusFault_Handler
080001e0 W UsageFault_Handler
080001ec t _memset.constprop.0
08000208 t _memcpy.constprop.0
0800022c T Reset_Handler
080002b8 T _sysclk_180mhz
08000348 T _dwt_init
0800036c T _dwt_cyccnt
08000378 T _dwt_zero
08000384 t _semihost_write_uint(unsigned long)
080003c4 t _semihost_write_seconds(unsigned long long)
080004b0 T main
08000590 t _GLOBAL__sub_I_main
0800062c d __init_array_start
0800062c d __preinit_array_end
0800062c d __preinit_array_start
08000630 d __init_array_end
08000630 A _sidata
20000000 D _edata
20000000 B _sbss
20000000 D _sdata
20000000 b g_runner
20000004 B _ebss
20000008 B _sheap
20020000 R _estack
```

Every symbol is defined in this repo; there are no undefined (externally
provided) symbols. Because nothing is linked implicitly, the size delta you
measure for an algorithm is honest end-to-end: if your code calls a libc
function (say `memcpy` or `printf`), that function is linked from newlib-nano
*then*, and its cost shows up in the `text` delta — you pay only for what you
use, and you can see exactly what that is. The bundled `ALGO=sprintf` example
demonstrates exactly this: it pulls newlib-nano's `sprintf` machinery into the
image, and the `text` delta against the baseline is its honest footprint.

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

### Newlib syscall stubs

Some newlib code paths expect POSIX-style syscalls — stdio's FILE glue
references `_read`/`_write`/`_close`/`_lseek`/`_fstat`/`_isatty`, and `abort()`
raises a signal via `_kill`/`_getpid`. `src/syscalls.c` provides minimal
implementations, so such code links cleanly instead of pulling libnosys's
stubs and their `_write is not implemented and will always fail` linker
warnings. `_write()` routes to the semihosting console (so `printf`/`puts`
output lands in the OpenOCD terminal); the others return sane character-device
defaults. Like the heap, all of it is stripped by `--gc-sections` unless
something actually references it — the empty baseline is unaffected.

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
make run_release               # empty baseline
make run_release ALGO=nop      # bundled nop example
make run_release ALGO=sprintf  # bundled sprintf test
```

Connects GDB, flashes, resets and runs. Benchmark output prints in the
OpenOCD terminal. When `main()` returns a breakpoint at `_exit_breakpoint`
catches the exit, the target is reset and GDB quits.

If the firmware crashes instead, the fault handlers report it before halting:
each of `HardFault` / `MemManage` / `BusFault` / `UsageFault` prints its name
via semihosting and stops on a `bkpt`, e.g.

```
*** fault: BusFault ***
```

(`MemManage`, `BusFault` and `UsageFault` are enabled at startup so a fault
lands in its own handler instead of escalating to `HardFault` — the message
names the actual cause. The handlers are `weak`; your algorithm may override
them.)

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
default; build with `make ALGO=nop` to include it. `src/algo_sprintf.cpp`
(build with `make ALGO=sprintf`) is a second example that exercises a real
libc routine — newlib-nano's `sprintf` — including the code-size cost of
linking it. To measure your own algorithm, add a new source file, update
`src/main.cpp` to call it, and adjust `kBenchRuns` for the desired repetition
count. No need to touch the bundled examples.

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

## Contributing

Contributions are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md) for the
(lightweight) process. The one hard rule: keep the image freestanding — no new
dependencies, no change to the empty baseline.

## License

Released under the [MIT License](LICENSE). © Christian Gröling.
