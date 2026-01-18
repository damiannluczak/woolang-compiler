#include <stdio.h>
#include "env.h"

int main() {
    Env env;
    env_init(&env);

    env_set(&env, "x", 42);
    env_set(&env, "y", 7);

    int v;
    if (env_get(&env, "x", &v)) printf("x = %d\n", v);
    if (env_get(&env, "y", &v)) printf("y = %d\n", v);
    if (!env_get(&env, "z", &v)) printf("z not found\n");

    return 0;
}