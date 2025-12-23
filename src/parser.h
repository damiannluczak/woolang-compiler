#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct {
    Token *tokens;
    int count;
    int pos;
} TokenStream;

void parse_program(TokenStream *ts);

#endif
