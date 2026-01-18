// src/lexer.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static void push_token(Token *tokens, int *token_count, TokenType type, const char *text) {
    if (*token_count >= MAX_TOKENS) return;

    tokens[*token_count].type = type;
    strncpy(tokens[*token_count].text, text, MAX_TOKEN_LEN - 1);
    tokens[*token_count].text[MAX_TOKEN_LEN - 1] = '\0';
    (*token_count)++;
}

static void lex_error(const char *msg) {
    fprintf(stderr, "Lexer error: %s\n", msg);
    exit(1);
}

static void flush_ident(Token *tokens, int *token_count, char *buf, int *len) {
    if (*len <= 0) return;

    buf[*len] = '\0';

    if (strcmp(buf, "if") == 0) push_token(tokens, token_count, TOKEN_IF, "");
    else if (strcmp(buf, "else") == 0) push_token(tokens, token_count, TOKEN_ELSE, "");
    else if (strcmp(buf, "while") == 0) push_token(tokens, token_count, TOKEN_WHILE, "");
    else if (strcmp(buf, "print") == 0) push_token(tokens, token_count, TOKEN_PRINT, "");
    else if (strcmp(buf, "return") == 0) push_token(tokens, token_count, TOKEN_RETURN, "");
    else push_token(tokens, token_count, TOKEN_IDENT, buf);

    *len = 0;
}

void lexer(const char *path, Token *tokens, int *token_count) {
    FILE *f = fopen(path, "r");
    if (!f) {
        perror("fopen");
        exit(1);
    }

    // --- INDENT/DEDENT ---
    int indent_stack[128];
    int indent_top = 0;
    indent_stack[0] = 0;
    int at_line_start = 1;

    // --- buffers ---
    char ident[64];
    int i = 0;

    char num[64];
    int n = 0;

    while (1) {
        int ch = fgetc(f);

        // --- EOF ---
        if (ch == EOF) {
            // domknij ewentualny ident
            flush_ident(tokens, token_count, ident, &i);

            // domknij wszystkie wcięcia
            while (indent_top > 0) {
                push_token(tokens, token_count, TOKEN_DEDENT, "");
                indent_top--;
            }

            push_token(tokens, token_count, TOKEN_EOF, "");
            break;
        }

        // --- obsługa indentacji tylko na początku linii ---
        if (at_line_start) {
            int spaces = 0;

            while (ch == ' ') {
                spaces++;
                ch = fgetc(f);
                if (ch == EOF) break;
            }

            // pusta linia (same spacje + newline)
            if (ch == '\n') {
                push_token(tokens, token_count, TOKEN_NEWLINE, "\\n");
                at_line_start = 1;
                continue;
            }

            // EOF po spacjach
            if (ch == EOF) {
                flush_ident(tokens, token_count, ident, &i);

                while (indent_top > 0) {
                    push_token(tokens, token_count, TOKEN_DEDENT, "");
                    indent_top--;
                }
                push_token(tokens, token_count, TOKEN_EOF, "");
                break;
            }

            // wymagamy wielokrotności 4
            if (spaces % 4 != 0) lex_error("Indentation must be multiple of 4 spaces");

            int new_indent = spaces;
            int cur_indent = indent_stack[indent_top];

            // indent tylko +4
            if (new_indent > cur_indent) {
                if (new_indent != cur_indent + 4)
                    lex_error("Indentation jump must be exactly +4 spaces");

                indent_top++;
                indent_stack[indent_top] = new_indent;
                push_token(tokens, token_count, TOKEN_INDENT, "");
            }
            // dedent: emituj aż do poziomu
            else if (new_indent < cur_indent) {
                while (indent_top > 0 && new_indent < indent_stack[indent_top]) {
                    push_token(tokens, token_count, TOKEN_DEDENT, "");
                    indent_top--;
                }
                if (new_indent != indent_stack[indent_top])
                    lex_error("Dedent does not match any previous indentation level");
            }

            at_line_start = 0;
            // ten znak jest pierwszym "normalnym" w linii — przetwarzamy go dalej (bez continue)
        }

        // --- zbieranie identów: litera/_ start, potem litera/_/cyfra ---
        if ((isalpha(ch) || ch == '_') || (isdigit(ch) && i > 0)) {
            if (i < (int)sizeof(ident) - 1) ident[i++] = (char)ch;
            continue;
        }

        // --- jeśli ident się zakończył, flush i wróć znak do strumienia ---
        if (i > 0) {
            flush_ident(tokens, token_count, ident, &i);
            ungetc(ch, f);
            continue;
        }

        // --- liczby ---
        if (isdigit(ch)) {
            num[0] = (char)ch;
            n = 1;

            while ((ch = fgetc(f)) != EOF && isdigit(ch)) {
                if (n < (int)sizeof(num) - 1) num[n++] = (char)ch;
            }

            num[n] = '\0';
            push_token(tokens, token_count, TOKEN_NUMBER, num);

            if (ch != EOF) ungetc(ch, f);
            continue;
        }

        // --- whitespace ---
        if (isspace(ch)) {
            if (ch == '\n') {
                push_token(tokens, token_count, TOKEN_NEWLINE, "\\n");
                at_line_start = 1;
            }
            continue;
        }

        // --- operatory/punktuacja ---
        switch (ch) {
            case '+': push_token(tokens, token_count, TOKEN_PLUS, "+"); break;
            case '-': push_token(tokens, token_count, TOKEN_MINUS, "-"); break;
            case ':': push_token(tokens, token_count, TOKEN_COLON, ":"); break;
            case '(': push_token(tokens, token_count, TOKEN_LPAREN, "("); break;
            case ')': push_token(tokens, token_count, TOKEN_RPAREN, ")"); break;
            case '[': push_token(tokens, token_count, TOKEN_LBRACKET, "["); break;
            case ']': push_token(tokens, token_count, TOKEN_RBRACKET, "]"); break;
            case '{': push_token(tokens, token_count, TOKEN_LBRACE, "{"); break;
            case '}': push_token(tokens, token_count, TOKEN_RBRACE, "}"); break;
            case '/': push_token(tokens, token_count, TOKEN_SLASH, "/"); break;
            case ',': push_token(tokens, token_count, TOKEN_COMMA, ","); break;
            case '.': push_token(tokens, token_count, TOKEN_DOT, "."); break;

            case '=': {
                int next = fgetc(f);
                if (next == '=') push_token(tokens, token_count, TOKEN_EQEQ, "==");
                else { if (next != EOF) ungetc(next, f); push_token(tokens, token_count, TOKEN_EQ, "="); }
                break;
            }

            case '<': {
                int next = fgetc(f);
                if (next == '=') push_token(tokens, token_count, TOKEN_LE, "<=");
                else { if (next != EOF) ungetc(next, f); push_token(tokens, token_count, TOKEN_LT, "<"); }
                break;
            }

            case '>': {
                int next = fgetc(f);
                if (next == '=') push_token(tokens, token_count, TOKEN_GE, ">=");
                else { if (next != EOF) ungetc(next, f); push_token(tokens, token_count, TOKEN_GT, ">"); }
                break;
            }

            case '!': {
                int next = fgetc(f);
                if (next == '=') push_token(tokens, token_count, TOKEN_NEQ, "!=");
                else lex_error("Unexpected '!'");
                break;
            }

            default: {
                char tmp[2] = {(char)ch, '\0'};
                push_token(tokens, token_count, TOKEN_UNKNOWN, tmp);
                break;
            }
        }
    }

    fclose(f);
}