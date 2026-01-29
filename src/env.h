#ifndef ENV_H
#define ENV_H

#include "ast.h"

#define ENV_NAME_MAX   64
#define ENV_MAX_VARS   128
#define ENV_MAX_FUNCS  128

typedef struct {
    char name[ENV_NAME_MAX];
    int  value;
} EnvEntry;

typedef struct {
    char name[ENV_NAME_MAX];
    Node *fn_def;              // NODE_FUNCDEF
} EnvFuncEntry;

typedef struct Env {
    EnvEntry entries[ENV_MAX_VARS];
    int count;

    EnvFuncEntry funcs[ENV_MAX_FUNCS];
    int func_count;

    struct Env *parent;        // scope chain
} Env;

void env_init(Env *env);

int  env_get(Env *env, const char *name, int *out_value);
void env_set(Env *env, const char *name, int value);

// funkcje (trzymamy Node* do NODE_FUNCDEF)
void env_set_func(Env *env, Node *fn_def);
int  env_get_func(Env *env, const char *name, Node **out_fn_def);

#endif