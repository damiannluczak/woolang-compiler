#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

int main(int argc, char **argv){
    if (argc < 2) {
        fprintf(stderr, "Uzycie: %s <sciezka_do_pliku>\n", argv[0]);
        return 1;
    }

    Token tokens[MAX_TOKENS];
    int token_count = 0;

    lexer(argv[1], tokens, &token_count);

    TokenStream ts = {
        .tokens = tokens,
        .count  = token_count,
        .pos    = 0
    };

    Node *root = parse_program(&ts);
    ast_print(root, 0);
    return 0;
}