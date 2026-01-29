#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "env.h"

void env_init(Env *env){
    env->count = 0;
    env->func_count = 0;
    env->parent = NULL;
}

int env_get(Env *env, const char *name, int *out_value){
    for (int i = 0; i < env->count; i++){
        if (strcmp(env->entries[i].name, name) == 0){
            *out_value = env->entries[i].value;
            return 1;
        }
    }
    if (env->parent) return env_get(env->parent, name, out_value);
    return 0;
}

// UWAGA: to jest wersja “lexical scoping + update parent if exists”
void env_set(Env *env, const char *name, int value){
    // jeśli istnieje w tym scope → podmień
    for (int i = 0; i < env->count; i++){
        if (strcmp(env->entries[i].name, name) == 0){
            env->entries[i].value = value;
            return;
        }
    }

    // jeśli istnieje w rodzicu → aktualizuj w rodzicu
    if (env->parent){
        int tmp;
        if (env_get(env->parent, name, &tmp)){
            env_set(env->parent, name, value);
            return;
        }
    }

    // inaczej: nowa zmienna w tym scope
    if (env->count >= ENV_MAX_VARS){
        fprintf(stderr, "env_set: ENV_MAX_VARS exceeded\n");
        exit(1);
    }

    strncpy(env->entries[env->count].name, name, ENV_NAME_MAX - 1);
    env->entries[env->count].name[ENV_NAME_MAX - 1] = '\0';
    env->entries[env->count].value = value;
    env->count++;
}

void env_set_func(Env *env, Node *fn_def){
    if (!fn_def || fn_def->type != NODE_FUNCDEF){
        fprintf(stderr, "env_set_func: expected NODE_FUNCDEF\n");
        exit(1);
    }

    // jeśli już jest w tym scope → podmień
    for (int i = 0; i < env->func_count; i++){
        if (strcmp(env->funcs[i].name, fn_def->name) == 0){
            env->funcs[i].fn_def = fn_def;
            return;
        }
    }

    if (env->func_count >= ENV_MAX_FUNCS){
        fprintf(stderr, "env_set_func: ENV_MAX_FUNCS exceeded\n");
        exit(1);
    }

    strncpy(env->funcs[env->func_count].name, fn_def->name, ENV_NAME_MAX - 1);
    env->funcs[env->func_count].name[ENV_NAME_MAX - 1] = '\0';
    env->funcs[env->func_count].fn_def = fn_def;
    env->func_count++;
}

int env_get_func(Env *env, const char *name, Node **out_fn_def){
    for (int i = 0; i < env->func_count; i++){
        if (strcmp(env->funcs[i].name, name) == 0){
            *out_fn_def = env->funcs[i].fn_def;
            return 1;
        }
    }
    if (env->parent) return env_get_func(env->parent, name, out_fn_def);
    return 0;
}