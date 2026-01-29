#ifndef AST_H
#define AST_H

#include "lexer.h"

typedef enum {
    NODE_IF,
    NODE_WHILE,
    NODE_ASSIGN,
    NODE_BINOP,
    NODE_LITERAL,
    NODE_IDENT,
    NODE_BLOCK,
    NODE_RETURN,
    NODE_PRINT,

    // functions
    NODE_CALL,
    NODE_FUNCDEF,
} NodeType;

typedef struct Node {
    NodeType type;

    int int_value;
    char name[64];
    TokenType op;

    struct Node *left;
    struct Node *right;
    struct Node *cond;
    struct Node *body;       // block body / while body / return expr / print expr / call args / funcdef body

    struct Node *else_body;
    struct Node *next;       // list: statements OR args

    // for funcdef
    int param_count;
    char params[32][64];
} Node;

void ast_print(Node *n, int indent);

Node *new_literal(int v);
Node *new_ident(const char *s);
Node *new_binop(TokenType op, Node *l, Node *r);

Node *new_assign(Node *l, Node *r);
Node *new_if(Node *cond, Node *body, Node *else_body);
Node *new_while(Node *cond, Node *body);
Node *new_block(Node *first_stmt);

Node *new_return(Node *expr);
Node *new_print(Node *expr);

// functions
Node *new_call(const char *fn_name, Node *args_first);
Node *new_funcdef(const char *fn_name, char params[][64], int param_count, Node *body);

#endif