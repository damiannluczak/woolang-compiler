#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
        case TOKEN_INDENT: return "INDENT";
        case TOKEN_DEDENT: return "DEDENT";
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
        case TOKEN_FN: return "FN";
        case TOKEN_TRUE: return "TRUE";
        case TOKEN_FALSE: return "FALSE";
        case TOKEN_PERCENT: return "PERCENT";
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

static Node *parse_fn(TokenStream *ts);

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
static Token *peek_next(TokenStream *ts){
    if (ts->pos + 1 >= ts->count) return &EOF_TOKEN;
    return &ts->tokens[ts->pos + 1];
}
static void skip_newlines(TokenStream *ts) {
    while (peek(ts)->type == TOKEN_NEWLINE) advance(ts);
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

    // true/false jako 1/0 (MVP bool)
    if (t->type == TOKEN_TRUE){
        advance(ts);
        return new_literal(1);
    }
    if (t->type == TOKEN_FALSE){
        advance(ts);
        return new_literal(0);
    }

    // IDENT albo CALL
    if (t->type == TOKEN_IDENT){
        Token *id = advance(ts);

        // call: ident(...)
        if (peek(ts)->type == TOKEN_LPAREN) {
            advance(ts); // '('

            Node *first_arg = NULL;
            Node *last_arg  = NULL;

            if (peek(ts)->type != TOKEN_RPAREN) {
                while (1) {
                    Node *arg = parse_expr(ts);

                    if (!first_arg) first_arg = arg;
                    else last_arg->next = arg;
                    last_arg = arg;

                    if (peek(ts)->type == TOKEN_COMMA) {
                        advance(ts);
                        continue;
                    }
                    break;
                }
            }

            expect(ts, TOKEN_RPAREN, "expected ')' after call args");
            return new_call(id->text, first_arg); // UWAGA: musisz mieć NODE_CALL + new_call()
        }

        return new_ident(id->text);
    }

    if (t->type == TOKEN_NUMBER){
        int value = atoi(t->text);
        advance(ts);
        return new_literal(value);
    }

    if (t->type == TOKEN_LPAREN){
        advance(ts);
        Node *expr = parse_expr(ts);
        expect(ts, TOKEN_RPAREN, "expected ')' after expression");
        return expr;
    }
    if (t->type == TOKEN_TRUE) {
    advance(ts);
    return new_literal(1);
    }
    if (t->type == TOKEN_FALSE) {
        advance(ts);
        return new_literal(0);
    }

    fprintf(stderr, "Syntax error: expected IDENT/NUMBER/true/false/(...) \n");
    fprintf(stderr, "Got token: %s\n", token_type_name(t->type));
    exit(1);
}
static Node *parse_term(TokenStream *ts){
    Node *left = parse_atom(ts);

    while (1) {
        TokenType op = peek(ts)->type;

        if (op == TOKEN_SLASH || op == TOKEN_PERCENT) {
            advance(ts);
            Node *right = parse_atom(ts);
            left = new_binop(op, left, right);
        } else {
            break;
        }
    }

    return left;
}


static Node *parse_add(TokenStream *ts){
    Node *left = parse_term(ts);

    while (1){
        TokenType op = peek(ts)->type;

        if (op == TOKEN_PLUS || op == TOKEN_MINUS){
            advance(ts);
            Node *right = parse_term(ts);
            left = new_binop(op, left, right);
        } else{
            break;
        }
    }
    return left;
}
static Node *parse_expr(TokenStream *ts){
    Node *left = parse_add(ts);

    TokenType op = peek(ts)->type;
    if (op == TOKEN_LT || op == TOKEN_LE ||
        op == TOKEN_GT || op == TOKEN_GE ||
        op == TOKEN_EQEQ || op == TOKEN_NEQ) {

        advance(ts);
        Node *right = parse_add(ts);
        left = new_binop(op, left, right);
    }

    return left;
}

static Node *parse_condition(TokenStream *ts){
    return parse_expr(ts);
}

static Node *parse_if(TokenStream *ts){

    expect(ts, TOKEN_IF, "expected 'if'");

    Node *cond = parse_condition(ts);

    expect(ts, TOKEN_COLON, "expected ':' after condition");
    expect(ts, TOKEN_NEWLINE, "expected newline after ':'");

    Node *then_body = parse_block(ts);

    while (peek(ts)->type == TOKEN_NEWLINE) advance(ts);

    Node *else_body = NULL;

    if (peek(ts) -> type == TOKEN_ELSE){
        expect(ts, TOKEN_ELSE, "expected 'else' ");
        expect(ts, TOKEN_COLON, "expected ':' after else ");
        expect(ts, TOKEN_NEWLINE, " expected newline after else ':' ");

        else_body = parse_block(ts);
    }
    return new_if(cond, then_body,else_body);
}

static Node *parse_while(TokenStream *ts){
    expect(ts, TOKEN_WHILE, "expected 'while' ");
    Node *cond = parse_condition(ts);
    expect(ts, TOKEN_COLON, "expected ':' after condition");
    expect(ts, TOKEN_NEWLINE, " expected newline after ':'");

    Node *body = parse_block(ts);
    return new_while(cond,body);
}

static Node *parse_fn(TokenStream *ts){
    expect(ts, TOKEN_FN, "expected 'fn'");

    Token *name = expect(ts, TOKEN_IDENT, "expected function name after 'fn'");
    expect(ts, TOKEN_LPAREN, "expected '(' after function name");

    // params: IDENT (, IDENT)*
    char params[32][64];
    int param_count = 0;

    if (peek(ts)->type != TOKEN_RPAREN) {
        while (1) {
            Token *p = expect(ts, TOKEN_IDENT, "expected param name");
            strncpy(params[param_count], p->text, 63);
            params[param_count][63] = '\0';
            param_count++;

            if (peek(ts)->type == TOKEN_COMMA) {
                advance(ts);
                continue;
            }
            break;
        }
    }

    expect(ts, TOKEN_RPAREN, "expected ')'");
    expect(ts, TOKEN_COLON, "expected ':' after fn signature");
    expect(ts, TOKEN_NEWLINE, "expected newline after ':'");

    Node *body = parse_block(ts);

    return new_funcdef(name->text, params, param_count, body); // musisz mieć NODE_FUNCDEF + new_funcdef()
}



static Node* parse_stmt(TokenStream *ts){
    Token *t = peek(ts);

    switch (t->type){
        case TOKEN_FN:
            return parse_fn(ts);
        case TOKEN_IF:
            return parse_if(ts);

        case TOKEN_RETURN: {
            expect(ts, TOKEN_RETURN, "expected 'return'");
            Node *expr = parse_expr(ts);
            return new_return(expr);
        }

        case TOKEN_PRINT: {
            expect(ts, TOKEN_PRINT, "expected 'print'");
            Node *expr = parse_expr(ts);
            return new_print(expr);
        }

        case TOKEN_IDENT: {
        // sprawdzczy to assignment
            if (peek_next(ts)->type != TOKEN_EQ) {
        // zwykłe wyrazenie
            return parse_expr(ts);
            }

            Token *id = advance(ts);
            expect(ts, TOKEN_EQ, "expected '='");
            Node *rhs = parse_expr(ts);
            Node *lhs = new_ident(id->text);

            return new_assign(lhs, rhs);
        }

        case TOKEN_WHILE: {
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
    }

    while (peek(ts)->type == TOKEN_NEWLINE) advance(ts);
}

static int is_stmt_start(TokenType tt) {
    return (tt == TOKEN_IF || tt == TOKEN_WHILE ||
            tt == TOKEN_RETURN || tt == TOKEN_PRINT ||
            tt == TOKEN_IDENT || tt == TOKEN_FN);
}

static Node *parse_block(TokenStream *ts){
    Node *first = NULL;
    Node *last  = NULL;

    skip_newlines(ts);

    expect(ts, TOKEN_INDENT, "expected INDENT to start block");
    skip_newlines(ts);

    while (1) {
        TokenType tt = peek(ts)->type;

        // koniec bloku
        if (tt == TOKEN_DEDENT) break;
        if (tt == TOKEN_EOF) {
            fprintf(stderr, "Syntax error: unexpected EOF inside block (missing DEDENT)\n");
            exit(1);
        }

        // puste linie wewnątrz bloku
        if (tt == TOKEN_NEWLINE) {
            advance(ts);
            continue;
        }

        if (!is_stmt_start(tt)) {
            fprintf(stderr, "Parser error: unexpected token %s inside block\n", token_type_name(tt));
            exit(1);
        }

        Node *stmt = parse_stmt(ts);

        if (!first) first = stmt;
        else last->next = stmt;
        last = stmt;

        // po statement mogą być newliny
        skip_newlines(ts);
    }

    expect(ts, TOKEN_DEDENT, "expected DEDENT to end block");
    return new_block(first);
}

Node *parse_program(TokenStream *ts){
    Node *first = NULL;
    Node *last = NULL;

    while (peek(ts)->type == TOKEN_NEWLINE || peek(ts)->type == TOKEN_DEDENT) {
    advance(ts);
}

    while (peek(ts)->type != TOKEN_EOF) {
        Node * stmt = parse_stmt(ts);

        if (!first) first = stmt; else last->next = stmt;

        last = stmt;

        while (peek(ts)->type == TOKEN_NEWLINE || peek(ts)->type == TOKEN_DEDENT) {
    advance(ts);
}
    }
    return new_block(first);
}