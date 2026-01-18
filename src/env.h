#ifndef ENV_H
#define ENV_H

#include "ast.h"

#define ENV_MAX 128

typedef struct {
    char name[64];
    int value;

} EnvEntry;

typedef struct
{
    EnvEntry entries[ENV_MAX];
    int count;
} Env;

void env_init(Env *env);
int env_get (Env * env, const char *name, int *out_value);
void env_set(Env * env, const char * name, int value);

#endif