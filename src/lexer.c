#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"

static void push_token(
    Token * tokens,
    int * token_count,
    TokenType type,
    const char *text
) {
    if (*token_count >= MAX_TOKENS) return;

    tokens[*token_count].type = type;
    strncpy(tokens[*token_count].text, text, MAX_TOKEN_LEN - 1);
    tokens[*token_count].text[MAX_TOKEN_LEN - 1] = '\0';
    (*token_count) ++;
}
const char *token_type_name(TokenType t);

void lexer(const char *path, Token *tokens, int *token_count)
{
    FILE *test;
    test = fopen(path, "r");

    if (test == NULL)
    {
        perror("fopen");
        return;
    }

    int ch;
    int alpha = 0, digit = 0, punct = 0, space = 0;

    // for colecting indent // keyword
    char res[50];
    int i = 0;

    // for colecting numbers
    char num[50];
    int n = 0;

   // printf("Plik zawiera: \n");

    while (1)
    {
        ch = fgetc(test);
        if (ch == EOF)
        {
            break;
        }
        if ((isalpha(ch) || ch == '_') || (isdigit(ch) && i > 0))
        {
            if (isalpha(ch))
                alpha++;
            if (i < (int)sizeof(res) - 1)
                res[i++] = (char)ch;
            continue;
        }
        else
        {
            if (i > 0)
            {
                res[i] = '\0';

                if (strcmp(res, "if") == 0)
                    push_token(tokens, token_count, TOKEN_IF, "");
                else if (strcmp(res, "else") == 0)
                    push_token(tokens, token_count, TOKEN_ELSE, "");
                else if (strcmp(res, "while") == 0)
                    push_token(tokens, token_count, TOKEN_WHILE, "");
                else if (strcmp(res, "print") == 0)
                    push_token(tokens, token_count, TOKEN_PRINT, "");
                else if (strcmp(res, "return") == 0)
                    push_token(tokens, token_count, TOKEN_RETURN, "");
                else { 
                    push_token(tokens, token_count, TOKEN_IDENT, res); 
                }
    

                i = 0;
                ungetc(ch, test);
                continue;
            }
            else if (isdigit(ch))
            {
                digit++;

                num[0] = (char)ch;
                n = 1;

                while ((ch = fgetc(test)) != EOF && isdigit(ch))
                {
                    if ((int)n < sizeof(num) - 1)
                        num[n++] = (char)ch;
                }
                num[n] = '\0';
                push_token(tokens, token_count, TOKEN_NUMBER, num);

                if (ch != EOF)
                    ungetc(ch, test);
                continue;
            }
            else if (isspace(ch))
            {
                if (ch == '\n')
                {
                    push_token(tokens, token_count, TOKEN_NEWLINE, "\\n");
                    continue;
                }
                continue;
            }
            else if (ispunct(ch))
            {
                switch (ch)
                {
                case '+':
                    push_token(tokens, token_count, TOKEN_PLUS, "+");
                    break;
                case ':':
                    push_token(tokens, token_count, TOKEN_COLON, ":");
                    break;
                case '-':
                    push_token(tokens, token_count, TOKEN_MINUS, "-");
                    break;
                case '(':
                    push_token(tokens, token_count, TOKEN_LPAREN, "(");
                    break;
                case ')':
                    push_token(tokens, token_count, TOKEN_RPAREN, ")");
                    break;
                case '[':
                    push_token(tokens, token_count, TOKEN_LBRACKET, "[");
                    break;
                case ']':
                    push_token(tokens, token_count, TOKEN_RBRACKET, "]");
                    break;
                case '{':
                    push_token(tokens, token_count, TOKEN_LBRACE, "{");
                    break;
                case '}':
                    push_token(tokens, token_count, TOKEN_RBRACE, "}");
                    break;
                case '/':
                    push_token(tokens, token_count, TOKEN_SLASH, "/");
                    break;
                case ',':
                    push_token(tokens, token_count, TOKEN_COMMA, ",");
                    break;
                case '.':
                    push_token(tokens, token_count, TOKEN_DOT, ".");
                    break;
                // arthmetic operators 
                case '=':
                {
                    int next = fgetc(test);
                    if (next == '=')
                    {
                        push_token(tokens, token_count, TOKEN_EQEQ, "==");
                    }
                    else
                    {
                        ungetc(next, test);
                        push_token(tokens, token_count, TOKEN_EQ, "=");
                    }
                    break;
                }
                case '<':
                {
                    int next = fgetc(test);
                    if (next == '=')
                    {
                        push_token(tokens, token_count, TOKEN_LE, "<=");
                    }
                    else
                    {
                        ungetc(next, test);
                        push_token(tokens, token_count, TOKEN_LT, "<");
                    }
                    break;
                }
                case '>':
                {
                    int next = fgetc(test);
                    if (next == '=')
                    {
                        push_token(tokens, token_count, TOKEN_GE, ">=");
                    }
                    else
                    {
                        ungetc(next, test);
                        push_token(tokens, token_count, TOKEN_GT, ">");
                    }
                    break;
                }
                case '!':
                {
                    int next = fgetc(test);
                    if (next == '=')
                    {
                        push_token(tokens, token_count, TOKEN_NEQ, "!=");
                    }
                    break; 
                }

                default: {
                    char tmp[2] = {(char)ch, '\0'};
                    push_token(tokens, token_count, TOKEN_UNKNOWN, tmp);
                    break;
                }
                }
                punct += 1;
            }
        }
    }
    if (i > 0)
    {
        res[i] = '\0';

        if (strcmp(res, "if") == 0)
            push_token(tokens, token_count, TOKEN_IF, "");
        else if (strcmp(res, "else") == 0)
            push_token(tokens, token_count, TOKEN_ELSE, "");
        else if (strcmp(res, "while") == 0)
            push_token(tokens, token_count, TOKEN_WHILE, "");
        else if (strcmp(res, "print") == 0)
            push_token(tokens, token_count, TOKEN_PRINT, "");
        else if (strcmp(res, "return") == 0)
             push_token(tokens, token_count, TOKEN_RETURN, "");
        else
            push_token(tokens, token_count, TOKEN_IDENT, res);
    }

   /* printf("\n");
    printf("\nA:%d D:%d S:%d P:%d\n", alpha, digit, space, punct);
    printf("\n");
    */

     push_token(tokens, token_count, TOKEN_EOF, "");

    fclose(test);
    
}
