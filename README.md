# nucleo-eval

Bare-metal Hello World on STM32 Nucleo-F446RE with ARM semihosting over
ST-LINK/V2-1. No C runtime, no HAL, no libc I/O.

## Prerequisites

### Debian / Ubuntu
```bash
sudo apt install gcc-arm-none-eabi openocd gdb-multiarch
```

### Arch Linux
```bash
sudo pacman -S arm-none-eabi-gcc openocd arm-none-eabi-gdb
```

### Fedora
```bash
sudo dnf install arm-none-eabi-gcc-cs openocd gdb
```

### openSUSE
```bash
sudo zypper install cross-arm-none-gcc14 openocd gdb
```

### udev rules (Linux)

The ST-LINK debugger needs permission to access USB. Create a udev rule:

```bash
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="0483", MODE="0666"' | sudo tee /etc/udev/rules.d/99-stlink.rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

Reconnect the board after adding the rule.

## Usage

### Check board connection

```bash
lsusb | grep STMicro
```

Expected: `ID 0483:374b STMicroelectronics ST-LINK/V2.1`

### Build

```bash
make            # builds build/firmware.bin and build/firmware.elf
make clean      # removes build/
```

### Flash

```bash
make flash
```

Uses OpenOCD to erase, program, verify and reset the target.

### Debug with semihosting output

**Terminal 1** — start the OpenOCD debug server:
```bash
openocd -d1 -f openocd.cfg
```

OpenOCD listens on GDB port `:3333`. Semihosting output appears in this
terminal. The `-d1` flag suppresses cosmetic `Info:` noise from the ST-LINK
driver.

**Terminal 2** — build, load and run:
```bash
make debug
```

Connects GDB to OpenOCD, loads the firmware, resets the target and runs it.
Output `"Hello, World!"` appears in the OpenOCD terminal. When `main()`
returns, the breakpoint at `_exit_breakpoint` is hit, the target is reset,
and GDB exits cleanly.
