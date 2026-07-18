# Build and run targets for STM32F446RE Nucleo bare-metal firmware.

CC       = arm-none-eabi-gcc
CXX      = arm-none-eabi-g++
OBJCOPY  = arm-none-eabi-objcopy
SIZE     = arm-none-eabi-size

BUILD    = build
TARGET   = $(BUILD)/firmware

C_SRC    = src/semihost.c src/startup.c src/clock.c src/syscalls.c
CXX_SRC  = src/main.cpp src/algo_nop.cpp src/algo_sprintf.cpp src/new.cpp
OBJ      = $(patsubst src/%.c,$(BUILD)/src/%.o,$(C_SRC)) \
           $(patsubst src/%.cpp,$(BUILD)/src/%.o,$(CXX_SRC))

ARCH     = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
SPECS    = -specs=nano.specs -specs=nosys.specs
OPT      ?= -O0

# Select the benchmarked algorithm (default: none — empty baseline loop).
# Example: make release ALGO=sprintf
ALGO     ?= none
ifeq ($(ALGO),nop)
DEFS     += -DUSE_ALGO_NOP
else ifeq ($(ALGO),sprintf)
DEFS     += -DUSE_ALGO_SPRINTF
# newlib-nano's printf is integer-only; force the float engine in for %f/%g.
LD_ALGO  += -Wl,-u,_printf_float
else ifneq ($(ALGO),none)
$(error unknown ALGO '$(ALGO)' - valid values: none, nop, sprintf)
endif

SHARED   = $(ARCH) $(SPECS) -nostartfiles $(OPT) -g3 -Wall -Wextra -Isrc \
           -ffunction-sections -fdata-sections $(DEFS)

CFLAGS   = $(SHARED) -std=c11
CXXFLAGS = $(SHARED) -std=c++17 -fno-exceptions -fno-rtti

LDFLAGS  = -T linker/stm32f446re.ld
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref,--gc-sections
LDFLAGS += $(LD_ALGO)

.PHONY: all debug release run_debug run_release clean

all: $(TARGET).bin

debug: clean
	$(MAKE) OPT=-O0

release: clean
	$(MAKE) OPT=-O3

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
	$(SIZE) $<

run_debug: clean
	$(MAKE) OPT=-O0
	LANG=C gdb-multiarch -batch -q $(TARGET).elf -x debug.gdb

run_release: clean
	$(MAKE) OPT=-O3
	LANG=C gdb-multiarch -batch -q $(TARGET).elf -x debug.gdb

clean:
	rm -rf $(BUILD)
