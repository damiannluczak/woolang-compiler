#ifndef EVAL_H
#define EVAL_H

#include "ast.h"
#include "env.h"

typedef struct {
    int value;
    int is_return;   // 1 jeśli trafiliśmy na return w aktualnym wykonaniu
} EvalResult;

EvalResult eval(Node *n, Env *env);

#endif