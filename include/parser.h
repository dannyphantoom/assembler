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

// Section types
typedef enum {
    SECTION_TEXT,
    SECTION_DATA,
    SECTION_BSS
} section_type_t;

// Data definition types
typedef enum {
    DATA_BYTE,    // db
    DATA_WORD,    // dw
    DATA_DWORD,   // dd
    DATA_QWORD    // dq
} data_type_t;

// Data definition structure
typedef struct {
    data_type_t type;
    union {
        uint8_t byte_value;
        uint16_t word_value;
        uint32_t dword_value;
        uint64_t qword_value;
        char* string_value;
    } data;
    size_t repeat_count;  // for resb, resw, etc.
} data_definition_t;

// Parsed program structure
typedef struct {
    instruction_t** instructions;
    int instruction_count;
    int instruction_capacity;
    data_definition_t** data_definitions;
    int data_count;
    int data_capacity;
    symbol_table_t* symbols;
    uint8_t* code;
    size_t code_size;
    uint8_t* data_section;
    size_t data_size;
    section_type_t current_section;
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
bool parse_section_directive(parser_t* parser);
data_definition_t* parse_data_definition(parser_t* parser);
void data_definition_destroy(data_definition_t* data_def);

// Utility functions
void parser_error(parser_t* parser, const char* message);
bool parser_expect_token(parser_t* parser, token_type_t expected);
void parser_advance(parser_t* parser);

#endif // PARSER_H 