# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- `ffb` benchmark (`src/algo_ffb.cpp/.hpp`) exercising the vendored
  header-only [fixed_format_buffer](https://github.com/chgroeling/fixed_format_buffer)
  library (`src/ffb/fixed_format_buffer.h`, v0.3.1) — allocation-free, no
  libc printf; select with `make release ALGO=ffb`.
- `sprintf` benchmark example (`src/algo_sprintf.cpp/.hpp`) exercising
  newlib-nano's `sprintf` (float/int/string/hex formatting, linked with
  `-u _printf_float`); select with `make release ALGO=sprintf`.
- Minimal newlib syscall implementations (`src/syscalls.c`): `_write` routes
  to the semihosting console, the rest return clean character-device
  defaults — removes libnosys's "not implemented" linker warnings. Stripped
  from the image unless referenced.
- Fault reporting: `HardFault`/`MemManage`/`BusFault`/`UsageFault` handlers
  print `*** fault: <name> ***` via semihosting and halt on a breakpoint;
  the configurable fault exceptions are enabled at startup (empty baseline
  grows 1392 → 1560 bytes text).
- FPU enabled in `Reset_Handler` (`SCB_CPACR` grants CP10/CP11 access)
  before any code runs — the whole image (including newlib) is built
  hard-float, and without this the first VFP instruction raises UsageFault.

### Changed

- Algorithm selection now uses the `ALGO` make variable
  (`ALGO=none|nop|sprintf|ffb`, default `none`), replacing `TEST_ALGO=1`.
- `sprintf` and `ffb` benchmarks share the byte-identical format
  `"%0.6f:%04d:%+f:%s:%#x:%c:%%\n"` (ffb's supported subset: no `%g`/`%p`,
  precision capped at 6) for a direct time and code-size comparison.
- `_sbrk` moved from `heap.cpp` to `syscalls.c` alongside the other newlib
  syscall stubs; `heap.cpp` renamed to `new.cpp` (now holds only C++
  `operator new`/`delete`).

## [1.0.0] - 2026-07-12

Initial release.

[Unreleased]: https://github.com/chgroeling/nucleo-bench/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/chgroeling/nucleo-bench/releases/tag/v1.0.0
