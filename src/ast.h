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
} NodeType;

typedef struct Node { 
    NodeType type;

    int int_value;
    char name[64];
    TokenType op;


    struct Node *left;
    struct Node *right;
    struct Node *cond;
    struct Node *body;


    struct Node *else_body;
    struct Node *next;
} Node;

Node *new_literal(int v);
Node *new_ident(const char *s);
Node *new_binop(TokenType op, Node *l, Node *r);

Node * new_assign(Node *l, Node *r);
Node * new_while(Node *cond, Node *body);

#endif
