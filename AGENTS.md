---
id: AGENTS
aliases: []
tags: []
---
# Project
This project should output a "Hello World" on a STM32 Nucelo Board for F4 series. The output must 
be done by the stlink utilties

## Usage

1) Check connection
```bash
lsusb
```

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

