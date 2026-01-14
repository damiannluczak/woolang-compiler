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
        default: return "UNKNOWN";
    }
}

static const char* op_name(TokenType op){
    switch (op) {
        case TOKEN_PLUS:  return "+";
        case TOKEN_MINUS: return "-";
        case TOKEN_LT:    return "<";
        case TOKEN_LE:    return "<=";
        case TOKEN_GT:    return ">";
        case TOKEN_GE:    return ">=";
        case TOKEN_EQEQ:  return "==";
        case TOKEN_NEQ:   return "!=";
        default:          return "?";
    }
}

void ast_print(Node *n, int indent) {
    if (!n) {
        indent_spaces(indent);
        printf("(null)\n");
        return;
    }

    // --- nagłówek noda (1 linia) ---
    indent_spaces(indent);

    if (n->type == NODE_BINOP) {
        printf("BINOP (%s)\n", op_name(n->op));
    } else {
        printf("%s", node_type_name(n->type));

        if (n->type == NODE_LITERAL) {
            printf(" value=%d", n->int_value);
        } else if (n->type == NODE_IDENT) {
            printf(" name=%s", n->name);
        }

        printf("\n");
    }

    // --- dzieci / wnętrze ---
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

        case NODE_LITERAL:
        case NODE_IDENT:
        default:
            break;
    }
}