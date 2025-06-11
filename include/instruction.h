#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include <stdbool.h>
#include "assembler.h"

// Operand types
typedef enum {
    OPERAND_NONE,
    OPERAND_REGISTER,
    OPERAND_IMMEDIATE,
    OPERAND_MEMORY,
    OPERAND_LABEL
} operand_type_t;

// Register encoding
typedef struct {
    char* name;
    uint8_t encoding;
    int size_bits;
    arch_type_t arch;
} register_info_t;

// Operand structure
typedef struct {
    operand_type_t type;
    union {
        struct {
            char* name;
            register_info_t* reg_info;
        } reg;
        struct {
            uint64_t value;
            int size_bits;
        } imm;
        struct {
            register_info_t* base;
            register_info_t* index;
            int scale;
            int64_t displacement;
            int size_bits;
        } mem;
        struct {
            char* name;
        } label;
    } data;
} operand_t;

// Instruction structure
typedef struct {
    char* mnemonic;
    operand_t operands[3];
    int operand_count;
    int line;
    int column;
} instruction_t;

// Function declarations
instruction_t* instruction_create(const char* mnemonic);
void instruction_destroy(instruction_t* instr);
void instruction_add_operand(instruction_t* instr, operand_t* operand);
operand_t* operand_create_register(const char* reg_name, arch_type_t arch);
operand_t* operand_create_immediate(uint64_t value, int size_bits);
operand_t* operand_create_memory(register_info_t* base, register_info_t* index, 
                                int scale, int64_t displacement, int size_bits);
operand_t* operand_create_label(const char* label_name);
void operand_destroy(operand_t* operand);

// Instruction encoding
int encode_instruction(instruction_t* instr, arch_type_t arch, uint8_t* output, int max_size);

#endif // INSTRUCTION_H 