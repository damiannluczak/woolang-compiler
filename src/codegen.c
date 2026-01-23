#include <stdio.h>
#include <string.h>
#include "codegen.h"

#define VAR_MAX 128
#define VAR_NAME_MAX 64

typedef struct {
   char names[VAR_MAX][VAR_NAME_MAX];
   int count;
} VarList;

// forward declarations
static void gen_node(Node *n, FILE *out, int indent);
static void gen_indent(FILE *out, int indent);
static void gen_expr(Node *n, FILE *out);

static int var_exists(VarList *vars, const char *name);
static void collect_vars(Node *n, VarList *vars);



static int var_exists(VarList *vars, const char *name){
    for (int i = 0; i < vars->count; i++){
        if (strcmp(vars->names[i], name) == 0) 
            return 1;
    }
    return 0;
}
static void collect_vars(Node *n, VarList *vars){
    if (!n) return;
    
    switch (n->type){

        case NODE_ASSIGN: {
            if (n->left && n->left->type == NODE_IDENT){
                if (!var_exists(vars, n->left->name)) {
                    strncpy(vars->names[vars->count], n->left->name, VAR_NAME_MAX - 1);
                    vars->names[vars->count][VAR_NAME_MAX - 1] = '\0';
                    vars->count++;
                }
            }
            collect_vars(n->right, vars);
            break;
        }
        case NODE_BLOCK: {
            Node *cur = n->body;
            while (cur){
                collect_vars(cur, vars);
                cur = cur->next;
            }
            break;
        }
        case NODE_IF :{
            collect_vars(n->body, vars);
            collect_vars(n->cond, vars);
            collect_vars(n->else_body, vars);
            break;
        }
        case NODE_WHILE: {
            collect_vars(n->cond, vars);
            collect_vars(n->body, vars);
            break;
        }
        case NODE_PRINT:
        case NODE_RETURN: {
            collect_vars(n->body, vars);
            break;
        }
        case NODE_BINOP: {
            collect_vars(n->left, vars);
            collect_vars(n->right, vars);
            break;
        }
        default:
            break;
    }
}

void gen_program(Node *root, FILE *out) {
    
    fprintf(out, "#include <stdio.h>\n\n");
    fprintf(out, "int main() {\n");

    // 1 pass zebranie zmiennych do deklaracji
    VarList vars = {0};
    collect_vars(root, &vars);

    for (int i = 0; i < vars.count; i ++){
        gen_indent(out, 1);
        fprintf(out, " int %s;\n", vars.names[i]);
    }

    gen_node(root, out, 1);

    // domyślne zakończenie programu (żeby C się nie pluło)
    fprintf(out, "    return 0;\n");
    fprintf(out, "}\n");
}

static void gen_indent(FILE *out, int indent) {
    for (int i = 0; i < indent; i++) {
        fprintf(out, "    "); // 4* spacja 
    }
}
static void gen_expr(Node *n, FILE *out){
    if (!n) {
        fprintf(out, "/*null*/");
        return;
    }

    switch (n->type){
        case NODE_LITERAL:
            fprintf(out, "%d", n->int_value);
            return;

        case NODE_IDENT:
            fprintf(out, "%s", n->name);
            return;

        case NODE_BINOP: {
            fprintf(out, "(");
            gen_expr(n->left, out);

            switch (n->op){
                case TOKEN_PLUS:  fprintf(out, " + ");  break;
                case TOKEN_MINUS: fprintf(out, " - ");  break;

                case TOKEN_LT:    fprintf(out, " < ");  break;
                case TOKEN_LE:    fprintf(out, " <= "); break;
                case TOKEN_GT:    fprintf(out, " > ");  break;
                case TOKEN_GE:    fprintf(out, " >= "); break;

                case TOKEN_EQEQ:  fprintf(out, " == "); break;
                case TOKEN_NEQ:   fprintf(out, " != "); break;

                default:
                    fprintf(out, " /*op?*/ ");
                    break;
            }

            gen_expr(n->right, out);
            fprintf(out, ")");
            return;
        }

        default:
            fprintf(out, "/*expr node type %d not supported*/", n->type);
            return;
    }
}

static void gen_node(Node *n, FILE *out, int indent) {
    if (!n) return;

    switch (n->type) {
        case NODE_BLOCK: {
            Node *cur = n->body;
            while (cur) {
                gen_node(cur, out, indent);
                cur = cur->next;
            }
            break;
        }
        case NODE_ASSIGN: {
            if (!n->left || n->left->type != NODE_IDENT){ 
                gen_indent(out, indent);
                fprintf(out, "/*ERROR: assign lhs not ident */\n");
                break;
            }
            gen_indent(out, indent);
            fprintf(out, "%s = ", n->left->name);
            gen_expr(n->right, out);
            fprintf(out, ";\n");
            break;
        }
        case NODE_PRINT: { 
            gen_indent(out, indent);
            fprintf(out, "printf(\"%%d\\n\", ");
            gen_expr(n->body, out);
            fprintf(out, ");\n");
            break;
        }
        case NODE_RETURN: {
            gen_indent(out, indent);
            fprintf(out, "return ");
            gen_expr(n->body, out);
            fprintf(out, ";\n");
            break;
        }
        case NODE_IF: {
            gen_indent(out, indent);
            fprintf(out,"if (");
            gen_expr(n->cond, out);
            fprintf(out, ")  {\n");

            gen_node(n->body, out, indent + 1);

            gen_indent(out, indent);
            fprintf(out, "}");

            if (n->else_body) { 
                fprintf(out, " else {\n");
                gen_node(n->else_body, out, indent + 1);
                gen_indent(out, indent);
                fprintf(out, "}\n");
            } else {
                fprintf(out, "\n");
            }
            break;
        }

        case NODE_WHILE: {
            gen_indent(out, indent);
            fprintf(out, "while (");
            gen_expr(n->cond, out);
            fprintf(out, ") {\n");

            gen_node(n->body, out, indent + 1);

            gen_indent(out, indent);
            fprintf(out, "}\n");
            break;
        }
        default:
    
            gen_indent(out, indent);
            fprintf(out, "// TODO: node type %d\n", n->type);
            break;
    }
}