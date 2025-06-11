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
- ✅ Intel syntax parser for instructions and basic directives
- ✅ Symbol table with label support
- ✅ Extended x86-64 instruction encoding (MOV, ADD, SUB, CMP, JMP, conditional jumps)
- ✅ Jump and conditional branch instructions (JE, JNE, JL, JG, etc.)
- ✅ Section directive recognition (.text, .data, .bss)
- ✅ Basic data definition directives (db, dw, dd, dq, resb, etc.)
- ✅ Label definitions and references
- ✅ Command-line interface with multiple options
- ✅ Binary output format
- ✅ Register recognition for x86/x64 (8, 16, 32, 64-bit)

### In Progress / TODO
- 🔄 Memory operand encoding (basic framework ready)
- 🔄 Data section generation and linking
- 🔄 String literal support in data definitions
- 🔄 Complex addressing modes ([base + index*scale + displacement])
- 🔄 More x86 instructions (PUSH, POP, CALL, etc.)
- 🔄 ARM instruction support
- 🔄 ELF and PE output format support
- 🔄 Proper label offset calculation for jumps

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
; Simple x86-64 program with control flow
.text
main:
    mov rax, 100                 ; Load immediate value
    mov rbx, rax                 ; Copy to another register
    add rax, 10                  ; Add immediate
    sub rbx, 5                   ; Subtract immediate
    cmp rax, rbx                 ; Compare values
    
    je equal                     ; Jump if equal
    jg greater                   ; Jump if greater
    
equal:
    mov rcx, 1                   ; Set result
    jmp end                      ; Unconditional jump
    
greater:
    mov rcx, 2                   ; Set different result
    
end:
    nop                          ; No operation
    ret                          ; Return

.data
    counter dq 42                ; 64-bit data
    buffer  resb 64              ; Reserve 64 bytes
```

### Supported Instructions (x86-64)

| Instruction | Description | Example |
|-------------|-------------|---------|
| `mov` | Move data | `mov rax, 42`, `mov rbx, rax` |
| `add` | Add values | `add rax, 10` |
| `sub` | Subtract values | `sub rbx, 5` |
| `cmp` | Compare values | `cmp rax, rbx` |
| `jmp` | Unconditional jump | `jmp label` |
| `je`/`jz` | Jump if equal/zero | `je equal_label` |
| `jne`/`jnz` | Jump if not equal/zero | `jne not_equal_label` |
| `jl` | Jump if less than | `jl less_label` |
| `jle` | Jump if less or equal | `jle less_equal_label` |
| `jg` | Jump if greater than | `jg greater_label` |
| `jge` | Jump if greater or equal | `jge greater_equal_label` |
| `nop` | No operation | `nop` |
| `ret` | Return | `ret` |

### Supported Directives

| Directive | Description | Example |
|-----------|-------------|---------|
| `.text` | Code section | `.text` |
| `.data` | Data section | `.data` |
| `.bss` | Uninitialized data section | `.bss` |
| `db` | Define byte(s) | `db 42`, `db 'H'` |
| `dw` | Define word(s) | `dw 1234` |
| `dd` | Define dword(s) | `dd 0x12345678` |
| `dq` | Define qword(s) | `dq 0x123456789ABCDEF0` |
| `resb` | Reserve bytes | `resb 64` |
| `resw` | Reserve words | `resw 32` |
| `resd` | Reserve dwords | `resd 16` |
| `resq` | Reserve qwords | `resq 8` |

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