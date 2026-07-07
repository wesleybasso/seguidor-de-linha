BUILD_DIR := build
TARGET := firmware

CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size

CFLAGS := -std=c11 -Wall -Wextra -Os -ffunction-sections -fdata-sections
CFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
CFLAGS += -DSTM32F411xE -DPEGASUS_TARGET_F411
CFLAGS += -ICore/Inc -IDrivers/CMSIS/Include -IDrivers/CMSIS/Device/ST/STM32F4xx/Include

LDFLAGS := -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
LDFLAGS += -TSTM32F411CEUX_FLASH.ld -Wl,--gc-sections -Wl,--no-warn-rwx-segments -Wl,-Map=$(BUILD_DIR)/$(TARGET).map

C_SOURCES := $(wildcard Core/Src/*.c)
ASM_SOURCES := Core/Startup/startup_stm32f411xe.s
OBJECTS := $(patsubst Core/Src/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
OBJECTS += $(BUILD_DIR)/startup_stm32f411xe.o

.PHONY: all clean

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin
	$(SIZE) $(BUILD_DIR)/$(TARGET).elf

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: Core/Src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/startup_stm32f411xe.o: $(ASM_SOURCES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR)
