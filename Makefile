# Build and run targets for STM32F446RE Nucleo bare-metal firmware.

CC       = arm-none-eabi-gcc
CXX      = arm-none-eabi-g++
OBJCOPY  = arm-none-eabi-objcopy

BUILD    = build
TARGET   = $(BUILD)/firmware

C_SRC    = src/semihost.c src/startup.c src/clock.c
CXX_SRC  = src/main.cpp src/algo_nop.cpp
OBJ      = $(patsubst src/%.c,$(BUILD)/src/%.o,$(C_SRC)) \
           $(patsubst src/%.cpp,$(BUILD)/src/%.o,$(CXX_SRC))

ARCH     = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
SPECS    = -specs=nano.specs -specs=nosys.specs
OPT      ?= -O0
SHARED   = $(ARCH) $(SPECS) -nostartfiles $(OPT) -g3 -Wall -Wextra -Isrc

CFLAGS   = $(SHARED) -std=c11
CXXFLAGS = $(SHARED) -std=c++17 -fno-exceptions -fno-rtti

LDFLAGS  = -T linker/stm32f446re.ld
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref

.PHONY: all run_debug run_release clean

all: $(TARGET).bin

$(BUILD)/src:
	mkdir -p $@

$(BUILD)/src/%.o: src/%.c | $(BUILD)/src
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/src/%.o: src/%.cpp | $(BUILD)/src
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET).elf: $(OBJ)
	$(CXX) $(SHARED) $(LDFLAGS) -o $@ $(OBJ)

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

run_debug:
	$(MAKE) OPT=-O0
	LANG=C gdb-multiarch -batch -q $(TARGET).elf -x debug.gdb

run_release:
	$(MAKE) OPT=-O3
	LANG=C gdb-multiarch -batch -q $(TARGET).elf -x debug.gdb

clean:
	rm -rf $(BUILD)
