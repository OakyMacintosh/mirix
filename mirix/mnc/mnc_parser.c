#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "mnc_parser.h"

// Token types for M&C language
typedef enum {
    TOKEN_SYS,
    TOKEN_TYPOSIS,
    TOKEN_START,
    TOKEN_DEF,
    TOKEN_RETURN,
    TOKEN_ASCII,
    TOKEN_PRINTER,
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_ADDRESS,
    TOKEN_MULTIPLY,
    TOKEN_AMPERSAND,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_DOLLAR,
    TOKEN_PERCENT,
    TOKEN_EOF,
    TOKEN_ERROR
} token_type_t;

// Token structure
typedef struct {
    token_type_t type;
    char *value;
    size_t line;
    size_t column;
} token_t;

// Parser state
typedef struct {
    const char *input;
    size_t position;
    size_t line;
    size_t column;
    token_t current_token;
} parser_state_t;

// Initialize parser
mnc_parser_t* mnc_parser_init(const char *input) {
    mnc_parser_t *parser = malloc(sizeof(mnc_parser_t));
    if (!parser) return NULL;
    
    parser->input = strdup(input);
    parser->position = 0;
    parser->line = 1;
    parser->column = 1;
    parser->ast_root = NULL;
    
    return parser;
}

// Cleanup parser
void mnc_parser_cleanup(mnc_parser_t *parser) {
    if (parser) {
        if (parser->input) free(parser->input);
        if (parser->ast_root) ast_node_free(parser->ast_root);
        free(parser);
    }
}

// Skip whitespace and comments
static void skip_whitespace(parser_state_t *state) {
    while (state->input[state->position] && 
           isspace(state->input[state->position])) {
        if (state->input[state->position] == '\n') {
            state->line++;
            state->column = 1;
        } else {
            state->column++;
        }
        state->position++;
    }
    
    // Skip comments (// to end of line)
    if (state->input[state->position] == '/' && 
        state->input[state->position + 1] == '/') {
        while (state->input[state->position] && 
               state->input[state->position] != '\n') {
            state->position++;
        }
        // Recursively skip more whitespace/comments
        skip_whitespace(state);
    }
}

// Parse number
static char* parse_number(parser_state_t *state) {
    size_t start = state->position;
    
    while (state->input[state->position] && 
           (isdigit(state->input[state->position]) || 
            state->input[state->position] == 'x' ||
            state->input[state->position] == 'X')) {
        state->position++;
        state->column++;
    }
    
    size_t length = state->position - start;
    char *number = malloc(length + 1);
    memcpy(number, &state->input[start], length);
    number[length] = '\0';
    
    return number;
}

// Parse string literal
static char* parse_string(parser_state_t *state) {
    // Skip opening quote
    state->position++;
    state->column++;
    
    size_t start = state->position;
    
    while (state->input[state->position] && 
           state->input[state->position] != '"') {
        if (state->input[state->position] == '\n') {
            state->line++;
            state->column = 1;
        } else {
            state->column++;
        }
        state->position++;
    }
    
    size_t length = state->position - start;
    char *string = malloc(length + 1);
    memcpy(string, &state->input[start], length);
    string[length] = '\0';
    
    // Skip closing quote
    if (state->input[state->position] == '"') {
        state->position++;
        state->column++;
    }
    
    return string;
}

// Parse identifier or keyword
static char* parse_identifier(parser_state_t *state) {
    size_t start = state->position;
    
    while (state->input[state->position] && 
           (isalnum(state->input[state->position]) || 
            state->input[state->position] == '_' ||
            state->input[state->position] == '*' ||
            state->input[state->position] == '$' ||
            state->input[state->position] == '@' ||
            state->input[state->position] == '&')) {
        state->position++;
        state->column++;
    }
    
    size_t length = state->position - start;
    char *identifier = malloc(length + 1);
    memcpy(identifier, &state->input[start], length);
    identifier[length] = '\0';
    
    return identifier;
}

// Get next token
static token_t next_token(parser_state_t *state) {
    token_t token = {0};
    token.line = state->line;
    token.column = state->column;
    
    skip_whitespace(state);
    
    if (!state->input[state->position]) {
        token.type = TOKEN_EOF;
        return token;
    }
    
    char current = state->input[state->position];
    
    // Single character tokens
    switch (current) {
        case '%':
            token.type = TOKEN_PERCENT;
            state->position++;
            state->column++;
            return token;
        case '$':
            token.type = TOKEN_DOLLAR;
            state->position++;
            state->column++;
            return token;
        case '&':
            token.type = TOKEN_AMPERSAND;
            state->position++;
            state->column++;
            return token;
        case '*':
            token.type = TOKEN_MULTIPLY;
            state->position++;
            state->column++;
            return token;
        case ':':
            token.type = TOKEN_COLON;
            state->position++;
            state->column++;
            return token;
        case ';':
            token.type = TOKEN_SEMICOLON;
            state->position++;
            state->column++;
            return token;
        case '"':
            token.type = TOKEN_STRING;
            token.value = parse_string(state);
            return token;
    }
    
    // Numbers (hexadecimal support)
    if (isdigit(current)) {
        token.type = TOKEN_NUMBER;
        token.value = parse_number(state);
        return token;
    }
    
    // Identifiers and keywords
    if (isalpha(current) || current == '_') {
        char *identifier = parse_identifier(state);
        
        // Check for keywords
        if (strcmp(identifier, "sys") == 0) {
            token.type = TOKEN_SYS;
        } else if (strcmp(identifier, "typosis") == 0) {
            token.type = TOKEN_TYPOSIS;
        } else if (strcmp(identifier, "start") == 0) {
            token.type = TOKEN_START;
        } else if (strcmp(identifier, "def") == 0) {
            token.type = TOKEN_DEF;
        } else if (strcmp(identifier, "return") == 0) {
            token.type = TOKEN_RETURN;
        } else if (strcmp(identifier, "ascii") == 0) {
            token.type = TOKEN_ASCII;
        } else if (strcmp(identifier, "prtlist") == 0) {
            token.type = TOKEN_PRINTER;
        } else {
            token.type = TOKEN_IDENTIFIER;
            token.value = identifier;
            return token;
        }
        
        free(identifier);
        return token;
    }
    
    token.type = TOKEN_ERROR;
    return token;
}

// Create AST node
static ast_node_t* ast_node_create(node_type_t type) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    if (!node) return NULL;
    
    memset(node, 0, sizeof(ast_node_t));
    node->type = type;
    return node;
}

// Free AST node
void ast_node_free(ast_node_t *node) {
    if (!node) return;
    
    // Free children
    for (int i = 0; i < node->child_count; i++) {
        ast_node_free(node->children[i]);
    }
    
    // Free value if allocated
    if (node->value) free(node->value);
    
    free(node);
}

// Add child to AST node
static void ast_node_add_child(ast_node_t *parent, ast_node_t *child) {
    if (!parent || !child) return;
    
    // Resize children array if needed
    if (parent->child_count >= parent->child_capacity) {
        parent->child_capacity = parent->child_capacity == 0 ? 4 : parent->child_capacity * 2;
        parent->children = realloc(parent->children, 
                                 parent->child_capacity * sizeof(ast_node_t*));
    }
    
    parent->children[parent->child_count++] = child;
}

// Parse M&C code
int mnc_parse(mnc_parser_t *parser) {
    if (!parser || !parser->input) return -1;
    
    parser_state_t state = {0};
    state.input = parser->input;
    state.position = 0;
    state.line = 1;
    state.column = 1;
    
    // Create root AST node
    parser->ast_root = ast_node_create(NODE_PROGRAM);
    if (!parser->ast_root) return -1;
    
    // Parse tokens and build AST
    while (true) {
        token_t token = next_token(&state);
        
        if (token.type == TOKEN_EOF) {
            break;
        }
        
        if (token.type == TOKEN_ERROR) {
            fprintf(stderr, "Parse error at line %zu, column %zu\n", 
                   token.line, token.column);
            return -1;
        }
        
        // Handle different token types
        ast_node_t *node = NULL;
        
        switch (token.type) {
            case TOKEN_SYS:
                node = ast_node_create(NODE_SYS_DECL);
                break;
            case TOKEN_TYPOSIS:
                node = ast_node_create(NODE_IMPORT);
                break;
            case TOKEN_START:
                node = ast_node_create(NODE_FUNCTION);
                break;
            case TOKEN_DEF:
                node = ast_node_create(NODE_VARIABLE);
                break;
            case TOKEN_STRING:
                node = ast_node_create(NODE_STRING);
                node->value = token.value;
                break;
            case TOKEN_NUMBER:
                node = ast_node_create(NODE_NUMBER);
                node->value = token.value;
                break;
            case TOKEN_IDENTIFIER:
                node = ast_node_create(NODE_IDENTIFIER);
                node->value = token.value;
                break;
            default:
                // Skip other tokens for now
                continue;
        }
        
        if (node) {
            ast_node_add_child(parser->ast_root, node);
        }
    }
    
    return 0;
}

// Get AST root
ast_node_t* mnc_get_ast(mnc_parser_t *parser) {
    return parser ? parser->ast_root : NULL;
}
