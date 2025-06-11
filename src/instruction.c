#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "../include/instruction.h"
#include "../include/lexer.h"

// Register information tables
static register_info_t x86_64_registers[] = {
    // 8-bit registers
    {"al", 0, 8, ARCH_X86_64}, {"cl", 1, 8, ARCH_X86_64}, {"dl", 2, 8, ARCH_X86_64}, {"bl", 3, 8, ARCH_X86_64},
    {"ah", 4, 8, ARCH_X86_64}, {"ch", 5, 8, ARCH_X86_64}, {"dh", 6, 8, ARCH_X86_64}, {"bh", 7, 8, ARCH_X86_64},
    
    // 16-bit registers
    {"ax", 0, 16, ARCH_X86_64}, {"cx", 1, 16, ARCH_X86_64}, {"dx", 2, 16, ARCH_X86_64}, {"bx", 3, 16, ARCH_X86_64},
    {"sp", 4, 16, ARCH_X86_64}, {"bp", 5, 16, ARCH_X86_64}, {"si", 6, 16, ARCH_X86_64}, {"di", 7, 16, ARCH_X86_64},
    
    // 32-bit registers
    {"eax", 0, 32, ARCH_X86_64}, {"ecx", 1, 32, ARCH_X86_64}, {"edx", 2, 32, ARCH_X86_64}, {"ebx", 3, 32, ARCH_X86_64},
    {"esp", 4, 32, ARCH_X86_64}, {"ebp", 5, 32, ARCH_X86_64}, {"esi", 6, 32, ARCH_X86_64}, {"edi", 7, 32, ARCH_X86_64},
    
    // 64-bit registers
    {"rax", 0, 64, ARCH_X86_64}, {"rcx", 1, 64, ARCH_X86_64}, {"rdx", 2, 64, ARCH_X86_64}, {"rbx", 3, 64, ARCH_X86_64},
    {"rsp", 4, 64, ARCH_X86_64}, {"rbp", 5, 64, ARCH_X86_64}, {"rsi", 6, 64, ARCH_X86_64}, {"rdi", 7, 64, ARCH_X86_64},
    {"r8", 8, 64, ARCH_X86_64}, {"r9", 9, 64, ARCH_X86_64}, {"r10", 10, 64, ARCH_X86_64}, {"r11", 11, 64, ARCH_X86_64},
    {"r12", 12, 64, ARCH_X86_64}, {"r13", 13, 64, ARCH_X86_64}, {"r14", 14, 64, ARCH_X86_64}, {"r15", 15, 64, ARCH_X86_64},
    
    {NULL, 0, 0, 0} // Sentinel
};

static register_info_t* find_register_info(const char* reg_name, arch_type_t arch) {
    register_info_t* regs = NULL;
    
    switch (arch) {
        case ARCH_X86_16:
        case ARCH_X86_32:
        case ARCH_X86_64:
            regs = x86_64_registers;
            break;
        case ARCH_ARM_32:
        case ARCH_ARM_64:
            // TODO: Implement ARM register tables
            return NULL;
        default:
            return NULL;
    }
    
    for (int i = 0; regs[i].name; i++) {
        if (strcasecmp(regs[i].name, reg_name) == 0) {
            return &regs[i];
        }
    }
    
    return NULL;
}

instruction_t* instruction_create(const char* mnemonic) {
    instruction_t* instr = malloc(sizeof(instruction_t));
    if (!instr) return NULL;
    
    instr->mnemonic = strdup(mnemonic);
    if (!instr->mnemonic) {
        free(instr);
        return NULL;
    }
    
    instr->operand_count = 0;
    instr->line = 0;
    instr->column = 0;
    
    // Initialize operands
    for (int i = 0; i < 3; i++) {
        instr->operands[i].type = OPERAND_NONE;
    }
    
    return instr;
}

void instruction_destroy(instruction_t* instr) {
    if (!instr) return;
    
    free(instr->mnemonic);
    
    for (int i = 0; i < instr->operand_count; i++) {
        operand_destroy(&instr->operands[i]);
    }
    
    free(instr);
}

void instruction_add_operand(instruction_t* instr, operand_t* operand) {
    if (!instr || !operand || instr->operand_count >= 3) return;
    
    instr->operands[instr->operand_count] = *operand;
    instr->operand_count++;
}

operand_t* operand_create_register(const char* reg_name, arch_type_t arch) {
    operand_t* operand = malloc(sizeof(operand_t));
    if (!operand) return NULL;
    
    operand->type = OPERAND_REGISTER;
    operand->data.reg.name = strdup(reg_name);
    operand->data.reg.reg_info = find_register_info(reg_name, arch);
    
    if (!operand->data.reg.name) {
        free(operand);
        return NULL;
    }
    
    return operand;
}

operand_t* operand_create_immediate(uint64_t value, int size_bits) {
    operand_t* operand = malloc(sizeof(operand_t));
    if (!operand) return NULL;
    
    operand->type = OPERAND_IMMEDIATE;
    operand->data.imm.value = value;
    operand->data.imm.size_bits = size_bits;
    
    return operand;
}

operand_t* operand_create_memory(register_info_t* base, register_info_t* index, 
                                int scale, int64_t displacement, int size_bits) {
    operand_t* operand = malloc(sizeof(operand_t));
    if (!operand) return NULL;
    
    operand->type = OPERAND_MEMORY;
    operand->data.mem.base = base;
    operand->data.mem.index = index;
    operand->data.mem.scale = scale;
    operand->data.mem.displacement = displacement;
    operand->data.mem.size_bits = size_bits;
    
    return operand;
}

operand_t* operand_create_label(const char* label_name) {
    operand_t* operand = malloc(sizeof(operand_t));
    if (!operand) return NULL;
    
    operand->type = OPERAND_LABEL;
    operand->data.label.name = strdup(label_name);
    
    if (!operand->data.label.name) {
        free(operand);
        return NULL;
    }
    
    return operand;
}

void operand_destroy(operand_t* operand) {
    if (!operand) return;
    
    switch (operand->type) {
        case OPERAND_REGISTER:
            free(operand->data.reg.name);
            break;
        case OPERAND_LABEL:
            free(operand->data.label.name);
            break;
        default:
            break;
    }
}

// Basic x86-64 instruction encoding
static int encode_x86_mov(instruction_t* instr, uint8_t* output, int max_size) {
    if (instr->operand_count != 2) return -1;
    
    operand_t* dst = &instr->operands[0];
    operand_t* src = &instr->operands[1];
    
    // MOV reg, imm32/64
    if (dst->type == OPERAND_REGISTER && src->type == OPERAND_IMMEDIATE) {
        if (dst->data.reg.reg_info && dst->data.reg.reg_info->size_bits == 64) {
            if (max_size < 10) return -1;
            
            // REX.W prefix for 64-bit
            output[0] = 0x48;
            
            // MOV opcode + register encoding
            output[1] = 0xB8 + dst->data.reg.reg_info->encoding;
            
            // Immediate value (little-endian)
            uint64_t imm = src->data.imm.value;
            for (int i = 0; i < 8; i++) {
                output[2 + i] = (imm >> (i * 8)) & 0xFF;
            }
            
            return 10;
        } else if (dst->data.reg.reg_info && dst->data.reg.reg_info->size_bits == 32) {
            if (max_size < 5) return -1;
            
            // MOV opcode + register encoding
            output[0] = 0xB8 + dst->data.reg.reg_info->encoding;
            
            // Immediate value (little-endian)
            uint32_t imm = (uint32_t)src->data.imm.value;
            for (int i = 0; i < 4; i++) {
                output[1 + i] = (imm >> (i * 8)) & 0xFF;
            }
            
            return 5;
        }
    }
    
    // MOV reg, reg
    if (dst->type == OPERAND_REGISTER && src->type == OPERAND_REGISTER) {
        if (dst->data.reg.reg_info && src->data.reg.reg_info && 
            dst->data.reg.reg_info->size_bits == 64) {
            if (max_size < 3) return -1;
            
            // REX.W prefix
            output[0] = 0x48;
            
            // MOV r/m64, r64 opcode
            output[1] = 0x89;
            
            // ModR/M byte: 11 (register mode) + src_reg * 8 + dst_reg
            output[2] = 0xC0 + (src->data.reg.reg_info->encoding << 3) + 
                       dst->data.reg.reg_info->encoding;
            
            return 3;
        }
    }
    
    return -1; // Unsupported operand combination
}

static int encode_x86_nop(instruction_t* instr, uint8_t* output, int max_size) {
    if (instr->operand_count != 0) return -1;
    if (max_size < 1) return -1;
    
    output[0] = 0x90;
    return 1;
}

static int encode_x86_ret(instruction_t* instr, uint8_t* output, int max_size) {
    if (instr->operand_count != 0) return -1;
    if (max_size < 1) return -1;
    
    output[0] = 0xC3;
    return 1;
}

int encode_instruction(instruction_t* instr, arch_type_t arch, uint8_t* output, int max_size) {
    if (!instr || !output || max_size <= 0) return -1;
    
    switch (arch) {
        case ARCH_X86_16:
        case ARCH_X86_32:
        case ARCH_X86_64:
            // Basic x86 instruction encoding
            if (strcasecmp(instr->mnemonic, "mov") == 0) {
                return encode_x86_mov(instr, output, max_size);
            } else if (strcasecmp(instr->mnemonic, "nop") == 0) {
                return encode_x86_nop(instr, output, max_size);
            } else if (strcasecmp(instr->mnemonic, "ret") == 0) {
                return encode_x86_ret(instr, output, max_size);
            } else {
                // Unsupported instruction - output NOP as placeholder
                if (max_size >= 1) {
                    output[0] = 0x90;
                    return 1;
                }
                return -1;
            }
            break;
            
        case ARCH_ARM_32:
        case ARCH_ARM_64:
            // TODO: Implement ARM instruction encoding
            return -1;
            
        default:
            return -1;
    }
    
    return -1;
} 