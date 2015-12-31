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

# Create targets
SRCS = $(wildcard $(SRC_DIR)*.c)
OBJS = $(subst $(SRC_DIR), $(BUILD_DIR), $(SRCS:.c=.o))
LIBS = $(wildcard $(LIB_DIR)/*.a)
DEPS = $(OBJS:.o=.d)
ELF = $(BUILD_DIR)main.elf
HEX = $(BUILD_DIR)main.hex

all: $(HEX)

-include $(DEPS)

$(BUILD_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR)

$(ELF): $(OBJS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)main.elf $(OBJS) $(LIBS)

$(HEX): $(ELF)
	rm -f $(HEX)
	avr-objcopy -j .text -j .data -O ihex $(ELF) $(HEX)

flash: all
	dfu-programmer atmega32u4 erase
	dfu-programmer atmega32u4 flash $(HEX)

clean:
	rm -r $(BUILD_DIR)

disasm: $(ELF)
	avr-objdump -d $(ELF)

# Build directory prerequisites
$(OBJS): | $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

