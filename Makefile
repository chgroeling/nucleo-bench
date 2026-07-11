CC       = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy

BUILD    = build
TARGET   = $(BUILD)/firmware

C_SRC    = src/main.c src/semihost.c src/startup.c
OBJ      = $(BUILD)/src/main.o $(BUILD)/src/semihost.o $(BUILD)/src/startup.o

CFLAGS   = -mcpu=cortex-m4 -mthumb
CFLAGS  += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS  += -specs=nano.specs -specs=nosys.specs
CFLAGS  += -nostartfiles
CFLAGS  += -O0 -g3 -Wall -Wextra
CFLAGS  += -Isrc

LDFLAGS  = -T linker/stm32f446re.ld
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref

.PHONY: all flash debug clean

all: $(TARGET).bin

$(BUILD)/src:
	mkdir -p $@

$(BUILD)/src/%.o: src/%.c | $(BUILD)/src
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ)

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash: $(TARGET).elf
	openocd -d1 -f openocd.cfg -c "program $< verify reset exit"

debug: $(TARGET).elf
	LANG=C gdb-multiarch -batch -q $< -x debug.gdb

clean:
	rm -rf $(BUILD)
