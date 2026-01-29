#include <stdio.h>
#include <stdlib.h>
#include "eval.h"
#include "lexer.h"   // TokenType

static EvalResult ok(int v){ return (EvalResult){ .value = v, .is_return = 0 }; }
static EvalResult ret(int v){ return (EvalResult){ .value = v, .is_return = 1 }; }

static int eval_binop(TokenType op, int a, int b){
    switch (op){
        case TOKEN_PLUS:  return a + b;
        case TOKEN_MINUS: return a - b;
        case TOKEN_SLASH: return b == 0 ? 0 : (a / b);
        case TOKEN_PERCENT: return b == 0 ? 0 : (a % b);

        case TOKEN_LT:   return a <  b;
        case TOKEN_LE:   return a <= b;
        case TOKEN_GT:   return a >  b;
        case TOKEN_GE:   return a >= b;
        case TOKEN_EQEQ: return a == b;
        case TOKEN_NEQ:  return a != b;

        default:
            fprintf(stderr, "eval_binop: unsupported op %d\n", op);
            exit(1);
    }
}

static EvalResult eval_node(Node *n, Env *env);

static EvalResult eval_block(Node *block, Env *env){
    // block->type == NODE_BLOCK
    Node *cur = block->body;
    while (cur){
        EvalResult r = eval_node(cur, env);
        if (r.is_return) return r;
        cur = cur->next;
    }
    return ok(0);
}

static EvalResult eval_call(Node *call, Env *env){
    // call->type == NODE_CALL
    Node *fn_def = NULL;
    if (!env_get_func(env, call->name, &fn_def) || !fn_def){
        fprintf(stderr, "eval: unknown function '%s'\n", call->name);
        exit(1);
    }

    // policz argumenty i oceń je
    int arg_vals[32];
    int argc = 0;
    Node *arg = call->body; // lista argumentów po next
    while (arg){
        if (argc >= 32){
            fprintf(stderr, "eval: too many args in call '%s'\n", call->name);
            exit(1);
        }
        EvalResult ar = eval_node(arg, env);
        if (ar.is_return){
            // return wewnątrz wyrażenia jako argument nie powinien wystąpić,
            // ale jak wystąpi to propagujemy (bezpiecznie)
            return ar;
        }
        arg_vals[argc++] = ar.value;
        arg = arg->next;
    }

    if (argc != fn_def->param_count){
        fprintf(stderr, "eval: function '%s' expects %d args, got %d\n",
                fn_def->name, fn_def->param_count, argc);
        exit(1);
    }

    // nowy lokalny scope
    Env local;
    env_init(&local);
    local.parent = env;

    // bind parametrów
    for (int i = 0; i < fn_def->param_count; i++){
        env_set(&local, fn_def->params[i], arg_vals[i]);
    }

    // wykonaj body funkcji
    EvalResult br = eval_node(fn_def->body, &local);

    // return z funkcji → zwracamy wartość, ale nie propagujemy flagi return na zewnątrz wywołania
    if (br.is_return) return ok(br.value);

    // brak return w funkcji → MVP: 0
    return ok(0);
}

static EvalResult eval_node(Node *n, Env *env){
    if (!n) return ok(0);

    switch (n->type){
        case NODE_LITERAL:
            return ok(n->int_value);

        case NODE_IDENT: {
            int v;
            if (!env_get(env, n->name, &v)){
                fprintf(stderr, "eval: undefined variable '%s'\n", n->name);
                exit(1);
            }
            return ok(v);
        }

        case NODE_ASSIGN: {
            // lhs ident
            if (!n->left || n->left->type != NODE_IDENT){
                fprintf(stderr, "eval: assign lhs not ident\n");
                exit(1);
            }
            EvalResult r = eval_node(n->right, env);
            if (r.is_return) return r;
            env_set(env, n->left->name, r.value);
            return ok(r.value);
        }

        case NODE_BINOP: {
            EvalResult a = eval_node(n->left, env);
            if (a.is_return) return a;
            EvalResult b = eval_node(n->right, env);
            if (b.is_return) return b;
            return ok(eval_binop(n->op, a.value, b.value));
        }

        case NODE_PRINT: {
            EvalResult r = eval_node(n->body, env);
            if (r.is_return) return r;
            printf("%d\n", r.value);
            return ok(r.value);
        }

        case NODE_RETURN: {
            EvalResult r = eval_node(n->body, env);
            // return zawsze zamyka wykonanie
            return ret(r.value);
        }

        case NODE_IF: {
            EvalResult c = eval_node(n->cond, env);
            if (c.is_return) return c;
            if (c.value){
                return eval_node(n->body, env);
            } else if (n->else_body){
                return eval_node(n->else_body, env);
            }
            return ok(0);
        }

        case NODE_WHILE: {
            while (1){
                EvalResult c = eval_node(n->cond, env);
                if (c.is_return) return c;
                if (!c.value) break;

                EvalResult b = eval_node(n->body, env);
                if (b.is_return) return b;
            }
            return ok(0);
        }

        case NODE_BLOCK:
            return eval_block(n, env);

        case NODE_FUNCDEF:
            // definicja – zapisujemy do env, nie wykonujemy
            env_set_func(env, n);
            return ok(0);

        case NODE_CALL:
            return eval_call(n, env);

        default:
            fprintf(stderr, "eval: unsupported node type %d\n", n->type);
            exit(1);
    }
}

EvalResult eval(Node *n, Env *env){
    return eval_node(n, env);
}