# Important directories
INC_DIR = ./include/
SRC_DIR = ./src/
LIB_DIR = ./libs/
BUILD_DIR = ./build/

# Change for different avrs and clock speeds
DEVICE = atmega32u4
CLOCK = 16000000

# Compiler settings
CC = avr-gcc
CFLAGS = -mmcu=$(DEVICE) -DF_CPU=$(CLOCK) -std=c99 -Wall -Os -lm -MMD -MP

# Create target lists
SRCS = $(wildcard $(SRC_DIR)*.c)
OBJS = $(subst $(SRC_DIR), $(BUILD_DIR), $(SRCS:.c=.o))
LIBS = $(wildcard $(LIB_DIR)/*.a)
DEPS = $(OBJS:.o=.d)

all: $(BUILD_DIR)main.hex

-include $(DEPS)

$(BUILD_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR)

$(BUILD_DIR)main.elf: $(OBJS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)main.elf $(OBJS) $(LIBS)

$(BUILD_DIR)main.hex: $(BUILD_DIR)main.elf
	rm -f $(BUILD_DIR)main.hex
	avr-objcopy -j .text -j .data -O ihex $(BUILD_DIR)main.elf $(BUILD_DIR)main.hex

flash: all
	dfu-programmer atmega32u4 erase
	dfu-programmer atmega32u4 flash $(BUILD_DIR)main.hex

clean:
	rm -r $(BUILD_DIR)

disasm: $(BUILD_DIR)main.elf
	avr-objdump -d $(BUILD_DIR)main.elf

# Build directory prerequisites
$(OBJS): | $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

