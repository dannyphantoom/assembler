#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdint.h>
#include "../include/lexer.h"

#define BUFFER_SIZE 4096

// x86/x64 registers
static const char* x86_registers[] = {
    // 8-bit registers
    "al", "bl", "cl", "dl", "ah", "bh", "ch", "dh",
    "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b",
    "sil", "dil", "bpl", "spl",
    
    // 16-bit registers  
    "ax", "bx", "cx", "dx", "si", "di", "bp", "sp",
    "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w",
    
    // 32-bit registers
    "eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp",
    "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d",
    
    // 64-bit registers
    "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp",
    "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
    
    // Segment registers
    "cs", "ds", "es", "fs", "gs", "ss",
    
    // Control registers
    "cr0", "cr1", "cr2", "cr3", "cr4", "cr8",
    
    // Debug registers
    "dr0", "dr1", "dr2", "dr3", "dr6", "dr7",
    
    NULL
};

// Common x86 instructions
static const char* x86_instructions[] = {
    "mov", "add", "sub", "mul", "div", "inc", "dec",
    "push", "pop", "call", "ret", "jmp", "je", "jne",
    "jz", "jnz", "jl", "jle", "jg", "jge", "ja", "jae",
    "jb", "jbe", "js", "jns", "jo", "jno", "jc", "jnc",
    "cmp", "test", "and", "or", "xor", "not", "shl",
    "shr", "sal", "sar", "rol", "ror", "rcl", "rcr",
    "lea", "nop", "int", "iret", "hlt", "cli", "sti",
    NULL
};

lexer_t* lexer_create(FILE* file) {
    lexer_t* lexer = malloc(sizeof(lexer_t));
    if (!lexer) return NULL;
    
    lexer->file = file;
    lexer->buffer = malloc(BUFFER_SIZE);
    if (!lexer->buffer) {
        free(lexer);
        return NULL;
    }
    
    lexer->buffer_size = BUFFER_SIZE;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->eof_reached = false;
    
    // Read initial buffer
    size_t bytes_read = fread(lexer->buffer, 1, BUFFER_SIZE, file);
    if (bytes_read < BUFFER_SIZE) {
        lexer->buffer[bytes_read] = '\0';
        lexer->eof_reached = true;
    }
    
    return lexer;
}

void lexer_destroy(lexer_t* lexer) {
    if (lexer) {
        free(lexer->buffer);
        free(lexer);
    }
}

bool is_register(const char* str) {
    for (int i = 0; x86_registers[i]; i++) {
        if (strcasecmp(str, x86_registers[i]) == 0) {
            return true;
        }
    }
    return false;
}

bool is_instruction(const char* str) {
    for (int i = 0; x86_instructions[i]; i++) {
        if (strcasecmp(str, x86_instructions[i]) == 0) {
            return true;
        }
    }
    return false;
}

static char lexer_peek(lexer_t* lexer) {
    if (lexer->position >= lexer->buffer_size || lexer->eof_reached) {
        return '\0';
    }
    return lexer->buffer[lexer->position];
}

static char lexer_advance_char(lexer_t* lexer) {
    if (lexer->position >= lexer->buffer_size || lexer->eof_reached) {
        return '\0';
    }
    
    char c = lexer->buffer[lexer->position++];
    
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    
    return c;
}

static void lexer_skip_whitespace(lexer_t* lexer) {
    while (lexer_peek(lexer) == ' ' || lexer_peek(lexer) == '\t') {
        lexer_advance_char(lexer);
    }
}

static void lexer_skip_comment(lexer_t* lexer) {
    if (lexer_peek(lexer) == ';') {
        while (lexer_peek(lexer) != '\n' && lexer_peek(lexer) != '\0') {
            lexer_advance_char(lexer);
        }
    }
}

static token_t* token_create(token_type_t type, const char* value, int line, int column) {
    token_t* token = malloc(sizeof(token_t));
    if (!token) return NULL;
    
    token->type = type;
    token->value = value ? strdup(value) : NULL;
    token->line = line;
    token->column = column;
    token->numeric_value = 0;
    
    return token;
}

void token_destroy(token_t* token) {
    if (token) {
        free(token->value);
        free(token);
    }
}

static token_t* lexer_read_string(lexer_t* lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    char buffer[256];
    int pos = 0;
    
    while (isalnum(lexer_peek(lexer)) || lexer_peek(lexer) == '_') {
        if (pos < 255) {
            buffer[pos++] = lexer_advance_char(lexer);
        } else {
            lexer_advance_char(lexer);
        }
    }
    buffer[pos] = '\0';
    
    // Determine token type
    token_type_t type = TOKEN_IDENTIFIER;
    if (is_register(buffer)) {
        type = TOKEN_REGISTER;
    } else if (is_instruction(buffer)) {
        type = TOKEN_INSTRUCTION;
    }
    
    return token_create(type, buffer, start_line, start_column);
}

static token_t* lexer_read_number(lexer_t* lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    char buffer[256];
    int pos = 0;
    uint64_t value = 0;
    int base = 10;
    
    // Check for hex prefix
    if (lexer_peek(lexer) == '0') {
        buffer[pos++] = lexer_advance_char(lexer);
        if (lexer_peek(lexer) == 'x' || lexer_peek(lexer) == 'X') {
            buffer[pos++] = lexer_advance_char(lexer);
            base = 16;
        }
    }
    
    while ((base == 16 && isxdigit(lexer_peek(lexer))) || 
           (base == 10 && isdigit(lexer_peek(lexer)))) {
        if (pos < 255) {
            buffer[pos++] = lexer_advance_char(lexer);
        } else {
            lexer_advance_char(lexer);
        }
    }
    buffer[pos] = '\0';
    
    // Convert to number
    if (base == 16) {
        value = strtoull(buffer + 2, NULL, 16); // Skip "0x"
    } else {
        value = strtoull(buffer, NULL, 10);
    }
    
    token_t* token = token_create(TOKEN_NUMBER, buffer, start_line, start_column);
    if (token) {
        token->numeric_value = value;
    }
    return token;
}

token_t* lexer_next_token(lexer_t* lexer) {
    lexer_skip_whitespace(lexer);
    
    char c = lexer_peek(lexer);
    int line = lexer->line;
    int column = lexer->column;
    
    // End of file
    if (c == '\0') {
        return token_create(TOKEN_EOF, NULL, line, column);
    }
    
    // Newline
    if (c == '\n') {
        lexer_advance_char(lexer);
        return token_create(TOKEN_NEWLINE, "\n", line, column);
    }
    
    // Comment
    if (c == ';') {
        lexer_skip_comment(lexer);
        return token_create(TOKEN_COMMENT, ";", line, column);
    }
    
    // Single character tokens
    switch (c) {
        case ',':
            lexer_advance_char(lexer);
            return token_create(TOKEN_COMMA, ",", line, column);
        case ':':
            lexer_advance_char(lexer);
            return token_create(TOKEN_COLON, ":", line, column);
        case '[':
            lexer_advance_char(lexer);
            return token_create(TOKEN_LBRACKET, "[", line, column);
        case ']':
            lexer_advance_char(lexer);
            return token_create(TOKEN_RBRACKET, "]", line, column);
        case '+':
            lexer_advance_char(lexer);
            return token_create(TOKEN_PLUS, "+", line, column);
        case '-':
            lexer_advance_char(lexer);
            return token_create(TOKEN_MINUS, "-", line, column);
        case '*':
            lexer_advance_char(lexer);
            return token_create(TOKEN_MULTIPLY, "*", line, column);
    }
    
    // Numbers
    if (isdigit(c)) {
        return lexer_read_number(lexer);
    }
    
    // Identifiers, registers, instructions
    if (isalpha(c) || c == '_') {
        return lexer_read_string(lexer);
    }
    
    // Unknown character
    lexer_advance_char(lexer);
    char unknown[2] = {c, '\0'};
    return token_create(TOKEN_UNKNOWN, unknown, line, column);
} 