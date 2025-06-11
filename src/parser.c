#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "../include/parser.h"

#define INITIAL_CAPACITY 256
#define MAX_CODE_SIZE 65536

parser_t* parser_create(lexer_t* lexer, arch_type_t arch) {
    parser_t* parser = malloc(sizeof(parser_t));
    if (!parser) return NULL;
    
    parser->lexer = lexer;
    parser->current_token = NULL;
    parser->symbol_table = symbol_table_create(256);
    parser->architecture = arch;
    parser->current_address = 0;
    parser->current_section = 0;
    parser->has_error = false;
    parser->error_message[0] = '\0';
    
    if (!parser->symbol_table) {
        free(parser);
        return NULL;
    }
    
    // Get first token
    parser_advance(parser);
    
    return parser;
}

void parser_destroy(parser_t* parser) {
    if (!parser) return;
    
    if (parser->current_token) {
        token_destroy(parser->current_token);
    }
    
    if (parser->symbol_table) {
        symbol_table_destroy(parser->symbol_table);
    }
    
    free(parser);
}

void parser_error(parser_t* parser, const char* message) {
    if (!parser) return;
    
    parser->has_error = true;
    snprintf(parser->error_message, sizeof(parser->error_message), 
             "Line %d: %s", 
             parser->current_token ? parser->current_token->line : 0,
             message);
}

bool parser_expect_token(parser_t* parser, token_type_t expected) {
    if (!parser->current_token) {
        parser_error(parser, "Unexpected end of file");
        return false;
    }
    
    if (parser->current_token->type != expected) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), 
                "Expected token type %d, got %d", expected, parser->current_token->type);
        parser_error(parser, error_msg);
        return false;
    }
    
    return true;
}

void parser_advance(parser_t* parser) {
    if (parser->current_token) {
        token_destroy(parser->current_token);
    }
    
    parser->current_token = lexer_next_token(parser->lexer);
}

static void skip_newlines(parser_t* parser) {
    while (parser->current_token && 
           (parser->current_token->type == TOKEN_NEWLINE || 
            parser->current_token->type == TOKEN_COMMENT)) {
        parser_advance(parser);
    }
}

operand_t* parse_operand(parser_t* parser) {
    if (!parser->current_token) return NULL;
    
    switch (parser->current_token->type) {
        case TOKEN_REGISTER: {
            // Register operand
            char* reg_name = strdup(parser->current_token->value);
            parser_advance(parser);
            
            operand_t* operand = operand_create_register(reg_name, parser->architecture);
            free(reg_name);
            return operand;
        }
        
        case TOKEN_NUMBER: {
            // Immediate operand
            uint64_t value = parser->current_token->numeric_value;
            parser_advance(parser);
            
            return operand_create_immediate(value, 32); // Default to 32-bit
        }
        
        case TOKEN_IDENTIFIER: {
            // Label reference
            char* label_name = strdup(parser->current_token->value);
            parser_advance(parser);
            
            operand_t* operand = operand_create_label(label_name);
            free(label_name);
            return operand;
        }
        
        case TOKEN_LBRACKET: {
            // Memory operand [base + index*scale + displacement]
            parser_advance(parser); // consume '['
            
            register_info_t* base = NULL;
            register_info_t* index = NULL;
            int scale = 1;
            int64_t displacement = 0;
            
            // Parse base register
            if (parser->current_token && parser->current_token->type == TOKEN_REGISTER) {
                operand_t* temp_reg = operand_create_register(parser->current_token->value, parser->architecture);
                if (temp_reg) {
                    base = temp_reg->data.reg.reg_info;
                    free(temp_reg->data.reg.name);
                    free(temp_reg);
                }
                parser_advance(parser);
            }
            
            // Parse optional + index*scale or displacement
            while (parser->current_token && parser->current_token->type != TOKEN_RBRACKET) {
                if (parser->current_token->type == TOKEN_PLUS) {
                    parser_advance(parser);
                    
                    if (parser->current_token->type == TOKEN_REGISTER) {
                        // Index register
                        operand_t* temp_reg = operand_create_register(parser->current_token->value, parser->architecture);
                        if (temp_reg) {
                            index = temp_reg->data.reg.reg_info;
                            free(temp_reg->data.reg.name);
                            free(temp_reg);
                        }
                        parser_advance(parser);
                        
                        // Check for scale
                        if (parser->current_token && parser->current_token->type == TOKEN_MULTIPLY) {
                            parser_advance(parser);
                            if (parser->current_token && parser->current_token->type == TOKEN_NUMBER) {
                                scale = (int)parser->current_token->numeric_value;
                                parser_advance(parser);
                            }
                        }
                    } else if (parser->current_token->type == TOKEN_NUMBER) {
                        // Displacement
                        displacement = (int64_t)parser->current_token->numeric_value;
                        parser_advance(parser);
                    }
                } else {
                    parser_advance(parser); // Skip unknown tokens
                }
            }
            
            if (!parser_expect_token(parser, TOKEN_RBRACKET)) {
                return NULL;
            }
            parser_advance(parser); // consume ']'
            
            return operand_create_memory(base, index, scale, displacement, 64);
        }
        
        default:
            parser_error(parser, "Invalid operand");
            return NULL;
    }
}

instruction_t* parse_instruction(parser_t* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_INSTRUCTION) {
        parser_error(parser, "Expected instruction mnemonic");
        return NULL;
    }
    
    instruction_t* instr = instruction_create(parser->current_token->value);
    if (!instr) {
        parser_error(parser, "Failed to create instruction");
        return NULL;
    }
    
    instr->line = parser->current_token->line;
    instr->column = parser->current_token->column;
    
    parser_advance(parser); // consume instruction mnemonic
    
    // Parse operands
    while (parser->current_token && 
           parser->current_token->type != TOKEN_NEWLINE && 
           parser->current_token->type != TOKEN_EOF &&
           parser->current_token->type != TOKEN_COMMENT) {
        
        operand_t* operand = parse_operand(parser);
        if (!operand) {
            instruction_destroy(instr);
            return NULL;
        }
        
        instruction_add_operand(instr, operand);
        free(operand); // instruction_add_operand copies the operand
        
        // Check for comma separator
        if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
            parser_advance(parser); // consume comma
        } else {
            break; // No more operands
        }
    }
    
    return instr;
}

bool parse_label(parser_t* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        return false;
    }
    
    // Look ahead for colon
    token_t* next_token = lexer_next_token(parser->lexer);
    bool is_label = (next_token && next_token->type == TOKEN_COLON);
    
    if (is_label) {
        // Define label in symbol table
        symbol_table_define(parser->symbol_table, parser->current_token->value, 
                          SYMBOL_LABEL, parser->current_address);
        
        parser_advance(parser); // consume label name
        parser_advance(parser); // consume colon
    }
    
    if (next_token) {
        token_destroy(next_token);
    }
    
    return is_label;
}

bool parse_section_directive(parser_t* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_DIRECTIVE) {
        return false;
    }
    
    if (strcasecmp(parser->current_token->value, "text") == 0) {
        parser->current_section = SECTION_TEXT;
        parser_advance(parser);
        return true;
    } else if (strcasecmp(parser->current_token->value, "data") == 0) {
        parser->current_section = SECTION_DATA;
        parser_advance(parser);
        return true;
    } else if (strcasecmp(parser->current_token->value, "bss") == 0) {
        parser->current_section = SECTION_BSS;
        parser_advance(parser);
        return true;
    }
    
    return false;
}

data_definition_t* parse_data_definition(parser_t* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_DIRECTIVE) {
        return NULL;
    }
    
    data_definition_t* data_def = malloc(sizeof(data_definition_t));
    if (!data_def) return NULL;
    
    data_def->repeat_count = 1;
    
    // Determine data type from directive
    if (strcasecmp(parser->current_token->value, "db") == 0) {
        data_def->type = DATA_BYTE;
    } else if (strcasecmp(parser->current_token->value, "dw") == 0) {
        data_def->type = DATA_WORD;
    } else if (strcasecmp(parser->current_token->value, "dd") == 0) {
        data_def->type = DATA_DWORD;
    } else if (strcasecmp(parser->current_token->value, "dq") == 0) {
        data_def->type = DATA_QWORD;
    } else if (strcasecmp(parser->current_token->value, "resb") == 0) {
        data_def->type = DATA_BYTE;
        data_def->data.byte_value = 0;
    } else if (strcasecmp(parser->current_token->value, "resw") == 0) {
        data_def->type = DATA_WORD;
        data_def->data.word_value = 0;
    } else if (strcasecmp(parser->current_token->value, "resd") == 0) {
        data_def->type = DATA_DWORD;
        data_def->data.dword_value = 0;
    } else if (strcasecmp(parser->current_token->value, "resq") == 0) {
        data_def->type = DATA_QWORD;
        data_def->data.qword_value = 0;
    } else {
        free(data_def);
        return NULL;
    }
    
    bool is_reserve = (strncasecmp(parser->current_token->value, "res", 3) == 0);
    parser_advance(parser); // consume directive
    
    // Parse the data value or count
    if (parser->current_token && parser->current_token->type == TOKEN_NUMBER) {
        uint64_t value = parser->current_token->numeric_value;
        
        if (is_reserve) {
            data_def->repeat_count = value;
        } else {
            switch (data_def->type) {
                case DATA_BYTE:
                    data_def->data.byte_value = (uint8_t)value;
                    break;
                case DATA_WORD:
                    data_def->data.word_value = (uint16_t)value;
                    break;
                case DATA_DWORD:
                    data_def->data.dword_value = (uint32_t)value;
                    break;
                case DATA_QWORD:
                    data_def->data.qword_value = value;
                    break;
            }
        }
        parser_advance(parser);
    } else {
        free(data_def);
        return NULL;
    }
    
    return data_def;
}

void data_definition_destroy(data_definition_t* data_def) {
    if (data_def) {
        if (data_def->type == DATA_BYTE && data_def->data.string_value) {
            // Handle string data later
            free(data_def->data.string_value);
        }
        free(data_def);
    }
}

bool parse_directive(parser_t* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_DIRECTIVE) {
        return false;
    }
    
    // Try to parse section directive
    if (parse_section_directive(parser)) {
        return true;
    }
    
    // Try to parse data definition
    data_definition_t* data_def = parse_data_definition(parser);
    if (data_def) {
        // TODO: Add data definition to program
        data_definition_destroy(data_def);
        return true;
    }
    
    return false;
}

program_t* parser_parse(parser_t* parser) {
    program_t* program = malloc(sizeof(program_t));
    if (!program) return NULL;
    
    program->instructions = malloc(INITIAL_CAPACITY * sizeof(instruction_t*));
    if (!program->instructions) {
        free(program);
        return NULL;
    }
    
    program->instruction_count = 0;
    program->instruction_capacity = INITIAL_CAPACITY;
    program->data_definitions = malloc(INITIAL_CAPACITY * sizeof(data_definition_t*));
    program->data_count = 0;
    program->data_capacity = INITIAL_CAPACITY;
    program->symbols = parser->symbol_table;
    program->code = malloc(MAX_CODE_SIZE);
    program->code_size = 0;
    program->data_section = malloc(MAX_CODE_SIZE);
    program->data_size = 0;
    program->current_section = SECTION_TEXT;
    
    if (!program->code || !program->data_section || !program->data_definitions) {
        free(program->instructions);
        free(program->data_definitions);
        free(program->code);
        free(program->data_section);
        free(program);
        return NULL;
    }
    
    // Parse the input
    while (parser->current_token && parser->current_token->type != TOKEN_EOF && !parser->has_error) {
        skip_newlines(parser);
        
        if (!parser->current_token || parser->current_token->type == TOKEN_EOF) {
            break;
        }
        
        // Try to parse label
        if (parse_label(parser)) {
            continue;
        }
        
        // Try to parse directive
        if (parse_directive(parser)) {
            continue;
        }
        
        // Parse instruction
        if (parser->current_token->type == TOKEN_INSTRUCTION) {
            instruction_t* instr = parse_instruction(parser);
            if (!instr) {
                break; // Error occurred
            }
            
            // Add instruction to program
            if (program->instruction_count >= program->instruction_capacity) {
                program->instruction_capacity *= 2;
                program->instructions = realloc(program->instructions, 
                    program->instruction_capacity * sizeof(instruction_t*));
                if (!program->instructions) {
                    instruction_destroy(instr);
                    break;
                }
            }
            
            program->instructions[program->instruction_count++] = instr;
            
            // Encode instruction
            uint8_t instruction_bytes[16];
            int bytes_generated = encode_instruction(instr, parser->architecture, 
                                                   instruction_bytes, sizeof(instruction_bytes));
            
            if (bytes_generated > 0 && program->code_size + bytes_generated <= MAX_CODE_SIZE) {
                memcpy(program->code + program->code_size, instruction_bytes, bytes_generated);
                program->code_size += bytes_generated;
                parser->current_address += bytes_generated;
            }
        } else {
            parser_error(parser, "Unexpected token");
            break;
        }
        
        skip_newlines(parser);
    }
    
    if (parser->has_error) {
        program_destroy(program);
        return NULL;
    }
    
    return program;
}

void program_destroy(program_t* program) {
    if (!program) return;
    
    if (program->instructions) {
        for (int i = 0; i < program->instruction_count; i++) {
            instruction_destroy(program->instructions[i]);
        }
        free(program->instructions);
    }
    
    if (program->data_definitions) {
        for (int i = 0; i < program->data_count; i++) {
            data_definition_destroy(program->data_definitions[i]);
        }
        free(program->data_definitions);
    }
    
    free(program->code);
    free(program->data_section);
    // Note: Don't destroy symbol_table here as it's owned by parser
    free(program);
} 