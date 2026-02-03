#ifndef EVAL_H
#define EVAL_H

#include "ast.h"
#include "env.h"

typedef struct {
    int value;
    int is_return;
    int is_break;
    int is_continue;
} EvalResult;
EvalResult eval(Node *n, Env *env);

#endif