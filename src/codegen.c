// src/codegen.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "codegen.h"

#define VAR_MAX 256
#define VAR_NAME_MAX 64
#define PARAM_MAX 32

typedef struct {
    char names[VAR_MAX][VAR_NAME_MAX];
    int count;
} VarList;

typedef struct {
    char names[PARAM_MAX][VAR_NAME_MAX];
    int count;
} ParamList;

// ---------- helpers ----------
static void gen_node(Node *n, FILE *out, int indent, int *has_return);
static void gen_indent(FILE *out, int indent);
static void gen_expr(Node *n, FILE *out);

static int var_exists(const VarList *vars, const char *name);
static void add_var(VarList *vars, const char *name);

static int param_exists(const ParamList *ps, const char *name);

static void collect_vars_top(Node *n, VarList *vars); // top-level only (skip func bodies)
static void collect_vars_in_func(Node *n, VarList *locals, const ParamList *ps, const VarList *globals);

static void gen_function_defs(Node *root, FILE *out, const VarList *globals);
static void gen_funcdef(Node *fn, FILE *out, const VarList *globals);

// ---------- Var helpers ----------
static int var_exists(const VarList *vars, const char *name) {
    for (int i = 0; i < vars->count; i++) {
        if (strcmp(vars->names[i], name) == 0) return 1;
    }
    return 0;
}

static void add_var(VarList *vars, const char *name) {
    if (vars->count >= VAR_MAX) return;
    if (var_exists(vars, name)) return;

    strncpy(vars->names[vars->count], name, VAR_NAME_MAX - 1);
    vars->names[vars->count][VAR_NAME_MAX - 1] = '\0';
    vars->count++;
}

static int param_exists(const ParamList *ps, const char *name) {
    for (int i = 0; i < ps->count; i++) {
        if (strcmp(ps->names[i], name) == 0) return 1;
    }
    return 0;
}

// ---------- Variable collection ----------
// collect top-level assignments, but DO NOT descend into func bodies
static void collect_vars_top(Node *n, VarList *vars) {
    if (!n) return;

    switch (n->type) {
        case NODE_ASSIGN:
            if (n->left && n->left->type == NODE_IDENT) add_var(vars, n->left->name);
            collect_vars_top(n->right, vars);
            break;

        case NODE_BLOCK: {
            for (Node *cur = n->body; cur; cur = cur->next) collect_vars_top(cur, vars);
            break;
        }

        case NODE_IF:
            collect_vars_top(n->cond, vars);
            collect_vars_top(n->body, vars);
            collect_vars_top(n->else_body, vars);
            break;

        case NODE_WHILE:
            collect_vars_top(n->cond, vars);
            collect_vars_top(n->body, vars);
            break;

        case NODE_PRINT:
        case NODE_RETURN:
            collect_vars_top(n->body, vars);
            break;

        case NODE_BINOP:
            collect_vars_top(n->left, vars);
            collect_vars_top(n->right, vars);
            break;

        case NODE_CALL: {
            for (Node *arg = n->body; arg; arg = arg->next) collect_vars_top(arg, vars);
            break;
        }

        case NODE_FUNCDEF:
            // skip function body at top-level scan
            break;

        default:
            break;
    }
}

// locals in function = variables assigned inside function that are NOT params and NOT globals
static void collect_vars_in_func(Node *n, VarList *locals, const ParamList *ps, const VarList *globals) {
    if (!n) return;

    switch (n->type) {
        case NODE_ASSIGN:
            if (n->left && n->left->type == NODE_IDENT) {
                const char *name = n->left->name;
                if (!param_exists(ps, name) && !var_exists(globals, name)) {
                    add_var(locals, name);
                }
            }
            collect_vars_in_func(n->right, locals, ps, globals);
            break;

        case NODE_BLOCK: {
            for (Node *cur = n->body; cur; cur = cur->next) collect_vars_in_func(cur, locals, ps, globals);
            break;
        }

        case NODE_IF:
            collect_vars_in_func(n->cond, locals, ps, globals);
            collect_vars_in_func(n->body, locals, ps, globals);
            collect_vars_in_func(n->else_body, locals, ps, globals);
            break;

        case NODE_WHILE:
            collect_vars_in_func(n->cond, locals, ps, globals);
            collect_vars_in_func(n->body, locals, ps, globals);
            break;

        case NODE_PRINT:
        case NODE_RETURN:
            collect_vars_in_func(n->body, locals, ps, globals);
            break;

        case NODE_BINOP:
            collect_vars_in_func(n->left, locals, ps, globals);
            collect_vars_in_func(n->right, locals, ps, globals);
            break;

        case NODE_CALL: {
            for (Node *arg = n->body; arg; arg = arg->next) collect_vars_in_func(arg, locals, ps, globals);
            break;
        }

        case NODE_FUNCDEF:
            // MVP: ignore nested func defs
            break;

        default:
            break;
    }
}

// ---------- Codegen entry ----------
void gen_program(Node *root, FILE *out) {
    fprintf(out, "#include <stdio.h>\n\n");

    // 1) collect globals (top-level variables)
    VarList globals = (VarList){0};
    collect_vars_top(root, &globals);

    // 2) emit globals at file scope (IMPORTANT for functions)
    for (int i = 0; i < globals.count; i++) {
        fprintf(out, "int %s = 0;\n", globals.names[i]);
    }
    if (globals.count > 0) fprintf(out, "\n");

    // 3) emit function definitions
    gen_function_defs(root, out, &globals);

    // 4) emit main (NO re-declare globals)
    fprintf(out, "int main() {\n");

    int has_return = 0;
    gen_node(root, out, 1, &has_return);

    if (!has_return) {
        gen_indent(out, 1);
        fprintf(out, "return 0;\n");
    }

    fprintf(out, "}\n");
}

// ---------- Function defs ----------
static void gen_function_defs(Node *root, FILE *out, const VarList *globals) {
    if (!root || root->type != NODE_BLOCK) return;

    for (Node *cur = root->body; cur; cur = cur->next) {
        if (cur->type == NODE_FUNCDEF) {
            gen_funcdef(cur, out, globals);
            fprintf(out, "\n");
        }
    }
}

static void gen_funcdef(Node *fn, FILE *out, const VarList *globals) {
    // Map: if param collides with global, use synthetic name in signature (_aN),
    // then bind at function start: globalName = _aN;
    int param_is_global[PARAM_MAX] = {0};

    fprintf(out, "int %s(", fn->name);
    for (int i = 0; i < fn->param_count; i++) {
        if (i) fprintf(out, ", ");

        const char *pname = fn->params[i];
        int collides = var_exists(globals, pname);
        param_is_global[i] = collides ? 1 : 0;

        if (collides) {
            fprintf(out, "int _a%d", i);
        } else {
            fprintf(out, "int %s", pname);
        }
    }
    fprintf(out, ") {\n");

    // ParamList for locals collection: keep ORIGINAL param names
    ParamList ps = (ParamList){0};
    ps.count = fn->param_count;
    for (int i = 0; i < fn->param_count && i < PARAM_MAX; i++) {
        strncpy(ps.names[i], fn->params[i], VAR_NAME_MAX - 1);
        ps.names[i][VAR_NAME_MAX - 1] = '\0';
    }

    // Bind params that collide with globals to globals at function start
    for (int i = 0; i < fn->param_count && i < PARAM_MAX; i++) {
        if (param_is_global[i]) {
            gen_indent(out, 1);
            fprintf(out, "%s = _a%d;\n", fn->params[i], i);
        }
    }

    // declare ONLY real locals (not globals, not params)
    VarList locals = (VarList){0};
    collect_vars_in_func(fn->body, &locals, &ps, globals);

    for (int i = 0; i < locals.count; i++) {
        gen_indent(out, 1);
        fprintf(out, "int %s = 0;\n", locals.names[i]);
    }

    int has_ret = 0;
    gen_node(fn->body, out, 1, &has_ret);

    if (!has_ret) {
        gen_indent(out, 1);
        fprintf(out, "return 0;\n");
    }

    fprintf(out, "}\n");
}

// ---------- Statements ----------
static void gen_node(Node *n, FILE *out, int indent, int *has_return) {
    if (!n) return;

    switch (n->type) {
        case NODE_BLOCK:
            for (Node *cur = n->body; cur; cur = cur->next) gen_node(cur, out, indent, has_return);
            break;

        case NODE_FUNCDEF:
            // already emitted above main
            break;

        case NODE_ASSIGN:
            if (!n->left || n->left->type != NODE_IDENT) {
                gen_indent(out, indent);
                fprintf(out, "/*ERROR: assign lhs not ident*/\n");
                break;
            }
            gen_indent(out, indent);
            fprintf(out, "%s = ", n->left->name);
            gen_expr(n->right, out);
            fprintf(out, ";\n");
            break;

        case NODE_PRINT:
            gen_indent(out, indent);
            fprintf(out, "printf(\"%%d\\n\", ");
            gen_expr(n->body, out);
            fprintf(out, ");\n");
            break;

        case NODE_RETURN:
            *has_return = 1;
            gen_indent(out, indent);
            fprintf(out, "return ");
            gen_expr(n->body, out);
            fprintf(out, ";\n");
            break;

        case NODE_IF:
            gen_indent(out, indent);
            fprintf(out, "if (");
            gen_expr(n->cond, out);
            fprintf(out, ") {\n");
            gen_node(n->body, out, indent + 1, has_return);
            gen_indent(out, indent);
            fprintf(out, "}");
            if (n->else_body) {
                fprintf(out, " else {\n");
                gen_node(n->else_body, out, indent + 1, has_return);
                gen_indent(out, indent);
                fprintf(out, "}\n");
            } else {
                fprintf(out, "\n");
            }
            break;

        case NODE_WHILE:
            gen_indent(out, indent);
            fprintf(out, "while (");
            gen_expr(n->cond, out);
            fprintf(out, ") {\n");
            gen_node(n->body, out, indent + 1, has_return);
            gen_indent(out, indent);
            fprintf(out, "}\n");
            break;

        default:
            // expression statement
            gen_indent(out, indent);
            gen_expr(n, out);
            fprintf(out, ";\n");
            break;
    }
}

// ---------- Expressions ----------
static void gen_expr(Node *n, FILE *out) {
    if (!n) { fprintf(out, "0"); return; }

    switch (n->type) {
        case NODE_LITERAL:
            fprintf(out, "%d", n->int_value);
            return;

        case NODE_IDENT:
            fprintf(out, "%s", n->name);
            return;

        case NODE_CALL: {
            fprintf(out, "%s(", n->name);
            int first = 1;
            for (Node *arg = n->body; arg; arg = arg->next) {
                if (!first) fprintf(out, ", ");
                gen_expr(arg, out);
                first = 0;
            }
            fprintf(out, ")");
            return;
        }

        case NODE_BINOP: {
            fprintf(out, "(");
            gen_expr(n->left, out);

            switch (n->op) {
                case TOKEN_PLUS:    fprintf(out, " + "); break;
                case TOKEN_MINUS:   fprintf(out, " - "); break;
                case TOKEN_SLASH:   fprintf(out, " / "); break;
                case TOKEN_PERCENT: fprintf(out, " %% "); break;

                case TOKEN_LT:      fprintf(out, " < "); break;
                case TOKEN_LE:      fprintf(out, " <= "); break;
                case TOKEN_GT:      fprintf(out, " > "); break;
                case TOKEN_GE:      fprintf(out, " >= "); break;

                case TOKEN_EQEQ:    fprintf(out, " == "); break;
                case TOKEN_NEQ:     fprintf(out, " != "); break;

                default:            fprintf(out, " /*op?*/ "); break;
            }

            gen_expr(n->right, out);
            fprintf(out, ")");
            return;
        }

        default:
            fprintf(out, "0");
            return;
    }
}

static void gen_indent(FILE *out, int indent) {
    for (int i = 0; i < indent; i++) fprintf(out, "    ");
}