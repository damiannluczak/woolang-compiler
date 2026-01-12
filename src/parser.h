#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Token *tokens;
    int count;
    int pos;
} TokenStream;

Node *parse_program(TokenStream *ts);

#endif
