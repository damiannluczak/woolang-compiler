#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "ast.h"

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



// forward declarations
static Node *parse_condition(TokenStream *ts);
static Node *parse_atom(TokenStream *ts);
static Node *parse_expr(TokenStream *ts);

static Node *parse_if(TokenStream *ts);
static Node *parse_block(TokenStream *ts);
static Node *parse_stmt(TokenStream *ts);
static Node *parse_while(TokenStream *ts);

static Token EOF_TOKEN = { .type = TOKEN_EOF,  .text = "EOF"};

static Token *peek(TokenStream *ts){
    if (ts->pos >= ts->count) return &EOF_TOKEN;
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

static Node *parse_atom(TokenStream *ts){
    Token *t = peek(ts);

    if (t->type == TOKEN_IDENT){
        advance(ts);
        return new_ident(t->text);
    }
    if (t->type == TOKEN_NUMBER){
        int value = atoi(t->text);
        advance(ts);
        return new_literal(value);
    }

    if (t->type == TOKEN_LPAREN){
        advance(ts);
        Node * expr = parse_expr(ts);
        expect(ts, TOKEN_RPAREN, "expected ')' after expression");
        return expr;
    }

    fprintf(stderr, "Syntax error: expected IDENT/NUMBER/(...) \n");
    fprintf(stderr, "Got token: %s\n", token_type_name(t->type));
    exit(1);
}


static Node *parse_expr(TokenStream * ts){
    Node *left = parse_atom(ts);
    while (1){
        TokenType op = peek(ts)->type;

        if (op == TOKEN_PLUS || op == TOKEN_MINUS){
            advance(ts);
            Node *right = parse_atom(ts);
            left = new_binop(op, left, right);

        } else{
            break;
        }
    }
    return left;
}

static Node *parse_condition(TokenStream *ts){
    Node *lhs = parse_expr(ts);

    TokenType op = peek(ts)->type;
    if (op == TOKEN_LT || op == TOKEN_LE ||
        op == TOKEN_GE || op == TOKEN_GT ||
        op == TOKEN_EQEQ || op == TOKEN_NEQ) {
        advance(ts);
    } else {
        fprintf(stderr, "Syntax error: expected comparison operator\n");
        fprintf(stderr, "Got token: %s\n", token_type_name(op));
        exit(1);
    }
    Node *rhs = parse_expr(ts);
    return new_binop(op, lhs ,rhs);
}

static Node *parse_if(TokenStream *ts){
    expect(ts, TOKEN_IF, "expected 'if'");
    parse_condition(ts);
    expect(ts, TOKEN_COLON, "expected ':' after condition");
    expect(ts, TOKEN_NEWLINE, "expected newline after ':'");

    parse_block(ts);

    if (peek(ts) -> type == TOKEN_ELSE){
        expect(ts, TOKEN_ELSE, "expected 'else' ");
        expect(ts, TOKEN_COLON, "expected ':' after else ");
        expect(ts, TOKEN_NEWLINE, " expected newline after else ':' ");
        parse_block(ts);
    }
    return NULL;
}

static Node *parse_while(TokenStream *ts){
    expect(ts, TOKEN_WHILE, "expected 'while' ");
    parse_condition(ts);
    expect(ts, TOKEN_COLON, "expected ':' after condition");
    expect(ts, TOKEN_NEWLINE, " expected newline after ':'");

    parse_block(ts);
    return NULL;
}



static Node* parse_stmt(TokenStream *ts){
    Token *t = peek(ts);

    switch (t->type){
        case TOKEN_IF:
            parse_if(ts);
            return parse_if(ts);

        case TOKEN_RETURN:
            expect(ts, TOKEN_RETURN, "expected 'return'");
            parse_expr(ts);
            return NULL;

        case TOKEN_PRINT:
            expect(ts, TOKEN_PRINT, "expected 'print'");
            parse_expr(ts);
            return NULL;

        case TOKEN_IDENT: {
            Token *id = advance(ts);
            expect(ts, TOKEN_EQ, "expected '='");
            Node *rhs = parse_expr(ts);
            Node *lhs = new_ident(id->text);

            return new_assign(lhs, rhs);
        }
        case TOKEN_WHILE:{
            expect(ts, TOKEN_WHILE, "expected 'while' ");
            Node *cond = parse_condition(ts);
            expect(ts, TOKEN_COLON, "expected ':' ");
            expect(ts, TOKEN_NEWLINE, "expected newline after ':'");
            Node *body = parse_block(ts);
            return new_while(cond, body);
        }

        default:
            fprintf(stderr, "Parser error: unexpected token %s\n", token_type_name(t->type));
            exit(1);
        // TODO: IDENT '=' 
    }

    while (peek(ts)->type == TOKEN_NEWLINE) advance(ts);
}

static Node *parse_block(TokenStream *ts){
    while (peek(ts)->type == TOKEN_NEWLINE) advance(ts);

    while (1){
        Token *t = peek(ts);

        if (t->type == TOKEN_ELSE || t->type == TOKEN_EOF) break;

        if (t->type == TOKEN_IF || t->type == TOKEN_WHILE ||
            t->type == TOKEN_RETURN || t->type == TOKEN_PRINT ||
            t->type == TOKEN_IDENT) {

            parse_stmt(ts);
            while (peek(ts)->type == TOKEN_NEWLINE) advance(ts);
        } else {
            break;
        }
        return NULL;
    }
    return NULL;
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