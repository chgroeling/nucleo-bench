# STM32 Nucleo F4 Series — Linux Commissioning Guide

## 1. Kernel Driver Requirements

**No kernel driver needed.** The ST-LINK/V2-1 onboard debugger on Nucleo F4 boards uses the standard `usb-storage` and `cdc-acm` kernel drivers already built into every modern Linux kernel. No additional kernel modules are required.

The board identifies on USB as:

```
Bus 002 Device 017: ID 0483:374b STMicroelectronics ST-LINK/V2.1
```

### USB Vendor/Product IDs for ST-LINK/V2-1 (Nucleo boards)

| USB ID        | Description                              |
|---------------|------------------------------------------|
| `0483:374a`   | ST-LINK/V2-1 (mass storage + VCP)        |
| `0483:374b`   | ST-LINK/V2-1 (typical Nucleo-64)         |
| `0483:3752`   | ST-LINK/V2-1 (newer revision)            |

For reference, standalone ST-LINK/V2 (Discovery boards) use `0483:3748`.

---

## 2. Udev Rules (USB Device Permissions)

**Problem:** By default, only `root` can access raw USB devices. Without udev rules, tools like `st-info` will fail with `LIBUSB_ERROR_ACCESS`.

### Option A: Permissive (single-user dev machine)

Create `/etc/udev/rules.d/49-stlinkv2-1.rules`:

```udev
# STM32 Nucleo boards, with onboard ST-LINK/V2-1
# e.g. STM32F0, STM32F4

SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374a", \
    MODE="0666", \
    SYMLINK+="stlinkv2-1_%n"

SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", \
    MODE="0666", \
    SYMLINK+="stlinkv2-1_%n"

SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3752", \
    MODE="0666", \
    SYMLINK+="stlinkv2-1_%n"
```

### Option B: Group-based (multi-user or more secure)

```udev
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374a", \
    MODE="0660", GROUP="plugdev"

SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", \
    MODE="0660", GROUP="plugdev"

SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3752", \
    MODE="0660", GROUP="plugdev"
```

Then add your user to the group and re-login:

```bash
sudo usermod -a -G plugdev $USER
```

### Activating the rules

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

> **Note:** The [stlink-org/stlink](https://github.com/stlink-org/stlink) project ships udev rules at `stlink/config/udev/rules.d/`. When building from source, pass `-DSTLINK_UDEV_RULES_DIR=/etc/udev/rules.d` to CMake to install them automatically.

---

## 3. Toolchain Installation

### 3.1 ARM GCC Cross-Compiler (`arm-none-eabi`)

#### Via package manager (recommended for quick start)

| Distribution  | Command                                                                                      |
|---------------|----------------------------------------------------------------------------------------------|
| Ubuntu/Debian | `sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi`          |
| Fedora        | `sudo dnf install arm-none-eabi-gcc arm-none-eabi-binutils arm-none-eabi-newlib`             |
| Arch Linux    | `sudo pacman -S arm-none-eabi-gcc arm-none-eabi-binutils arm-none-eabi-newlib`               |
| Arch (AUR)    | `yay -S gcc-arm-none-eabi-bin`                                                               |

#### Direct download from ARM (latest stable)

Download from [developer.arm.com](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads):

```bash
# Example with version 15.2.Rel1 — check site for latest
wget "https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi.tar.xz"
sudo mkdir -p /opt/arm-gnu-toolchain
sudo tar xJf arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi.tar.xz \
    -C /opt/arm-gnu-toolchain --strip-components=1
echo 'export PATH="/opt/arm-gnu-toolchain/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

#### XPack prebuilt (alternative)

```bash
sudo mkdir -p /opt/xpack-arm-none-eabi
sudo tar xf xpack-arm-none-eabi-gcc-13.3.1-1.1-linux-x64.tar.gz \
    -C /opt/xpack-arm-none-eabi --strip-components=1
echo 'export PATH="/opt/xpack-arm-none-eabi/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

**Verify toolchain:**

```bash
arm-none-eabi-gcc --version
```

---

### 3.2 ST-LINK Utilities (flashing & debugging)

#### Via package manager

| Distribution  | Command                        |
|---------------|--------------------------------|
| Ubuntu/Debian | `sudo apt install stlink-tools`|
| Fedora        | `sudo dnf install stlink`      |
| Arch Linux    | `sudo pacman -S stlink`        |

#### Build from source (latest features)

```bash
# Prerequisites
sudo apt install git make cmake libusb-1.0-0-dev gcc build-essential

# Build and install
git clone https://github.com/stlink-org/stlink.git
cd stlink
mkdir build && cd build
cmake -DSTLINK_UDEV_RULES_DIR=/etc/udev/rules.d -DSTLINK_STATIC_LIB=OFF ..
make -j$(nproc)
sudo make install
sudo ldconfig
```

**Verify ST-LINK:**

```bash
st-info --probe
```

Expected output (example for STM32F446 Nucleo):

```
Found 1 stlink programmers
  version:    V2J27S15
  serial:     066EFF485550755187251425
  flash:      524288 (pagesize: 131072)
  sram:       131072
  chipid:     0x421
  dev-type:   STM32F446
```

#### Key commands

| Command                                   | Description                    |
|-------------------------------------------|--------------------------------|
| `st-info --probe`                         | Detect connected STM32 device  |
| `st-flash write firmware.bin 0x8000000`   | Flash binary to MCU            |
| `st-flash read dump.bin 0 0xFFFF`         | Read flash memory              |
| `st-util --semihosting`                   | Start GDB server on :4242      |

---

### 3.3 OpenOCD (debug server)

```bash
sudo apt install openocd       # Debian/Ubuntu
sudo dnf install openocd       # Fedora
sudo pacman -S openocd         # Arch
```

**Connect to Nucleo F4:**

```bash
openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
        -f /usr/share/openocd/scripts/target/stm32f4x.cfg
```

**Flash via OpenOCD (alternative to st-flash):**

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
        -c "program firmware.elf verify reset exit"
```

---

## 4. Permissions & User Setup Summary

| Step | Command |
|------|---------|
| Install udev rules | `sudo cp 49-stlinkv2-1.rules /etc/udev/rules.d/` |
| Reload udev | `sudo udevadm control --reload-rules && sudo udevadm trigger` |
| Add user to `plugdev` (optional) | `sudo usermod -a -G plugdev $USER` |
| **Logout and login** | Required for new group membership |
| Verify device appears on USB | `lsusb \| grep -i stlink` |
| Verify user access | `st-info --probe` |

---

## 5. Quick-Start (All-in-One)

Copy-paste setup for Ubuntu/Debian:

```bash
# 1. Install toolchain and tools
sudo apt install -y gcc-arm-none-eabi binutils-arm-none-eabi stlink-tools openocd

# 2. Install udev rules
sudo tee /etc/udev/rules.d/49-stlinkv2-1.rules << 'EOF'
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374a", MODE="0666"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", MODE="0666"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3752", MODE="0666"
EOF

sudo udevadm control --reload-rules && sudo udevadm trigger

# 3. Verify setup
arm-none-eabi-gcc --version
st-info --probe
```

---

## 6. Troubleshooting

| Symptom | Likely cause | Fix |
|---------|-------------|-----|
| Board not in `lsusb` | Bad USB cable, VM passthrough missing, or jumper issue | Use a data-capable cable, enable USB passthrough in VM, check BOOT0 jumper |
| `LIBUSB_ERROR_ACCESS` on `st-info --probe` | udev rules not applied | Re-run `sudo udevadm trigger` or re-plug board |
| `unknown chip id! 0x0000` | Target MCU not powered or ST-LINK not communicating | Ensure Nucleo board is powered via USB; check SWD jumpers are in place |
| `unknown chip id! 0x413` / `No such file or directory` for chips | Missing chip database | Copy `stlink/config/chips/` to `/usr/local/share/stlink/chips/` |
| `st-flash write` fails after working before | MCU in sleep/low-power mode | Hold RESET button, run flash command, release RESET |

---

## Sources

- [ARM GNU Toolchain — Official Install Guide](https://learn.arm.com/install-guides/gcc/arm-gnu)
- [stlink-org/stlink — GitHub](https://github.com/stlink-org/stlink)
- [cjacker/opensource-toolchain-stm32 — GitHub](https://github.com/cjacker/opensource-toolchain-stm32)
- [udev Rules for ST-LINK (Gist)](https://gist.github.com/daniel-p-carvalho/5b533d7e743081960637e73535f60e6d)
- [Installing ST-Link v2 on Linux — freeelectron.ro](https://freeelectron.ro/installing-st-link-v2-to-flash-stm32-targets-on-linux/)
- [ST Community: udev rules for STM32CubeProgrammer](https://community.st.com/stm32cubeprogrammer-mcus-30/error-connecting-with-st-link-in-linux-135682)
- [STM32 in Linux — Leo Febey](https://leofebey.com/blog/stm32-linux/)
