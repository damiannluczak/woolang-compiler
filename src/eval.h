#ifndef EVAL_H
#define EVAL_H

#include "env.h"
#include "ast.h"


typedef struct  {
    int value;
    int has_return;
} EvalResult;

EvalResult eval(Node *n, Env *env);

#endif