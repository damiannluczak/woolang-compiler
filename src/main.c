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
        fprintf(stderr, "Usage: %s <file.woo> [--ast|--eval|--emit-c]\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];
    const char *mode = (argc >= 3) ? argv[2] : "--eval";

    /* ===== LEXER ===== */
    Token tokens[MAX_TOKENS];
    int token_count = 0;
    lexer(path, tokens, &token_count);

    TokenStream ts = {
        .tokens = tokens,
        .count  = token_count,
        .pos    = 0
    };


    /* ===== PARSER ===== */
    Node *root = parse_program(&ts);

    /* ===== AST PRINT ===== */
    if (strcmp(mode, "--ast") == 0) {
        ast_print(root, 0);
        return 0;
    }

    /* ===== CODEGEN ===== */
    if (strcmp(mode, "--emit-c") == 0) {
        gen_program(root, stdout);
        return 0;
    }

    /* ===== EVAL (default) ===== */
    Env env;
    env_init(&env);

    EvalResult r = eval(root, &env);

    printf("Program result = %d\n", r.value);
    return r.value;
}