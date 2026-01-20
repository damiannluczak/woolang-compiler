#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "ast.h"

// generacje programu out 
void gen_program(Node *root, FILE *out);

#endif