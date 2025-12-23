#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

static const char *token_type_name(TokenType t){
    switch (t) {
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_PRINT: return "PRINT";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_IDENT: return "IDENT";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_NEWLINE: return "NEWLINE";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_COLON: return "COLON";
        case TOKEN_LPAREN: return "LPAREN";
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

static const Token EOF_TOKEN = { .type = TOKEN_EOF, .text = "EOF" };

// forward declarations
static void parse_condition(TokenStream *ts);
static void parse_atom(TokenStream *ts);
static void parse_if(TokenStream *ts);
static void parse_block(TokenStream *ts);
static void parse_stmt(TokenStream *ts);

static Token *peek(TokenStream *ts){
    if (ts->pos >= ts->count) return (Token*)&EOF_TOKEN;
    return &ts->tokens[ts->pos];
}

static Token *advance(TokenStream *ts){
    Token *t = peek(ts);
    if (ts->pos < ts->count) ts->pos++;
    return t;
}

static Token *expect(TokenStream *ts, TokenType type, const char *msg){
    Token *t = peek(ts);
    if (t->type != type){
        fprintf(stderr, "Syntax error: %s\n", msg);
        fprintf(stderr, "Got token: %s\n", token_type_name(t->type));
        exit(1);
    }
    return advance(ts);
}

static void parse_atom(TokenStream *ts){
    Token *t = peek(ts);

    if (t->type == TOKEN_IDENT || t->type == TOKEN_NUMBER){
        advance(ts);
        return;
    }

    if (t->type == TOKEN_LPAREN){
        advance(ts);
        parse_condition(ts);
        expect(ts, TOKEN_RPAREN, "expected ')' after condition");
        return;
    }

    fprintf(stderr, "Syntax error: expected IDENT/NUMBER/(...) \n");
    fprintf(stderr, "Got token: %s\n", token_type_name(t->type));
    exit(1);
}


static void parse_expr(TokenStream * ts){
    parse_atom(ts);

    while (1){
        TokenType tt = peek(ts)->type;

        if (tt == TOKEN_PLUS || tt == TOKEN_MINUS){
            advance(ts);
            parse_atom(ts);
        } else{
            break;
        }
    }
}

static void parse_condition(TokenStream *ts){
    parse_atom(ts);

    Token *t = peek(ts);
    if (t->type == TOKEN_LT || t->type == TOKEN_LE ||
        t->type == TOKEN_GE || t->type == TOKEN_GT ||
        t->type == TOKEN_EQEQ || t->type == TOKEN_NEQ) {
        advance(ts);
    } else {
        fprintf(stderr, "Syntax error: expected comparison operator\n");
        fprintf(stderr, "Got token: %s\n", token_type_name(t->type));
        exit(1);
    }

    parse_atom(ts);
}

static void parse_if(TokenStream *ts){
    expect(ts, TOKEN_IF, "expected 'if'");
    parse_condition(ts);
    expect(ts, TOKEN_COLON, "expected ':' after condition");
    expect(ts, TOKEN_NEWLINE, "expected newline after ':'");

    parse_block(ts);

    if (peek(ts) -> type == TOKEN_ELSE){
        advance(ts);
        expect(ts, TOKEN_COLON, "expected ':' after else ");
        expect(ts, TOKEN_NEWLINE, " expected newline after else ':' ");
        parse_block(ts);
    }
}

static void parse_stmt(TokenStream *ts){
    Token *t = peek(ts);

    switch (t->type){
        case TOKEN_IF:
            parse_if(ts);
            break;

        case TOKEN_RETURN:
            expect(ts, TOKEN_RETURN, "expected 'return'");
            parse_expr(ts);
            break;

        case TOKEN_PRINT:
            expect(ts, TOKEN_PRINT, "expected 'print'");
            parse_expr(ts);
            break;

        case TOKEN_IDENT:
            advance(ts);
            expect(ts, TOKEN_EQ, "expected '='");
            parse_expr(ts);
            break;

        default:
            fprintf(stderr, "Parser error: unexpected token %s\n", token_type_name(t->type));
            exit(1);
    }

    while (peek(ts)->type == TOKEN_NEWLINE) advance(ts);
}

static void parse_block(TokenStream *ts){
    while (peek(ts)->type == TOKEN_NEWLINE) advance(ts);

    while (1){
        Token *t = peek(ts);
        if (t->type == TOKEN_IF || t->type == TOKEN_WHILE ||
            t->type == TOKEN_RETURN || t->type == TOKEN_PRINT ||
            t->type == TOKEN_IDENT) {

            parse_stmt(ts);
            while (peek(ts)->type == TOKEN_NEWLINE) advance(ts);

        } else {
            break;
        }
    }
}

void parse_program(TokenStream *ts){
    while (peek(ts)->type == TOKEN_NEWLINE) {
        advance(ts);
    }

    while (peek(ts)->type != TOKEN_EOF) {
        parse_stmt(ts);

        while (peek(ts)->type == TOKEN_NEWLINE) {
            advance(ts);
        }
    }
}