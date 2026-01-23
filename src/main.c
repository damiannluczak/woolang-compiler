#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "env.h"
#include "eval.h"
#include "codegen.h"

int main(int argc, char **argv){
    if (argc < 2) {
        fprintf(stderr, "Uzycie: %s <plik.woo> [--ast|--eval|--emit-c]\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];
    const char *mode = (argc >= 3) ? argv[2] : "--eval";

    Token tokens[MAX_TOKENS];
    int token_count = 0;

    lexer(path, tokens, &token_count);

    TokenStream ts = {
        .tokens = tokens,
        .count  = token_count,
        .pos    = 0
    };

    Node *root = parse_program(&ts);

    if (strcmp(mode, "--ast") == 0) {
        ast_print(root, 0);
        return 0;
    }

    if (strcmp(mode, "--emit-c") == 0) {
        gen_program(root, stdout);
        return 0;
    }

    // --eval (domyślnie)
    Env env;
    env_init(&env);
    EvalResult r = eval(root, &env);

    
    return 0;
}