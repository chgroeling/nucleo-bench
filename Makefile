# Build, flash and debug targets for STM32F446RE Nucleo bare-metal firmware.

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
SHARED   = $(ARCH) $(SPECS) -nostartfiles -O0 -g3 -Wall -Wextra -Isrc

CFLAGS   = $(SHARED) -std=c11
CXXFLAGS = $(SHARED) -std=c++17 -fno-exceptions -fno-rtti

LDFLAGS  = -T linker/stm32f446re.ld
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref

.PHONY: all flash debug clean

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

flash: $(TARGET).elf
	openocd -d1 -f openocd.cfg -c "program $< verify reset exit"

debug: $(TARGET).elf
	LANG=C gdb-multiarch -batch -q $< -x debug.gdb

clean:
	rm -rf $(BUILD)
