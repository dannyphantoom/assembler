#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdint.h>
#include <stdbool.h>

// Symbol types
typedef enum {
    SYMBOL_LABEL,
    SYMBOL_CONSTANT,
    SYMBOL_VARIABLE
} symbol_type_t;

// Symbol structure
typedef struct symbol {
    char* name;
    symbol_type_t type;
    uint64_t address;
    bool defined;
    int section;
    struct symbol* next;
} symbol_t;

// Symbol table structure
typedef struct {
    symbol_t** buckets;
    int bucket_count;
    int symbol_count;
} symbol_table_t;

// Function declarations
symbol_table_t* symbol_table_create(int bucket_count);
void symbol_table_destroy(symbol_table_t* table);
symbol_t* symbol_table_lookup(symbol_table_t* table, const char* name);
symbol_t* symbol_table_define(symbol_table_t* table, const char* name, 
                             symbol_type_t type, uint64_t address);
bool symbol_table_is_defined(symbol_table_t* table, const char* name);
void symbol_table_print(symbol_table_t* table);

// Hash function
unsigned int hash_string(const char* str);

#endif // SYMBOL_TABLE_H 