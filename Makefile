CC       = arm-none-eabi-gcc
AS       = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy
OBJDUMP  = arm-none-eabi-objdump

BUILD    = build
TARGET   = $(BUILD)/firmware

C_SRC    = src/main.c
ASM_SRC  = src/semihost.s
OBJ      = $(BUILD)/src/main.o $(BUILD)/src/semihost.o

CFLAGS   = -mcpu=cortex-m4 -mthumb
CFLAGS  += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS  += -specs=nano.specs -specs=nosys.specs
CFLAGS  += -nostartfiles
CFLAGS  += -O0 -g3 -Wall -Wextra
CFLAGS  += -Isrc

LDFLAGS  = -T linker/stm32f411re.ld
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref

.PHONY: all flash debug clean

all: $(TARGET).bin

$(BUILD)/src:
	mkdir -p $@

$(BUILD)/src/main.o: $(C_SRC) | $(BUILD)/src
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/src/semihost.o: $(ASM_SRC) | $(BUILD)/src
	$(AS) $(CFLAGS) -c -o $@ $<

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ)

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash: $(TARGET).bin
	st-flash write $(TARGET).bin 0x08000000

debug: $(TARGET).elf
	gdb-multiarch -batch -q $(TARGET).elf \
		-ex "target extended-remote :4242" \
		-ex "load" \
		-ex "monitor reset" \
		-ex "echo === Check st-util terminal for output ===\n" \
		-ex "continue"

clean:
	rm -rf $(BUILD)
