#include <stdio.h>
#include <stdlib.h>
#include "eval.h"

EvalResult eval(Node *n, Env *env){
    EvalResult res;
    res.value = 0;
    res.has_return = 0;

    if (!n) return res;

    switch (n->type){
        case NODE_LITERAL:
            res.value = n->int_value;
            return res;

        case NODE_IDENT:
            if(!env_get(env, n->name, &res.value)){
                fprintf(stderr, "eval: undefined var '%s'\n", n->name);
                exit(1);
            }
            return res;

        case NODE_ASSIGN: {
            EvalResult rhs = eval(n->right, env);

            if (!n->left || n->left->type != NODE_IDENT){
                fprintf(stderr, "eval: left side of assignment must be identifier\n");
                exit(1);
            }
            env_set(env, n->left->name, rhs.value);

            res.value = rhs.value;
            return res;
        }
        case NODE_BINOP: {
            EvalResult lhs = eval(n->left,  env);
            if (lhs.has_return) return lhs;

            EvalResult rhs = eval(n->right, env);
            if (rhs.has_return) return rhs;

            switch (n->op){
                case TOKEN_PLUS:
                    res.value = lhs.value + rhs.value;
                    return res;

                case TOKEN_MINUS:
                    res.value = lhs.value - rhs.value;
                    return res;
                case TOKEN_LT:
                    res.value = (lhs.value < rhs.value);
                    return res;
                case TOKEN_LE:
                    res.value = (lhs.value <= rhs.value);
                    return res;
                case TOKEN_GT:
                    res.value = (lhs.value > rhs.value);
                    return res;
                case TOKEN_GE:
                    res.value = (lhs.value >= rhs.value);
                    return res;
                case TOKEN_EQEQ:
                    res.value = (lhs.value == rhs.value);
                    return res;
                case TOKEN_NEQ:
                    res.value = (lhs.value != rhs.value);
                    return res;
                default:
                    fprintf(stderr, "eval: unsupported binop token %d\n", n->op);
                    exit(1);
            }
            return res;
        }

        case NODE_BLOCK: {
            Node *cur = n->body;
            
            EvalResult last;
            last.value = 0;
            last.has_return = 0;

            while(cur) {
                 last = eval(cur, env);
                if (last.has_return) return last;
                cur = cur->next;
           }
           return last;
        }
        case NODE_IF: {
            EvalResult c = eval(n->cond, env);
            if (c.has_return) return c;

            if (c.value) {
                return eval(n->body, env); 
            } else if (n->else_body) {
                return eval(n->else_body,env);
            } else {
                return res;
            }
        }
        case NODE_WHILE: {
            while(1){
                EvalResult cond = eval(n->cond, env);
                if (cond.has_return) return cond;

                if (cond.value == 0){
                    res.value = 0;
                    res.has_return = 0;
                    return res;
                }
                EvalResult body = eval(n->body, env);
                if (body.has_return) return body;
            }
        }

        case NODE_PRINT: {
            EvalResult v = eval(n->body, env);
            if (v.has_return) return v;

            printf("%d\n", v.value);

            res.value = v.value;
            return res;
        }

        case NODE_RETURN: {
            EvalResult v = eval(n->body, env);
            v.has_return = 1;
            return v;
        }




        default:
            fprintf(stderr, "eval: unsupported node type %d\n", n->type);
            exit(1);
    }
}