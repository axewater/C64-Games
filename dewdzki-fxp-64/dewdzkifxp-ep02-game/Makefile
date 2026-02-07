# Dewdzki FXP - Episode 2 Game - Makefile
CC := C:/bin/cc65/bin/cl65.exe
TARGET := c64
PROGRAM := ep02-game.prg
BUILD_DIR := build
SRC_DIR := src

# Compiler flags
CFLAGS := -t $(TARGET) -O -Or -Cl -I$(SRC_DIR)
LDFLAGS := -t $(TARGET) -m $(BUILD_DIR)/ep02-game.map

# Source files
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Default target
all: $(BUILD_DIR)/$(PROGRAM)

# Link program
$(BUILD_DIR)/$(PROGRAM): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo Build complete: $(BUILD_DIR)/$(PROGRAM)

# Compile C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Run in VICE emulator
run: $(BUILD_DIR)/$(PROGRAM)
	x64sc $(BUILD_DIR)/$(PROGRAM)

# Clean build artifacts
clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all run clean
