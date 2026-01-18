#include "string.h"
#include "stdio.h"
#include <stdlib.h> 
#include "env.h"



void env_init(Env * env){

    env->count = 0;

}

int env_get(Env * env, const char *name, int *out_value){

    for (int i = 0; i < env->count; i++){
        if (strcmp(env->entries[i].name, name) == 0){
            *out_value = env->entries[i].value;
            return 1;
        }
    }
    return 0;
}


void env_set(Env * env, const char *name, int value){

    for (int i = 0; i < env->count; i++){
        if (strcmp(env->entries[i].name, name) == 0){
            env->entries[i].value = value;
            return;
        }
    }
    if (env->count >= ENV_MAX){
        fprintf(stderr, "evn_set: env full\n");
        exit(1);
    }
     // skopiuj nazwę
    strncpy(env->entries[env->count].name, name, sizeof(env->entries[env->count].name) - 1);
    env->entries[env->count].name[sizeof(env->entries[env->count].name) - 1] = '\0';


    // ustaw wartość i zwiększ count
    env->entries[env->count].value = value;
    env->count++;

}
