#include <stdio.h>

#include "NameResolver.h"

int main(int argc, char* argv[])
{
    /* Open code file to read */
    FILE* fptr;
    fptr = fopen(argv[1], "r");
    if(!fptr)
    {
        printf("ERROR: Opening source file %s\n", argv[1]);
        return 1;
    }

    /* Build the ast */
    AST* ast = Program(fptr); 
    if (!ast) {
        printf("ERROR: ast failed to build\n");
        exit(1);
    }

    Namespaces* nss = ResolveNames(ast);
    if (!nss) {
        printf("ERROR: Namespaces failed to build\n");
        exit(1);
    }


	return 0;
}    