#include <stdio.h>
#include "lexer.h"
#include "parser.h"

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

    parse_program(&ts);
    printf("OK: parsed successfully\n");
    return 0;
}