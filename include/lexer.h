#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Token types
typedef enum {
    TOKEN_EOF,
    TOKEN_NEWLINE,
    TOKEN_LABEL,
    TOKEN_INSTRUCTION,
    TOKEN_REGISTER,
    TOKEN_IMMEDIATE,
    TOKEN_MEMORY,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_DIRECTIVE,
    TOKEN_COMMENT,
    TOKEN_DOT,
    TOKEN_DWORD_PTR,
    TOKEN_QWORD_PTR,
    TOKEN_BYTE_PTR,
    TOKEN_WORD_PTR,
    TOKEN_UNKNOWN
} token_type_t;

// Token structure
typedef struct {
    token_type_t type;
    char* value;
    int line;
    int column;
    uint64_t numeric_value;
} token_t;

// Lexer state
typedef struct {
    FILE* file;
    char* buffer;
    size_t buffer_size;
    size_t position;
    int line;
    int column;
    bool eof_reached;
} lexer_t;

// Function declarations
lexer_t* lexer_create(FILE* file);
void lexer_destroy(lexer_t* lexer);
token_t* lexer_next_token(lexer_t* lexer);
void token_destroy(token_t* token);
bool is_register(const char* str);
bool is_instruction(const char* str);
bool is_directive(const char* str);

#endif // LEXER_H 