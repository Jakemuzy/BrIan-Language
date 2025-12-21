#include <stdio.h>

#include "Parser.h"


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
    AST* ast = ConstructAST(fptr);
    Program(fptr, ast); 
	return 0;
}
