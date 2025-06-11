# Multi-Architecture Assembler

A multi-architecture assembler that supports Intel syntax for both x86 and ARM architectures in 16, 32, and 64-bit modes, written in C.

## Features

- **Intel Syntax Support**: Familiar Intel assembly syntax
- **Multi-Architecture**: Support for x86 (16/32/64-bit) and ARM (32/64-bit)
- **Multiple Output Formats**: Binary, ELF, and PE formats
- **Symbol Table Management**: Labels and symbol resolution
- **Comprehensive Error Reporting**: Line numbers and descriptive error messages
- **Modular Design**: Clean separation of lexing, parsing, and code generation

## Current Status

### Implemented Features
- ✅ Complete lexical analysis (tokenizer)
- ✅ Intel syntax parser for basic instructions
- ✅ Symbol table with label support
- ✅ Basic x86-64 instruction encoding (MOV, NOP, RET)
- ✅ Command-line interface with multiple options
- ✅ Binary output format
- ✅ Register recognition for x86/x64

### In Progress / TODO
- 🔄 Extended x86 instruction set (ADD, SUB, JMP, etc.)
- 🔄 Memory operand parsing and encoding
- 🔄 ARM instruction support
- 🔄 ELF and PE output format support
- 🔄 Directive support (.data, .text, .section)
- 🔄 More complex addressing modes

## Architecture Support

| Architecture | Status | Notes |
|-------------|--------|-------|
| x86-16      | 🔄 Partial | Basic framework in place |
| x86-32      | 🔄 Partial | Basic framework in place |
| x86-64      | ✅ Basic | MOV, NOP, RET instructions working |
| ARM-32      | 📋 Planned | Framework ready, encoding TODO |
| ARM-64      | 📋 Planned | Framework ready, encoding TODO |

## Building

### Prerequisites
- GCC compiler
- Make
- Linux/Unix environment (tested on Arch Linux)

### Build Instructions
```bash
# Build the assembler
make

# Build with debug symbols
make debug

# Build optimized release
make release

# Clean build artifacts
make clean

# Install to /usr/local/bin
make install

# Run basic test
make test
```

## Usage

```bash
# Basic usage
./bin/assembler input.asm

# Specify architecture and output format
./bin/assembler -a x86_64 -f bin -o output.bin input.asm

# Enable debug mode
./bin/assembler -d -a x86_64 input.asm

# Show help
./bin/assembler -h
```

### Command Line Options

| Option | Description | Values |
|--------|-------------|---------|
| `-a, --arch` | Target architecture | `x86_16`, `x86_32`, `x86_64`, `arm_32`, `arm_64` |
| `-f, --format` | Output format | `bin`, `elf`, `pe` |
| `-o, --output` | Output file | Filename (auto-generated if not specified) |
| `-d, --debug` | Enable debug mode | Flag |
| `-h, --help` | Show help message | Flag |

## Assembly Syntax

The assembler supports Intel syntax assembly language.

### Example Assembly Code

```assembly
; Simple x86-64 program
main:
    mov rax, 0x1234567890ABCDEF  ; 64-bit immediate to register
    mov rbx, rax                 ; register to register transfer
    mov ecx, 42                  ; 32-bit immediate to register
    nop                          ; no operation
    ret                          ; return
```

### Supported Instructions (x86-64)

| Instruction | Description | Example |
|-------------|-------------|---------|
| `mov` | Move data | `mov rax, 42`, `mov rbx, rax` |
| `nop` | No operation | `nop` |
| `ret` | Return | `ret` |

*More instructions coming soon...*

### Supported Registers (x86-64)

#### 64-bit Registers
`rax`, `rbx`, `rcx`, `rdx`, `rsi`, `rdi`, `rbp`, `rsp`, `r8`-`r15`

#### 32-bit Registers  
`eax`, `ebx`, `ecx`, `edx`, `esi`, `edi`, `ebp`, `esp`, `r8d`-`r15d`

#### 16-bit Registers
`ax`, `bx`, `cx`, `dx`, `si`, `di`, `bp`, `sp`, `r8w`-`r15w`

#### 8-bit Registers
`al`, `bl`, `cl`, `dl`, `ah`, `bh`, `ch`, `dh`, `sil`, `dil`, `bpl`, `spl`, `r8b`-`r15b`

## Project Structure

```
assembler/
├── include/           # Header files
│   ├── assembler.h   # Main assembler definitions
│   ├── lexer.h       # Tokenizer definitions
│   ├── parser.h      # Parser definitions
│   ├── instruction.h # Instruction handling
│   └── symbol_table.h# Symbol management
├── src/              # Source files
│   ├── main.c        # Entry point and CLI
│   ├── assembler.c   # Core assembler logic
│   ├── lexer.c       # Lexical analysis
│   ├── parser.c      # Syntax analysis
│   ├── instruction.c # Instruction encoding
│   └── symbol_table.c# Symbol table management
├── examples/         # Example assembly files
│   └── hello.asm     # Simple example
├── Makefile          # Build configuration
└── README.md         # This file
```

## Contributing

This is an educational/hobby project. Contributions are welcome!

### Areas that need work:
1. **Extended Instruction Set**: More x86 instructions
2. **ARM Support**: Complete ARM instruction encoding
3. **Memory Operands**: Complex addressing modes
4. **Output Formats**: ELF and PE file generation
5. **Optimization**: Better code generation
6. **Testing**: More comprehensive test suite

## License

This project is open source. Feel free to use, modify, and distribute.

## Acknowledgments

This assembler project is designed to be educational and demonstrate:
- Compiler/assembler design principles
- Multi-architecture code generation
- Lexical analysis and parsing techniques
- Symbol table management
- Machine code generation 