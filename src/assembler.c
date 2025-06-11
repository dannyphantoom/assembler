#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/assembler.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/instruction.h"

int write_output_file(const char* filename, uint8_t* code, size_t code_size, 
                     output_format_t format, arch_type_t arch) {
    FILE* output_file = fopen(filename, "wb");
    if (!output_file) {
        fprintf(stderr, "Error: Cannot open output file '%s'\n", filename);
        return -1;
    }

    int result = 0;
    
    switch (format) {
        case FORMAT_BIN:
            // Raw binary output
            if (fwrite(code, 1, code_size, output_file) != code_size) {
                fprintf(stderr, "Error: Failed to write binary output\n");
                result = -1;
            }
            break;
            
        case FORMAT_ELF:
            // TODO: Implement ELF format output
            fprintf(stderr, "Warning: ELF format not yet implemented, writing raw binary\n");
            if (fwrite(code, 1, code_size, output_file) != code_size) {
                fprintf(stderr, "Error: Failed to write output\n");
                result = -1;
            }
            break;
            
        case FORMAT_PE:
            // TODO: Implement PE format output  
            fprintf(stderr, "Warning: PE format not yet implemented, writing raw binary\n");
            if (fwrite(code, 1, code_size, output_file) != code_size) {
                fprintf(stderr, "Error: Failed to write output\n");
                result = -1;
            }
            break;
            
        default:
            fprintf(stderr, "Error: Unknown output format\n");
            result = -1;
            break;
    }

    fclose(output_file);
    return result;
}

int assemble_file(assembler_context_t* ctx) {
    // Open input file
    FILE* input_file = fopen(ctx->input_file, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", ctx->input_file);
        return -1;
    }

    if (ctx->debug_mode) {
        printf("Starting assembly of '%s'\n", ctx->input_file);
    }

    // Create lexer
    lexer_t* lexer = lexer_create(input_file);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to create lexer\n");
        fclose(input_file);
        return -1;
    }

    // Create parser
    parser_t* parser = parser_create(lexer, ctx->architecture);
    if (!parser) {
        fprintf(stderr, "Error: Failed to create parser\n");
        lexer_destroy(lexer);
        fclose(input_file);
        return -1;
    }

    if (ctx->debug_mode) {
        printf("Parsing assembly code...\n");
    }

    // Parse the input
    program_t* program = parser_parse(parser);
    if (!program) {
        fprintf(stderr, "Error: Parsing failed\n");
        if (parser->has_error) {
            fprintf(stderr, "Parser error: %s\n", parser->error_message);
        }
        parser_destroy(parser);
        lexer_destroy(lexer);
        fclose(input_file);
        return -1;
    }

    if (ctx->debug_mode) {
        printf("Parsed %d instructions\n", program->instruction_count);
        printf("Code size: %zu bytes\n", program->code_size);
    }

    // Write output file
    if (ctx->debug_mode) {
        printf("Writing output to '%s'\n", ctx->output_file);
    }

    int write_result = write_output_file(ctx->output_file, program->code, 
                                       program->code_size, ctx->output_format, 
                                       ctx->architecture);

    // Cleanup
    program_destroy(program);
    parser_destroy(parser);
    lexer_destroy(lexer);
    fclose(input_file);

    if (write_result != 0) {
        fprintf(stderr, "Error: Failed to write output file\n");
        return -1;
    }

    return 0;
} 