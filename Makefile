CC       = arm-none-eabi-gcc
AS       = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy
OBJDUMP  = arm-none-eabi-objdump

BUILD    = build
TARGET   = $(BUILD)/firmware

C_SRC    = src/main.c src/semihost.c
OBJ      = $(BUILD)/src/main.o $(BUILD)/src/semihost.o

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

flash: $(TARGET).bin
	st-flash write $(TARGET).bin 0x08000000

debug: $(TARGET).elf
	LANG=C gdb-multiarch -batch -q $(TARGET).elf -x debug.gdb

clean:
	rm -rf $(BUILD)
