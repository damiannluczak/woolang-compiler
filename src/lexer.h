#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKEN_LEN 64 
#define MAX_TOKENS 1024

typedef enum { 
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_PRINT,
    TOKEN_RETURN,
    TOKEN_IDENT,
    TOKEN_NUMBER,

    TOKEN_NEWLINE,
    TOKEN_INDENT,
    TOKEN_DEDENT,
    
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_COLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SLASH,
    TOKEN_COMMA,
    TOKEN_DOT,

    TOKEN_EQ,
    TOKEN_EQEQ,
    
    TOKEN_LE, 
    TOKEN_LT,
    TOKEN_GE,
    TOKEN_GT,

    TOKEN_NEQ,
    
    TOKEN_UNKNOWN,
    TOKEN_ERROR,
    TOKEN_EOF
} TokenType;

typedef struct { 
    TokenType type;
    char text[MAX_TOKEN_LEN];
} Token;



void lexer(const char *path, Token *tokens, int *token_count);

#endif