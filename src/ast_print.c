// src/ast_print.c
#include <stdio.h>
#include "ast.h"

static void indent_spaces(int indent) {
    for (int i = 0; i < indent; i++) putchar(' ');
}

static const char* node_type_name(NodeType t) {
    switch (t) {
        case NODE_IF: return "IF";
        case NODE_WHILE: return "WHILE";
        case NODE_ASSIGN: return "ASSIGN";
        case NODE_BINOP: return "BINOP";
        case NODE_LITERAL: return "LITERAL";
        case NODE_IDENT: return "IDENT";
        case NODE_BLOCK: return "BLOCK";
        case NODE_RETURN: return "RETURN";
        case NODE_PRINT: return "PRINT";
        case NODE_CALL: return "CALL";
        case NODE_FUNCDEF: return "FUNCDEF";
        default: return "UNKNOWN";
    }
}

static const char* op_name(TokenType op){
    switch (op) {
        case TOKEN_PLUS:    return "+";
        case TOKEN_MINUS:   return "-";
        case TOKEN_SLASH:   return "/";
        case TOKEN_PERCENT: return "%";

        case TOKEN_LT:    return "<";
        case TOKEN_LE:    return "<=";
        case TOKEN_GT:    return ">";
        case TOKEN_GE:    return ">=";
        case TOKEN_EQEQ:  return "==";
        case TOKEN_NEQ:   return "!=";
        default:          return "?";
    }
}

static void print_params(Node *n, int indent) {
    indent_spaces(indent);
    printf("params (%d):", n->param_count);
    for (int i = 0; i < n->param_count; i++) {
        printf(" %s", n->params[i]);
        if (i + 1 < n->param_count) printf(",");
    }
    printf("\n");
}

static void print_arg_list(Node *first_arg, int indent) {
    Node *cur = first_arg;
    int idx = 0;
    while (cur) {
        indent_spaces(indent);
        printf("arg[%d]:\n", idx);
        ast_print(cur, indent + 2);
        cur = cur->next;
        idx++;
    }
}

void ast_print(Node *n, int indent) {
    if (!n) {
        indent_spaces(indent);
        printf("(null)\n");
        return;
    }

    // --- header ---
    indent_spaces(indent);

    if (n->type == NODE_BINOP) {
        printf("BINOP (%s)\n", op_name(n->op));
    } else {
        printf("%s", node_type_name(n->type));

        if (n->type == NODE_LITERAL) {
            printf(" value=%d", n->int_value);
        } else if (n->type == NODE_IDENT) {
            printf(" name=%s", n->name);
        } else if (n->type == NODE_CALL) {
            printf(" name=%s", n->name);
        } else if (n->type == NODE_FUNCDEF) {
            printf(" name=%s", n->name);
        }

        printf("\n");
    }

    // --- children ---
    switch (n->type) {
        case NODE_ASSIGN:
            indent_spaces(indent + 2); printf("lhs:\n");
            ast_print(n->left, indent + 4);

            indent_spaces(indent + 2); printf("rhs:\n");
            ast_print(n->right, indent + 4);
            break;

        case NODE_BINOP:
            indent_spaces(indent + 2); printf("lhs:\n");
            ast_print(n->left, indent + 4);

            indent_spaces(indent + 2); printf("rhs:\n");
            ast_print(n->right, indent + 4);
            break;

        case NODE_IF:
            indent_spaces(indent + 2); printf("cond:\n");
            ast_print(n->cond, indent + 4);

            indent_spaces(indent + 2); printf("then:\n");
            ast_print(n->body, indent + 4);

            if (n->else_body) {
                indent_spaces(indent + 2); printf("else:\n");
                ast_print(n->else_body, indent + 4);
            }
            break;

        case NODE_WHILE:
            indent_spaces(indent + 2); printf("cond:\n");
            ast_print(n->cond, indent + 4);

            indent_spaces(indent + 2); printf("body:\n");
            ast_print(n->body, indent + 4);
            break;

        case NODE_BLOCK: {
            Node *cur = n->body;
            while (cur) {
                ast_print(cur, indent + 2);
                cur = cur->next;
            }
            break;
        }

        case NODE_RETURN:
        case NODE_PRINT:
            ast_print(n->body, indent + 2);
            break;

        case NODE_CALL:
            indent_spaces(indent + 2); printf("args:\n");
            print_arg_list(n->body, indent + 4);
            break;

        case NODE_FUNCDEF:
            print_params(n, indent + 2);
            indent_spaces(indent + 2); printf("body:\n");
            ast_print(n->body, indent + 4);
            break;

        case NODE_LITERAL:
        case NODE_IDENT:
        default:
            break;
    }
}