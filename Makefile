# Important directories
<<<<<<< HEAD
INC_DIR = ./include/
SRC_DIR = ./src/
LIB_DIR = ./lib/
BUILD_DIR = ./build/
=======
INC_DIR 	= ./include/
SRC_DIR 	= ./src/
LIB_DIR 	= ./libs/
BUILD_DIR 	= ./build/
>>>>>>> refs/remotes/origin/master

# Change for different avrs and clock speeds
DEVICE 		= atmega32u4
CLOCK 		= 16000000

# Compiler settings
CC 			= avr-gcc
CFLAGS 		= -mmcu=$(DEVICE) -DF_CPU=$(CLOCK) -std=c99 -Wall -Os -lm -MMD -MP

# Create targets
# Either manually define sources, or generate list from SRC_DIR
# SRCS = $(addprefix $(SRC_DIR), main.c)
SRC = $(wildcard $(SRC_DIR)*.c)
LIB_SRC = $(wildcard $(LIB_DIR)*.c)
LIB_A = $(wildcard $(LIB_DIR)*.a)
OBJ = $(subst $(SRC_DIR), $(BUILD_DIR), $(SRC:.c=.o))
LIB_OBJ = $(subst $(LIB_DIR), $(BUILD_DIR), $(LIB_SRC:.c=.o))
DEPS = $(OBJ:.o=.d)
ELF = $(BUILD_DIR)main.elf
HEX = $(BUILD_DIR)main.hex

all: $(HEX)

-include $(DEPS)

$(BUILD_DIR)%.o: $(LIB_DIR)%.c
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@ 

$(BUILD_DIR)%.o: $(SRC_DIR)%.c $(LIB_OBJ)
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@ 	

$(ELF): $(OBJ)
	$(CC) $(CFLAGS) -o $(ELF) $(OBJ) $(LIB_OBJ) $(LIB_A)

$(HEX): $(ELF)
	rm -f $(HEX)
	avr-objcopy -j .text -j .data -O ihex $(ELF) $(HEX)

.PHONY: flash
flash: all
	dfu-programmer atmega32u4 erase
	dfu-programmer atmega32u4 flash $(HEX)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: disasm
disasm: $(ELF)
	avr-objdump -d $(ELF)

# Build directory prerequisites
$(OBJ) $(LIB_OBJ): $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

