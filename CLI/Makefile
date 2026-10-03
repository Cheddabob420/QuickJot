# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L

# Output binary name and source files
TARGET = quickjot
SRC = main.c

# Default rule built when you run 'make'
all: $(TARGET)

# Compile the executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Build and run the executable
run: $(TARGET)
	./$(TARGET)

# Clean up built binaries
clean:
	rm -f $(TARGET)

.PHONY: all run clean
