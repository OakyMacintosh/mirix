#ifndef MNC_PARSER_H
#define MNC_PARSER_H

#include <stdint.h>
#include <stdbool.h>

// AST node types
typedef enum {
    NODE_PROGRAM,
    NODE_SYS_DECL,
    NODE_IMPORT,
    NODE_FUNCTION,
    NODE_VARIABLE,
    NODE_STRING,
    NODE_NUMBER,
    NODE_IDENTIFIER,
    NODE_BINARY_OP,
    NODE_CALL,
    NODE_RETURN
} node_type_t;

// AST node structure
typedef struct ast_node {
    node_type_t type;
    char *value;
    struct ast_node **children;
    int child_count;
    int child_capacity;
} ast_node_t;

// Parser structure
typedef struct {
    char *input;
    size_t position;
    size_t line;
    size_t column;
    ast_node_t *ast_root;
} mnc_parser_t;

// Parser API
mnc_parser_t* mnc_parser_init(const char *input);
void mnc_parser_cleanup(mnc_parser_t *parser);
int mnc_parse(mnc_parser_t *parser);
ast_node_t* mnc_get_ast(mnc_parser_t *parser);

// AST functions
void ast_node_free(ast_node_t *node);

#endif // MNC_PARSER_H
