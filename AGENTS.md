---
id: AGENTS
aliases: []
tags: []
---
# Project

This project outputs "Hello, World!" from an STM32 Nucleo board (F4 series,
STM32F446RE) using ARM semihosting over the ST-LINK/V2-1 debug interface.
There is no C runtime startup, no HAL and no libc I/O — everything is bare metal:
a hand-written vector table, a minimal `Reset_Handler`, and assembly semihosting
stubs. Output is captured by OpenOCD / GDB, not a UART.

## Project layout

| Path                     | Purpose                                                        |
|--------------------------|---------------------------------------------------------------|
| `src/main.c`             | Vector table, `Reset_Handler`, `main()`, exit breakpoint       |
| `src/semihost.c`         | Semihosting stubs (`_semihost_write_asm`)                      |
| `linker/stm32f446re.ld`  | Linker script (512K FLASH @ 0x08000000, 128K RAM @ 0x20000000) |
| `Makefile`               | Build, flash and run targets                                   |
| `openocd.cfg`            | OpenOCD config (ST-LINK/V2-1, STM32F4, semihosting)            |
| `debug.gdb`              | GDB batch script used by `make run_debug`                      |
| `build/`                 | Build output (git-ignored)                                     |

## How it works

- **Startup**: `Reset_Handler` copies `.data` from FLASH to RAM, zeroes `.bss`,
  then calls `main()`.
- **Output**: `main()` calls `_semihost_write_asm()` which issues `bkpt #0xAB`
  with `SYS_WRITE (0x05)` to print via the host debugger.
- **Exit detection**: after `main()` returns, `Reset_Handler` calls
  `_exit_breakpoint()` which executes `bkpt #0` so the debugger can detect
  program exit before the final `while (1)` halt loop.

## Toolchain

- `arm-none-eabi-gcc` / `objcopy` (Cortex-M4, hard float `fpv4-sp-d16`)
- `openocd` (debug server, flash programming, semihosting)
- `gdb-multiarch`

## Usage

### 1) Check connection
```bash
lsusb
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
make            # builds build/firmware.bin
make clean      # removes build/
```

### 3) Run with semihosting output
Start the OpenOCD server in one terminal:
```bash
openocd -d1 -f openocd.cfg    # GDB port :3333, semihosting output printed here
```

Then in another terminal:
```bash
make run_debug
```

`make run_debug` and `make run_release` run `gdb-multiarch -batch -q build/firmware.elf -x debug.gdb`,
which connects to OpenOCD, loads the firmware, resets, and continues.
Semihosting output appears in the `openocd` terminal. When `main()` returns,
the `_exit_breakpoint` breakpoint is hit; the GDB script prints an exit
message, resets the target and quits cleanly (batch mode auto-confirms
prompts via `set confirm off`).
