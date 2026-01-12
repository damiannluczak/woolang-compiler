#include <stdlib.h>
#include <string.h>
#include "ast.h"

static Node *node_alloc(NodeType type) {
    Node *n = (Node*)calloc(1, sizeof(Node));
    if (!n) exit(1);
    n->type = type;
    return n;
}

Node *new_literal(int v) {
    Node *n = node_alloc(NODE_LITERAL);
    n->int_value = v;
    return n;
}

Node *new_ident(const char *s) {
    Node *n = node_alloc(NODE_IDENT);
    strncpy(n->name, s, sizeof(n->name) - 1);
    n->name[sizeof(n->name) - 1] = '\0';
    return n;
}

Node *new_binop(TokenType op, Node *l, Node *r) {
    Node *n = node_alloc(NODE_BINOP);
    n->op = op;
    n->left = l;
    n->right = r;
    return n;
}

Node *new_assign(Node *lhs, Node *rhs){
    Node *n = node_alloc(NODE_ASSIGN);

    n->left = lhs;
    n->right = rhs;
    return n;
}

Node *new_if(Node *cond, Node *body, Node *else_body){
    Node *n = node_alloc(NODE_IF);
    n->cond = cond;
    n->body = body;
    n->else_body = else_body;
    return n;
}

Node *new_while(Node *cond, Node *body){
    Node *n = node_alloc(NODE_WHILE);
    n->cond = cond;
    n->body = body;
    return n;
}

Node *new_block(Node *first_stmt){
    Node *n = node_alloc(NODE_BLOCK);
    n->body = first_stmt;
    return n;
}

Node *new_return(Node *expr){
    Node *n = node_alloc(NODE_RETURN);
    n->body = expr;
    return n;
}

Node * new_print(Node *expr){
    Node *n = node_alloc(NODE_PRINT);
    n->body = expr;
    return n;
}

