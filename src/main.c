#include <stdio.h>
#include <ctype.h>
#include "lexer.h"


void tokenize()
{
    // └─ next_token() (w pętli)
}

void parser()
{
    //  │    ├─ parse_func()
    //  │    │    ├─ parse_block()
    // │    │    │    └─ parse_stmt()
    // │    │    │         └─ parse_expr()
}

void c_code_gen()
{
    // └─ gen_node() → (rekurencja po AST)
}

void build_and_run()
{
    // gcc ... ""
}

Token tokens[MAX_TOKENS];
int token_count = 0;

static const char *token_type_name(TokenType t){
    switch (t) {
    case TOKEN_IF: return "IF";
    case TOKEN_ELSE: return "ELSE";
    case TOKEN_WHILE: return "WHILE";
    case TOKEN_PRINT: return "PRINT";
    case TOKEN_RETURN: return "RETURN";
    case  TOKEN_IDENT: return "IDENT";
    case TOKEN_NUMBER: return "NUMBER";
    case TOKEN_NEWLINE: return "NEWLINE";

    case TOKEN_PLUS: return "PLUS";
    case TOKEN_MINUS: return "MINUS";
    case TOKEN_COLON: return "COLON";
    case TOKEN_LPAREN: return "RPAREN";
    case TOKEN_RPAREN: return "RPAREN";
    case TOKEN_LBRACKET: return "LBRACKET";
    case TOKEN_RBRACKET: return "RBRACKET";
    case TOKEN_LBRACE: return "LBRACE";
    case TOKEN_RBRACE: return "RBRACE";
    case TOKEN_SLASH: return "SLASH";
    case TOKEN_COMMA: return "COMMA";
    case TOKEN_DOT: return "DOT";

    case TOKEN_EQ: return "EQ";
    case TOKEN_EQEQ: return "EQEQ";

    case TOKEN_LE: return "LE";
    case TOKEN_LT: return "LT";
    case TOKEN_GE: return "GE";
    case TOKEN_GT: return "GT";
    
    case TOKEN_NEQ: return "NEQ";

    case TOKEN_UNKNOWN: return "UNKNOWN";
    case TOKEN_ERROR: return "ERROR";
    case TOKEN_EOF: return "EOF";

    default: return "???";
    }
}



int main(int argc, char ** argv){
    if (argc < 2) {
        fprintf(stderr, "Uzycie: %s <sciezka_do_pliku\n", argv[0]);
        return 1;
    }

    lexer(argv[1], tokens, &token_count);
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].text[0] == '\0'){
            printf("%s\n", token_type_name(tokens[i].type));
        }else{
             printf("%s(%s)\n", token_type_name(tokens[i].type), tokens[i].text);
        }
}
    
    return 0;
}