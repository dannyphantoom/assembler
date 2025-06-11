#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "../include/symbol_table.h"

#define DEFAULT_BUCKET_COUNT 256

unsigned int hash_string(const char* str) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash;
}

symbol_table_t* symbol_table_create(int bucket_count) {
    if (bucket_count <= 0) {
        bucket_count = DEFAULT_BUCKET_COUNT;
    }
    
    symbol_table_t* table = malloc(sizeof(symbol_table_t));
    if (!table) return NULL;
    
    table->buckets = calloc(bucket_count, sizeof(symbol_t*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }
    
    table->bucket_count = bucket_count;
    table->symbol_count = 0;
    
    return table;
}

void symbol_table_destroy(symbol_table_t* table) {
    if (!table) return;
    
    for (int i = 0; i < table->bucket_count; i++) {
        symbol_t* symbol = table->buckets[i];
        while (symbol) {
            symbol_t* next = symbol->next;
            free(symbol->name);
            free(symbol);
            symbol = next;
        }
    }
    
    free(table->buckets);
    free(table);
}

symbol_t* symbol_table_lookup(symbol_table_t* table, const char* name) {
    if (!table || !name) return NULL;
    
    unsigned int hash = hash_string(name);
    int bucket = hash % table->bucket_count;
    
    symbol_t* symbol = table->buckets[bucket];
    while (symbol) {
        if (strcmp(symbol->name, name) == 0) {
            return symbol;
        }
        symbol = symbol->next;
    }
    
    return NULL;
}

symbol_t* symbol_table_define(symbol_table_t* table, const char* name, 
                             symbol_type_t type, uint64_t address) {
    if (!table || !name) return NULL;
    
    // Check if symbol already exists
    symbol_t* existing = symbol_table_lookup(table, name);
    if (existing) {
        // Update existing symbol
        existing->type = type;
        existing->address = address;
        existing->defined = true;
        return existing;
    }
    
    // Create new symbol
    symbol_t* symbol = malloc(sizeof(symbol_t));
    if (!symbol) return NULL;
    
    symbol->name = strdup(name);
    if (!symbol->name) {
        free(symbol);
        return NULL;
    }
    
    symbol->type = type;
    symbol->address = address;
    symbol->defined = true;
    symbol->section = 0; // Default section
    
    // Insert into hash table
    unsigned int hash = hash_string(name);
    int bucket = hash % table->bucket_count;
    
    symbol->next = table->buckets[bucket];
    table->buckets[bucket] = symbol;
    table->symbol_count++;
    
    return symbol;
}

bool symbol_table_is_defined(symbol_table_t* table, const char* name) {
    symbol_t* symbol = symbol_table_lookup(table, name);
    return symbol && symbol->defined;
}

void symbol_table_print(symbol_table_t* table) {
    if (!table) return;
    
    printf("Symbol Table (%d symbols):\n", table->symbol_count);
    printf("%-20s %-10s %-16s %-8s\n", "Name", "Type", "Address", "Defined");
    printf("%-20s %-10s %-16s %-8s\n", "----", "----", "-------", "-------");
    
    for (int i = 0; i < table->bucket_count; i++) {
        symbol_t* symbol = table->buckets[i];
        while (symbol) {
            const char* type_str;
            switch (symbol->type) {
                case SYMBOL_LABEL: type_str = "LABEL"; break;
                case SYMBOL_CONSTANT: type_str = "CONST"; break;
                case SYMBOL_VARIABLE: type_str = "VAR"; break;
                default: type_str = "UNKNOWN"; break;
            }
            
            printf("%-20s %-10s 0x%014lx %-8s\n", 
                   symbol->name, type_str, symbol->address,
                   symbol->defined ? "YES" : "NO");
            
            symbol = symbol->next;
        }
    }
} 