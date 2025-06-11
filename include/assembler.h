#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// Architecture types
typedef enum {
    ARCH_X86_16,
    ARCH_X86_32,
    ARCH_X86_64,
    ARCH_ARM_32,
    ARCH_ARM_64
} arch_type_t;

// Output formats
typedef enum {
    FORMAT_ELF,
    FORMAT_PE,
    FORMAT_BIN
} output_format_t;

// Main assembler context
typedef struct {
    arch_type_t architecture;
    output_format_t output_format;
    const char* input_file;
    const char* output_file;
    bool debug_mode;
} assembler_context_t;

// Function declarations
int assemble_file(assembler_context_t* ctx);
void print_usage(const char* program_name);
int parse_arguments(int argc, char* argv[], assembler_context_t* ctx);

#endif // ASSEMBLER_H 