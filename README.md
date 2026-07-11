# nucleo-eval

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

## Project layout

| Path                     | Purpose                                           |
|--------------------------|---------------------------------------------------|
| `src/main.cpp`           | Entry point, output helpers, `AlgoRunner`         |
| `src/algo_nop.cpp/hpp`   | 1000‑nop throughput test (CPI validation)         |
| `src/semihost.c`         | `_semihost_write0` — SYS_WRITE0 via `bkpt #0xAB` |
| `src/clock.c`            | PLL config to 180 MHz, DWT cycle‑counter helpers   |
| `src/startup.c`          | Vector table, `Reset_Handler`, C++ static init    |
| `linker/stm32f446re.ld`  | Linker script (512K FLASH, 128K RAM)              |
| `Makefile`               | Build, debug, release, run targets                |
| `openocd.cfg`            | OpenOCD config (ST-LINK/V2-1, SWD, semihosting)   |
| `debug.gdb`              | GDB batch script for semihosting sessions         |

## Usage

### Build

```bash
make clean        # remove build/
make              # debug build (-O0) → build/firmware.bin, build/firmware.elf
make debug        # same as make (explicit debug)
make release      # optimized build (-O3)
```

`make` prints the size of the debug firmware:

```
   text    data     bss     dec     hex filename
   4708       4       4    4716    126c build/firmware.elf
```

- `text` — code + read‑only data (FLASH)
- `data` — initialized read/write data (RAM, copied from FLASH at startup)
- `bss`  — zero‑initialized read/write data (RAM, zeroed at startup)
- `dec` / `hex` — total (text + data + bss) in decimal / hex

The STM32F446RE has 512 KiB FLASH and 128 KiB RAM. To measure the code
size of your algorithm, do a baseline build with an empty `algo()` body,
note the `text` value, then restore your algorithm and build again. The
difference in the `text` column is your algorithm's code footprint.
Changes in `data` or `bss` reflect global/static variables your algorithm
introduces.

The build uses `-ffunction-sections -fdata-sections` and link‑time garbage
collection (`--gc-sections`) so unreferenced code and data are stripped
from the final binary — the measured footprint reflects only what your
algorithm actually pulls in.

### Run & measure

```bash
make run_debug       # build -O0, flash and run
make run_release     # build -O3, flash and run
```

**Terminal 1** — OpenOCD server (semihosting output lands here):

```bash
openocd -d1 -f openocd.cfg    # GDB on :3333, -d1 suppresses driver noise
```

**Terminal 2** — build, flash and run in one step:

```bash
make run_debug       # or: make run_release
```

Connects GDB, flashes, resets and runs. Benchmark output prints in the
OpenOCD terminal. When `main()` returns a breakpoint at `_exit_breakpoint`
catches the exit, the target is reset and GDB quits.

Example output for the 1000‑nop throughput test (3 000 000 runs):

```
=== make run_debug (-O0 -g3) ===
--- start ---
wrap 23.861 s
runs 3000000
dt = 17.567 s  avg = 5856 ns

=== make run_release (-O3 -g3) ===
--- start ---
wrap 23.861 s
runs 3000000
dt = 16.853 s  avg = 5618 ns
```

At -O3 the effective frequency is 1000 nops / 5618 ns ≈ 178 MHz —
close to the 180 MHz core clock. The remaining gap is loop-counter overhead
that even -O3 cannot fully eliminate (3 M branches and increments).

- `wrap` — DWT cycle counter wraparound limit (2³² cycles at 180 MHz)
- `runs` — repetition count (`g_runner(N)`)
- `dt` — total elapsed time
- `avg` — per-run average in nanoseconds

## Bring your own algorithm

`src/algo_nop.cpp` is a test stub that lets you verify execution frequency
(cycles-per-nop → effective CPI). To measure your own algorithm, add a new
source file, update `src/main.cpp` to call it, and adjust `kBenchRuns` for
the desired repetition count. No need to touch `algo_nop.cpp`.

Zero dependencies beyond nano stdlib — everything else is bare metal.
