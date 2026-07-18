---
id: AGENTS
aliases: []
tags: []
---
# Project

`nucleo-bench` is a bare-metal benchmarking environment for the STM32 Nucleo-64
(NUCLEO-F446RE, Cortex-M4F). It measures compute time and code size of
algorithms running directly on the MCU — no OS, no HAL, no libc I/O. Everything
is bare metal: a hand-written vector table, a minimal `Reset_Handler`, C++ static
init support, PLL clock bring-up to 180 MHz, DWT cycle-counter timing, and an
assembly semihosting stub. Output is captured by OpenOCD / GDB, not a UART.

## Project layout

| Path                     | Purpose                                                        |
|--------------------------|----------------------------------------------------------------|
| `src/main.cpp`           | Entry point, semihosting output helpers, `AlgoRunner` bench loop |
| `src/algo_nop.cpp/.hpp`  | 1000-nop throughput stub for CPI / frequency validation        |
| `src/algo_sprintf.cpp/.hpp` | newlib-nano `sprintf` benchmark (float/int/string/pointer formatting, linked with `-u _printf_float`) |
| `src/compiler.hpp`       | Zero-overhead optimizer barriers (do_not_optimize, clobber_memory, compiler_barrier) |
| `src/heap.cpp`           | `_sbrk` + `operator new`/`delete` — heap support, GC'd out unless used |
| `src/syscalls.c`         | Newlib syscall stubs (`_write` → semihosting, rest fail cleanly), GC'd out unless used |
| `src/semihost.c`         | Semihosting stub `_semihost_write0` (SYS_WRITE0 via `bkpt #0xAB`) |
| `src/clock.c`            | PLL config to 180 MHz (over-drive), DWT cycle-counter helpers   |
| `src/startup.c`          | Vector table, `Reset_Handler`, `.data`/`.bss` init, C++ static init, fault reporting |
| `linker/stm32f446re.ld`  | Linker script (512K FLASH @ 0x08000000, 128K RAM @ 0x20000000) |
| `Makefile`               | Build (debug/release), flash and run targets                   |
| `openocd.cfg`            | OpenOCD config (ST-LINK/V2-1, SWD, semihosting)                |
| `debug.gdb`              | GDB batch script used by `make run_debug` / `make run_release` |
| `build/`                 | Build output (git-ignored)                                     |

## How it works

- **Startup**: `Reset_Handler` enables the FPU (`SCB_CPACR`), copies `.data`
  from FLASH to RAM, zeroes `.bss`, calls `_sysclk_180mhz()` to configure the
  PLL, enables the three configurable fault exceptions in `SCB_SHCSR`, then
  runs the C++ preinit/init arrays (static constructors), then calls `main()`.
- **Clock**: `_sysclk_180mhz()` brings HSI (16 MHz) up through the PLL to a
  180 MHz SYSCLK using over-drive mode and 5 flash wait states.
- **Timing**: `main()` uses the DWT cycle counter (`_dwt_init`, `_dwt_zero`,
  `_dwt_cyccnt`) to measure the benchmark loop; elapsed cycles are converted
  to seconds and per-run nanoseconds against the 180 MHz core clock.
- **Benchmark loop**: the `AlgoRunner` (`g_runner`) calls `algo()` `kBenchRuns`
  times (default 3,000,000). `algo()` dispatches on the `ALGO` build variable
  (`nop` → `algo_nop()`, `sprintf` → `algo_sprintf()`; default is an empty
  barrier-only baseline).
- **Output**: helpers call `_semihost_write0()` which issues `bkpt #0xAB` with
  `SYS_WRITE0 (0x04)` to print via the host debugger.
- **Exit detection**: after `main()` returns, `Reset_Handler` calls
  `_exit_breakpoint()` which executes `bkpt #0` so the debugger can detect
  program exit before the final `while (1)` halt loop.
- **Fault reporting**: `HardFault`/`MemManage`/`BusFault`/`UsageFault` handlers
  (weak, in `startup.c`) print `*** fault: <name> ***` via semihosting, then
  halt on `bkpt #0`. `Reset_Handler` enables the three configurable fault
  exceptions in `SCB_SHCSR` so they don't all escalate to HardFault.

## Toolchain

- `arm-none-eabi-gcc` / `arm-none-eabi-g++` / `objcopy` (Cortex-M4, hard float `fpv4-sp-d16`)
- `openocd` (debug server, flash programming, semihosting)
- `gdb-multiarch`

## Usage

### 1) Check connection
```bash
lsusb | grep STMicro
```

The board identifies on USB as:

```
Bus 002 Device 017: ID 0483:374b STMicroelectronics ST-LINK/V2.1
```

#### USB Vendor/Product IDs for ST-LINK/V2-1 (Nucleo boards)

| USB ID        | Description                              |
|---------------|------------------------------------------|
| `0483:374a`   | ST-LINK/V2-1 (mass storage + VCP)        |
| `0483:374b`   | ST-LINK/V2-1 (typical Nucleo-64)         |
| `0483:3752`   | ST-LINK/V2-1 (newer revision)            |

### 2) Build
```bash
make            # debug build (-O0) → build/firmware.bin, build/firmware.elf
make debug      # same as make (explicit debug)
make release    # optimized build (-O3)
make clean      # removes build/
```

Select the benchmarked algorithm with the `ALGO` variable (make variable or
environment): `ALGO=none` (default, empty baseline), `ALGO=nop`,
`ALGO=sprintf` — e.g. `make release ALGO=sprintf`. Unknown values fail the
build.

`make` prints the firmware size (`text` = FLASH code footprint). To measure an
algorithm's code size, baseline-build with an empty `algo()` body, note `text`,
then restore the algorithm and rebuild — the `text` delta is the footprint.
Link-time garbage collection (`--gc-sections`) strips unreferenced code/data.

### 3) Run with semihosting output
Start the OpenOCD server in one terminal:
```bash
openocd -d1 -f openocd.cfg    # GDB port :3333, semihosting output printed here
```

Then in another terminal:
```bash
make run_debug      # build -O0, flash and run
make run_release    # build -O3, flash and run
```

`make run_debug` and `make run_release` run
`gdb-multiarch -batch -q build/firmware.elf -x debug.gdb`, which connects to
OpenOCD, loads the firmware, resets, and continues. Benchmark output appears in
the `openocd` terminal. When `main()` returns, the `_exit_breakpoint` breakpoint
is hit; the GDB script prints an exit message, resets the target and quits
cleanly (batch mode auto-confirms prompts via `set confirm off`).

## Bring your own algorithm

`src/algo_nop.cpp` (`ALGO=nop`) is a test stub for verifying execution
frequency (cycles-per-nop → effective CPI); `src/algo_sprintf.cpp`
(`ALGO=sprintf`) benchmarks newlib-nano's `sprintf`. To benchmark your own
algorithm, add a source file, update `src/main.cpp`'s `AlgoRunner::algo()` to
call it, and adjust `kBenchRuns` for the desired repetition count. Zero
dependencies beyond nano stdlib — everything else is bare metal.
