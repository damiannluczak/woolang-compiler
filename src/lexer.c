#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"

void lexer(const char *path)
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

    printf("Plik zawiera: \n");

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
                    printf("KEYWORD(IF)\n");
                else if (strcmp(res, "else") == 0)
                    printf("KEYWORD(ELSE)\n");
                else if (strcmp(res, "while") == 0)
                    printf("KEYWORD(WHILE)\n");
                else if (strcmp(res, "print") == 0)
                    printf("KEYWORD(PRINT)\n");
                else if (strcmp(res, "return") == 0)
                    printf("KEYWORD(RETURN)\n");
                else
                    printf("IDENT(%s)\n", res);

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
                printf("NUMBER(%s)\n", num);

                if (ch != EOF)
                    ungetc(ch, test);
                continue;
            }
            else if (isspace(ch))
            {
                if (ch == '\n')
                {
                    printf("NEWLINE\n");
                    continue;
                }
                continue;
            }
            else if (ispunct(ch))
            {
                switch (ch)
                {
                case '+':
                    printf("PLUS: %c\n", ch);
                    break;
                case ':':
                    printf("COLON: %c\n",ch);
                    break;
                case '-':
                    printf("MINUS: %c\n", ch);
                    break;
                case '(':
                    printf("LPAREN: %c\n", ch);
                    break;
                case ')':
                    printf("RPAREN: %c\n", ch);
                    break;
                case '[':
                    printf("LBRACKET: %c\n", ch);
                    break;
                case ']':
                    printf("RBRACKET: %c\n", ch);
                    break;
                case '{':
                    printf("LBRACE: %c\n", ch);
                    break;
                case '}':
                    printf("RBRACE: %c\n", ch);
                    break;
                case '/':
                    printf("SLASH: %c\n", ch);
                    break;
                case ',':
                    printf("COMMA: %c\n", ch);
                    break;
                case '.':
                    printf("DOT: %c\n", ch);
                    break;
                // arthmetic operators 
                case '=':
                {
                    int next = fgetc(test);
                    if (next == '=')
                    {
                        printf("EQEQ\n");
                    }
                    else
                    {
                        ungetc(next, test);
                        printf("EQ\n");
                    }
                    break;
                }
                case '<':
                {
                    int next = fgetc(test);
                    if (next == '=')
                    {
                        printf("LE\n");
                    }
                    else
                    {
                        ungetc(next, test);
                        printf("LT\n");
                    }
                    break;
                }
                case '>':
                {
                    int next = fgetc(test);
                    if (next == '=')
                    {
                        printf("GE\n");
                    }
                    else
                    {
                        ungetc(next, test);
                        printf("GT\n");
                    }
                    break;
                }
                case '!':
                {
                    int next = fgetc(test);
                    if (next == '=')
                    {
                        printf("NEQ\n");
                    }
                    break; 
                }

                default:
                    printf("PUNCTUATION: %c\n", ch);
                }
                punct += 1;
            }
        }
    }
    if (i > 0)
    {
        res[i] = '\0';

        if (strcmp(res, "if") == 0)
            printf("KEYWORD(IF)\n");
        else if (strcmp(res, "else") == 0)
            printf("KEYWORD(ELSE)\n");
        else if (strcmp(res, "while") == 0)
            printf("KEYWORD(WHILE)\n");
        else if (strcmp(res, "print") == 0)
            printf("KEYWORD(PRINT)\n");
        else if (strcmp(res, "return") == 0)
            printf("KEYWORD(RETURN)\n");
        else
            printf("IDENT(%s)\n", res);
    }
    printf("\n");
    printf("\nA:%d D:%d S:%d P:%d\n", alpha, digit, space, punct);
    printf("\n");

    fclose(test);
    
}
