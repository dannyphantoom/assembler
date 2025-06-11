#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "instruction.h"
#include "symbol_table.h"
#include "assembler.h"

// Parser state
typedef struct {
    lexer_t* lexer;
    token_t* current_token;
    symbol_table_t* symbol_table;
    arch_type_t architecture;
    uint64_t current_address;
    int current_section;
    bool has_error;
    char error_message[256];
} parser_t;

// Parsed program structure
typedef struct {
    instruction_t** instructions;
    int instruction_count;
    int instruction_capacity;
    symbol_table_t* symbols;
    uint8_t* code;
    size_t code_size;
} program_t;

// Function declarations
parser_t* parser_create(lexer_t* lexer, arch_type_t arch);
void parser_destroy(parser_t* parser);
program_t* parser_parse(parser_t* parser);
void program_destroy(program_t* program);

// Parsing functions
instruction_t* parse_instruction(parser_t* parser);
operand_t* parse_operand(parser_t* parser);
bool parse_label(parser_t* parser);
bool parse_directive(parser_t* parser);

// Utility functions
void parser_error(parser_t* parser, const char* message);
bool parser_expect_token(parser_t* parser, token_type_t expected);
void parser_advance(parser_t* parser);

#endif // PARSER_H 