#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "mnc_parser.h"
#include "mnc_compiler.h"

// Compiler state
typedef struct {
    uint8_t *bytecode;
    size_t bytecode_size;
    size_t bytecode_capacity;
    size_t current_address;
    symbol_table_t *symbol_table;
} compiler_state_t;

// Symbol table entry
typedef struct symbol_entry {
    char *name;
    uint32_t address;
    symbol_type_t type;
    struct symbol_entry *next;
} symbol_entry_t;

// Initialize compiler
mnc_compiler_t* mnc_compiler_init(void) {
    mnc_compiler_t *compiler = malloc(sizeof(mnc_compiler_t));
    if (!compiler) return NULL;
    
    compiler->bytecode = malloc(1024); // Initial 1KB
    if (!compiler->bytecode) {
        free(compiler);
        return NULL;
    }
    
    compiler->bytecode_size = 0;
    compiler->bytecode_capacity = 1024;
    compiler->symbol_table = NULL;
    
    return compiler;
}

// Cleanup compiler
void mnc_compiler_cleanup(mnc_compiler_t *compiler) {
    if (!compiler) return;
    
    if (compiler->bytecode) free(compiler->bytecode);
    
    // Free symbol table
    symbol_entry_t *entry = compiler->symbol_table;
    while (entry) {
        symbol_entry_t *next = entry->next;
        if (entry->name) free(entry->name);
        free(entry);
        entry = next;
    }
    
    free(compiler);
}

// Add symbol to symbol table
static int add_symbol(compiler_state_t *state, const char *name, uint32_t address, symbol_type_t type) {
    symbol_entry_t *entry = malloc(sizeof(symbol_entry_t));
    if (!entry) return -1;
    
    entry->name = strdup(name);
    entry->address = address;
    entry->type = type;
    entry->next = state->symbol_table;
    state->symbol_table = entry;
    
    return 0;
}

// Find symbol in symbol table
static symbol_entry_t* find_symbol(compiler_state_t *state, const char *name) {
    symbol_entry_t *entry = state->symbol_table;
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

// Emit bytecode
static int emit_byte(compiler_state_t *state, uint8_t byte) {
    if (state->bytecode_size >= state->bytecode_capacity) {
        // Resize bytecode buffer
        state->bytecode_capacity *= 2;
        uint8_t *new_bytecode = realloc(state->bytecode, state->bytecode_capacity);
        if (!new_bytecode) return -1;
        state->bytecode = new_bytecode;
    }
    
    state->bytecode[state->bytecode_size++] = byte;
    state->current_address++;
    return 0;
}

// Emit 32-bit value (little-endian)
static int emit_dword(compiler_state_t *state, uint32_t value) {
    if (emit_byte(state, value & 0xFF) != 0) return -1;
    if (emit_byte(state, (value >> 8) & 0xFF) != 0) return -1;
    if (emit_byte(state, (value >> 16) & 0xFF) != 0) return -1;
    if (emit_byte(state, (value >> 24) & 0xFF) != 0) return -1;
    return 0;
}

// Emit string
static int emit_string(compiler_state_t *state, const char *string) {
    size_t len = strlen(string);
    for (size_t i = 0; i < len; i++) {
        if (emit_byte(state, (uint8_t)string[i]) != 0) return -1;
    }
    return 0;
}

// Compile AST node
static int compile_node(compiler_state_t *state, ast_node_t *node) {
    if (!node) return -1;
    
    switch (node->type) {
        case NODE_SYS_DECL:
            // System declaration - emit system call prefix
            if (emit_byte(state, 0x48) != 0) return -1; // SYS prefix
            printf("Compiled system declaration\n");
            break;
            
        case NODE_IMPORT:
            // Import statement - emit library import
            if (emit_byte(state, 0x49) != 0) return -1; // IMPORT prefix
            printf("Compiled import: %s\n", node->value ? node->value : "(null)");
            break;
            
        case NODE_FUNCTION:
            // Function definition
            if (emit_byte(state, 0x4A) != 0) return -1; // FUNC prefix
            if (node->value) {
                add_symbol(state, node->value, state->current_address, SYMBOL_FUNCTION);
                printf("Compiled function: %s at address 0x%08X\n", 
                       node->value, state->current_address);
            }
            break;
            
        case NODE_VARIABLE:
            // Variable declaration
            if (emit_byte(state, 0x4B) != 0) return -1; // VAR prefix
            if (node->value) {
                add_symbol(state, node->value, state->current_address, SYMBOL_VARIABLE);
                printf("Compiled variable: %s at address 0x%08X\n", 
                       node->value, state->current_address);
            }
            break;
            
        case NODE_STRING:
            // String literal
            if (emit_byte(state, 0x4C) != 0) return -1; // STRING prefix
            if (node->value) {
                emit_string(state, node->value);
                printf("Compiled string: \"%s\"\n", node->value);
            }
            break;
            
        case NODE_NUMBER:
            // Number literal
            if (emit_byte(state, 0x4D) != 0) return -1; // NUMBER prefix
            if (node->value) {
                uint32_t value = strtoul(node->value, NULL, 0);
                emit_dword(state, value);
                printf("Compiled number: %s (0x%08X)\n", node->value, value);
            }
            break;
            
        case NODE_IDENTIFIER:
            // Identifier reference
            if (emit_byte(state, 0x4E) != 0) return -1; // ID prefix
            if (node->value) {
                symbol_entry_t *symbol = find_symbol(state, node->value);
                if (symbol) {
                    emit_dword(state, symbol->address);
                    printf("Compiled identifier: %s (address 0x%08X)\n", 
                           node->value, symbol->address);
                } else {
                    printf("Warning: Undefined identifier: %s\n", node->value);
                    emit_dword(state, 0); // Placeholder
                }
            }
            break;
            
        case NODE_CALL:
            // Function call
            if (emit_byte(state, 0x4F) != 0) return -1; // CALL prefix
            printf("Compiled function call\n");
            break;
            
        case NODE_RETURN:
            // Return statement
            if (emit_byte(state, 0x50) != 0) return -1; // RETURN prefix
            printf("Compiled return statement\n");
            break;
            
        default:
            printf("Unknown node type: %d\n", node->type);
            break;
    }
    
    // Compile children
    for (int i = 0; i < node->child_count; i++) {
        if (compile_node(state, node->children[i]) != 0) return -1;
    }
    
    return 0;
}

// Compile M&C AST to bytecode
int mnc_compile(mnc_compiler_t *compiler, ast_node_t *ast_root) {
    if (!compiler || !ast_root) return -1;
    
    compiler_state_t state = {0};
    state.bytecode = compiler->bytecode;
    state.bytecode_size = compiler->bytecode_size;
    state.bytecode_capacity = compiler->bytecode_capacity;
    state.current_address = 0;
    state.symbol_table = compiler->symbol_table;
    
    printf("Starting M&C compilation...\n");
    
    // Compile the AST
    int result = compile_node(&state, ast_root);
    
    if (result == 0) {
        printf("Compilation completed successfully\n");
        printf("Generated %zu bytes of bytecode\n", state.bytecode_size);
    } else {
        printf("Compilation failed\n");
    }
    
    // Update compiler state
    compiler->bytecode_size = state.bytecode_size;
    compiler->symbol_table = state.symbol_table;
    
    return result;
}

// Get compiled bytecode
uint8_t* mnc_get_bytecode(mnc_compiler_t *compiler, size_t *size) {
    if (!compiler) return NULL;
    
    if (size) *size = compiler->bytecode_size;
    return compiler->bytecode;
}

// Print symbol table
void mnc_print_symbol_table(mnc_compiler_t *compiler) {
    if (!compiler) return;
    
    printf("\nSymbol Table:\n");
    printf("============\n");
    
    symbol_entry_t *entry = compiler->symbol_table;
    while (entry) {
        const char *type_str;
        switch (entry->type) {
            case SYMBOL_FUNCTION: type_str = "function"; break;
            case SYMBOL_VARIABLE: type_str = "variable"; break;
            default: type_str = "unknown"; break;
        }
        
        printf("%s: %s @ 0x%08X\n", type_str, entry->name, entry->address);
        entry = entry->next;
    }
    printf("\n");
}
