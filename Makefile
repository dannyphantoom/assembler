# Multi-Architecture Assembler Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
INCLUDES = -Iinclude
LDFLAGS =

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = build
BINDIR = bin

# Target name
TARGET = assembler

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Default target
all: $(BINDIR)/$(TARGET)

# Create directories if they don't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Build the main executable
$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "Clean complete"

# Install (copy to /usr/local/bin)
install: $(BINDIR)/$(TARGET)
	sudo cp $(BINDIR)/$(TARGET) /usr/local/bin/
	@echo "Installed $(TARGET) to /usr/local/bin/"

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstalled $(TARGET)"

# Run tests with sample assembly files
test: $(BINDIR)/$(TARGET)
	@echo "Running basic tests..."
	@echo "Creating test assembly file..."
	@echo -e "mov rax, 0x42\nmov rbx, rax\nnop\nret" > test.asm
	./$(BINDIR)/$(TARGET) -a x86_64 -f bin -o test.bin test.asm
	@echo "Test completed. Check test.bin for output."
	@rm -f test.asm

# Debug build
debug: CFLAGS += -DDEBUG -g3 -O0
debug: $(BINDIR)/$(TARGET)

# Release build
release: CFLAGS += -DNDEBUG -O3
release: $(BINDIR)/$(TARGET)

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the assembler (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  uninstall- Remove from /usr/local/bin"
	@echo "  test     - Run basic functionality test"
	@echo "  debug    - Build with debug symbols"
	@echo "  release  - Build optimized release version"
	@echo "  help     - Show this help message"

# Dependencies
$(OBJDIR)/main.o: $(INCDIR)/assembler.h $(INCDIR)/lexer.h $(INCDIR)/parser.h
$(OBJDIR)/assembler.o: $(INCDIR)/assembler.h $(INCDIR)/lexer.h $(INCDIR)/parser.h $(INCDIR)/instruction.h
$(OBJDIR)/lexer.o: $(INCDIR)/lexer.h
$(OBJDIR)/parser.o: $(INCDIR)/parser.h $(INCDIR)/lexer.h $(INCDIR)/instruction.h $(INCDIR)/symbol_table.h
$(OBJDIR)/instruction.o: $(INCDIR)/instruction.h $(INCDIR)/assembler.h $(INCDIR)/lexer.h
$(OBJDIR)/symbol_table.o: $(INCDIR)/symbol_table.h

.PHONY: all clean install uninstall test debug release help 