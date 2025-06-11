#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "../include/assembler.h"
#include "../include/lexer.h"
#include "../include/parser.h"

void print_usage(const char* program_name) {
    printf("Usage: %s [options] <input_file>\n", program_name);
    printf("Options:\n");
    printf("  -a, --arch <arch>     Target architecture (x86_16, x86_32, x86_64, arm_32, arm_64)\n");
    printf("  -f, --format <format> Output format (elf, pe, bin)\n");
    printf("  -o, --output <file>   Output file\n");
    printf("  -d, --debug           Enable debug mode\n");
    printf("  -h, --help            Show this help message\n");
    printf("\nSupported architectures:\n");
    printf("  x86_16   - x86 16-bit mode\n");
    printf("  x86_32   - x86 32-bit mode\n");
    printf("  x86_64   - x86 64-bit mode\n");
    printf("  arm_32   - ARM 32-bit mode\n");
    printf("  arm_64   - ARM 64-bit mode (AArch64)\n");
}

arch_type_t parse_architecture(const char* arch_str) {
    if (strcmp(arch_str, "x86_16") == 0) return ARCH_X86_16;
    if (strcmp(arch_str, "x86_32") == 0) return ARCH_X86_32;
    if (strcmp(arch_str, "x86_64") == 0) return ARCH_X86_64;
    if (strcmp(arch_str, "arm_32") == 0) return ARCH_ARM_32;
    if (strcmp(arch_str, "arm_64") == 0) return ARCH_ARM_64;
    return -1; // Invalid architecture
}

output_format_t parse_format(const char* format_str) {
    if (strcmp(format_str, "elf") == 0) return FORMAT_ELF;
    if (strcmp(format_str, "pe") == 0) return FORMAT_PE;
    if (strcmp(format_str, "bin") == 0) return FORMAT_BIN;
    return -1; // Invalid format
}

int parse_arguments(int argc, char* argv[], assembler_context_t* ctx) {
    // Initialize defaults
    ctx->architecture = ARCH_X86_64;
    ctx->output_format = FORMAT_ELF;
    ctx->input_file = NULL;
    ctx->output_file = NULL;
    ctx->debug_mode = false;

    static struct option long_options[] = {
        {"arch", required_argument, 0, 'a'},
        {"format", required_argument, 0, 'f'},
        {"output", required_argument, 0, 'o'},
        {"debug", no_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;

    while ((c = getopt_long(argc, argv, "a:f:o:dh", long_options, &option_index)) != -1) {
        switch (c) {
            case 'a': {
                arch_type_t arch = parse_architecture(optarg);
                if (arch == -1) {
                    fprintf(stderr, "Error: Invalid architecture '%s'\n", optarg);
                    return -1;
                }
                ctx->architecture = arch;
                break;
            }
            case 'f': {
                output_format_t format = parse_format(optarg);
                if (format == -1) {
                    fprintf(stderr, "Error: Invalid format '%s'\n", optarg);
                    return -1;
                }
                ctx->output_format = format;
                break;
            }
            case 'o':
                ctx->output_file = optarg;
                break;
            case 'd':
                ctx->debug_mode = true;
                break;
            case 'h':
                print_usage(argv[0]);
                return 1;
            case '?':
                return -1;
            default:
                abort();
        }
    }

    // Get input file
    if (optind >= argc) {
        fprintf(stderr, "Error: No input file specified\n");
        print_usage(argv[0]);
        return -1;
    }
    ctx->input_file = argv[optind];

    // Set default output file if not specified
    if (!ctx->output_file) {
        const char* input_base = strrchr(ctx->input_file, '/');
        input_base = input_base ? input_base + 1 : ctx->input_file;
        
        // Remove extension and add new one
        char* output = malloc(strlen(input_base) + 10);
        strcpy(output, input_base);
        char* dot = strrchr(output, '.');
        if (dot) *dot = '\0';
        
        switch (ctx->output_format) {
            case FORMAT_ELF:
                strcat(output, ".o");
                break;
            case FORMAT_PE:
                strcat(output, ".obj");
                break;
            case FORMAT_BIN:
                strcat(output, ".bin");
                break;
        }
        ctx->output_file = output;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    assembler_context_t ctx;
    
    // Parse command line arguments
    int parse_result = parse_arguments(argc, argv, &ctx);
    if (parse_result != 0) {
        return parse_result == 1 ? 0 : 1; // 1 means help was shown
    }

    if (ctx.debug_mode) {
        printf("Assembler Configuration:\n");
        printf("  Input file: %s\n", ctx.input_file);
        printf("  Output file: %s\n", ctx.output_file);
        printf("  Architecture: %d\n", ctx.architecture);
        printf("  Format: %d\n", ctx.output_format);
    }

    // Assemble the file
    int result = assemble_file(&ctx);
    
    if (result == 0) {
        printf("Assembly completed successfully: %s -> %s\n", 
               ctx.input_file, ctx.output_file);
    } else {
        printf("Assembly failed with error code: %d\n", result);
    }

    return result;
} 