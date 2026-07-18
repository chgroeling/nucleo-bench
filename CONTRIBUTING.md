# Contributing to nucleo-bench

Thanks for your interest! This is a small, focused project — contributions are
welcome and the process is deliberately lightweight.

## Ways to help

- **Report a bug or ask a question** — open an [issue](https://github.com/chgroeling/nucleo-bench/issues).
  Include your toolchain versions (`arm-none-eabi-gcc --version`, `openocd --version`)
  and, for hardware issues, your board and `lsusb` output.
- **Suggest an improvement** — open an issue describing the idea before large
  changes, so we can agree on scope.
- **Send a fix or feature** — fork, branch, and open a pull request.

## Pull requests

1. Fork the repo and create a branch off `main`.
2. Make your change.
3. Make sure the firmware still builds clean before pushing:
   ```bash
   make release                 # empty baseline
   make release ALGO=nop        # bundled nop example
   make release ALGO=sprintf    # bundled sprintf example
   ```
   CI runs these same builds and reports the firmware size.
4. Add an entry under the `## [Unreleased]` section of
   [CHANGELOG.md](CHANGELOG.md), grouped under `Added` / `Changed` / `Fixed`
   (etc.) as appropriate.
5. Open the PR with a short description of *what* changed and *why*.

## Keeping the project honest

This repo's whole point is measuring compute time and code size on bare metal
with **zero hidden overhead**. Please keep that intact:

- **No new dependencies.** The image links no support library by default (no
  libc, no libgcc) — see the README's *No library dependencies* section. Don't
  add anything that changes the empty baseline.
- **Match the existing style.** C is C11, C++ is C++17 (`-fno-exceptions`,
  `-fno-rtti`), warnings-clean under `-Wall -Wextra`. Follow the naming and
  formatting already in `src/`.
- **The `algo()` slot is for the caller.** Add example algorithms as separate
  files gated behind a build flag (like `ALGO=nop` / `ALGO=sprintf`) rather
  than wiring them in by default.

That's it — thanks for contributing!
