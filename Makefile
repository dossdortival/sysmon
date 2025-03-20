# sysmon - Interactive System Monitor Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -O2
LDFLAGS = -lncurses -lm

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/collector/cpu_collector.c \
       $(SRC_DIR)/collector/memory_collector.c \
       $(SRC_DIR)/ui/ui_manager.c \
       $(SRC_DIR)/util/error_handler.c

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Output binary
TARGET = $(BIN_DIR)/sysmon

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	mkdir -p $(BUILD_DIR)/collector
	mkdir -p $(BUILD_DIR)/ui
	mkdir -p $(BUILD_DIR)/util
	mkdir -p $(BIN_DIR)

# Link the final binary
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run the application
run: all
	$(TARGET)

# Install to system
install: all
	mkdir -p $(DESTDIR)/usr/local/bin
	cp $(TARGET) $(DESTDIR)/usr/local/bin/sysmon

# Uninstall from system
uninstall:
	rm -f $(DESTDIR)/usr/local/bin/sysmon

.PHONY: all clean run install uninstall directories