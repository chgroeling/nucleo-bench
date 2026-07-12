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
make clean        # remove build/
make              # debug build (-O0) → build/firmware.bin, build/firmware.elf
make debug        # same as make (explicit debug)
make release      # optimized build (-O3)
```

By default `algo()` is empty, so a build measures only the benchmark loop's
own overhead. This is the intended starting point: you drop in your own
algorithm (see [Bring your own algorithm](#bring-your-own-algorithm)) and
compare against this empty baseline.

The repo also ships one ready-made algorithm, `src/algo_nop.cpp` (1000 nops),
purely as a working example. It is **not** compiled in unless you ask for it
with `TEST_ALGO=1`, which defines `USE_TEST_ALGO` and makes `algo()` call it:

```bash
make TEST_ALGO=1              # debug build (-O0) including the nop example
make release TEST_ALGO=1      # optimized build (-O3) including the example
make run_debug TEST_ALGO=1    # build -O0, flash and run the example
make run_release TEST_ALGO=1  # build -O3, flash and run the example
```

Every build prints the firmware size. Here is the empty baseline
(`make release`, no algorithm selected):

```
   text    data     bss     dec     hex filename
   2476       4       4    2484     9b4 build/firmware.elf
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
   4480       4       4    4488    1188 build/firmware.elf
```

The `text` grows from 2476 to 4480 bytes, so the 1000‑nop example costs
**4480 − 2476 = 2004 bytes** of FLASH — the 1000 nops (2 bytes each = 2000 B)
plus the call/return glue around `algo_nop()`.

The build uses `-ffunction-sections -fdata-sections` and link‑time garbage
collection (`--gc-sections`) so unreferenced code and data are stripped
from the final binary — the measured footprint reflects only what your
algorithm actually pulls in.

### Run & measure

Use `make run_release` for benchmarking — the `-O3` build reflects real
algorithm performance. `make run_debug` (`-O0`, unoptimized) is optional and
mainly useful for stepping through code:

```bash
make run_release     # build -O3, flash and run (use this for measurements)
make run_debug       # build -O0, flash and run (optional, for debugging)
```

**Terminal 1** — OpenOCD server (semihosting output lands here):

```bash
openocd -d1 -f openocd.cfg    # GDB on :3333, -d1 suppresses driver noise
```

**Terminal 2** — build, flash and run in one step:

```bash
make run_release     # or, for debugging: make run_debug
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
dt = 16.853 s  avg = 5618 ns
```

At -O3 the effective frequency is 1000 nops / 5618 ns ≈ 178 MHz —
close to the 180 MHz core clock. The remaining gap is loop-counter overhead
that even -O3 cannot fully eliminate (3 M branches and increments).

- `wrap` — DWT cycle counter wraparound limit (2**32 cycles at 180 MHz)
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

`clobber_memory()` is a full memory-clobber fence (`asm volatile("" ::: "memory")`,
the same idiom as the Linux kernel's `barrier()`). It forbids reordering
loads/stores across the fence and prevents caching stale values in registers.
Use it when the algorithm's result lives in a buffer rather than a return
value:

```cpp
#include "compiler.hpp"

static uint8_t buf[256];

void algo(void)
{
    fill_buffer(buf, sizeof(buf));   // writes into buf
    clobber_memory();                // stores to buf must not be elided
}
```

Without the fence, `-O3` may notice `buf` is never read afterwards and drop the
stores (or the whole call). `clobber_memory()` makes the compiler treat all
memory as observed, so the writes stay.

### The empty baseline

When no algorithm is selected, `algo()` calls `compiler_barrier()` from
`compiler.hpp`, which wraps a single empty `__asm volatile("")`. It emits **zero**
instructions, yet counts as an observable side effect the optimizer must
preserve. Placing it in a code path stops the compiler from proving that path
has no effect and deleting it — here, the empty benchmark loop body. Without
it, `-O3` would delete the entire `for` loop (dead-code elimination), so the
DWT delta would read ~0 ns and the harness would look broken. Because it
generates no code, it adds no cycles — the measured time then reflects the pure
loop overhead (counter increment, compare, branch) alone. A `nop` would work
too, but it costs one real cycle per iteration and would skew that baseline.

Note the memory clobber is *not* used here: on the empty baseline it would only
add spurious loop-counter reloads.

Zero dependencies beyond nano stdlib — everything else is bare metal.
