#ifndef MNC_COMPILER_H
#define MNC_COMPILER_H

#include <stdint.h>
#include <stdbool.h>
#include "mnc_parser.h"

// Symbol types
typedef enum {
    SYMBOL_FUNCTION,
    SYMBOL_VARIABLE,
    SYMBOL_CONSTANT
} symbol_type_t;

// Symbol table (simplified)
typedef struct symbol_entry symbol_table_t;

// Compiler structure
typedef struct {
    uint8_t *bytecode;
    size_t bytecode_size;
    size_t bytecode_capacity;
    symbol_table_t *symbol_table;
} mnc_compiler_t;

// Compiler API
mnc_compiler_t* mnc_compiler_init(void);
void mnc_compiler_cleanup(mnc_compiler_t *compiler);
int mnc_compile(mnc_compiler_t *compiler, ast_node_t *ast_root);
uint8_t* mnc_get_bytecode(mnc_compiler_t *compiler, size_t *size);
void mnc_print_symbol_table(mnc_compiler_t *compiler);

#endif // MNC_COMPILER_H
