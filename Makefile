# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = 

# Directories
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/program

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files to object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BIN_DIR)/*.o $(TARGET)
	@echo "Clean complete"

# Remove all generated files
distclean: clean
	rm -rf $(BIN_DIR)
	
# Run the program
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean distclean