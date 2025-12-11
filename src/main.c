#include <stdio.h>
#include <ctype.h>
#include <lexer.h>


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

int main(int argc, char ** argv){
    if (argc < 2) {
        fprintf(stderr, "Uzycie: %s <sciezka_do_pliku\n", argv[0]);
        return 1;
    }

    lexer(argv[1]);
    
    return 0;
}