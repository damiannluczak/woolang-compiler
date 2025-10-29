#include <ctype.h>
#include <stdio.h>
#define LEXER_H

void lexer(void){
    FILE *test;
    test = fopen("samples/test.txt", "r");

    if (test == NULL)
    {
        perror("fopen");
        return;
    }

    int ch;
    int alpha = 0, digit = 0, punct = 0, space = 0;
    
    //for colecting indent // keyword 
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
                if (isalpha(ch)) alpha++;
                if (i < (int)sizeof(res) - 1) res[i++] = (char)ch;
                continue;
            } else { 
            if (i > 0)
            {
                res[i] = '\0';
                printf("IDENT(%s)\n", res);
                i = 0;
                ungetc(ch, test);
                continue;
            }
            else if (isdigit(ch))
            {   
                digit ++;

                num[0] = (char)ch;
                n = 1;

                while ((ch = fgetc(test)) != EOF  && isdigit(ch)){
                    if ((int)n < sizeof(num) - 1) num[n ++] = (char)ch;
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
                printf("SPACE: %c\n", ch);
                space += 1;
            }
            else if (ispunct(ch))
            {
                printf("PUNCTUATION: %c\n", ch);
                punct += 1;
            }
        }
    }
    if (i > 0)
    {
        res[i] = '\0';
        printf("IDENT(%s)\n", res);
    }
    printf("\n");
    printf("\nA:%d D:%d S:%d P:%d\n", alpha, digit, space, punct);
    printf("\n");

    fclose(test);
}


