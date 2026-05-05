/* Can't test pass/fail since can't fail, have to do visual insepction */

#include <stdio.h>
#include <stdlib.h>

#include "TypeChecker.h"
#include "Desugar.h"
#include "DEBUG.h"

int main()
{
    /* Open code file to read */
    FILE* fptr;
    fptr = fopen("TestCases/Desugarizer/DesugarTest.b", "r");
    if(!fptr) {
        printf("ERROR: Opening source file\n");
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

    TYPE* typeCheck = TypeCheck(nss, ast->root);
    if (typeCheck == TY_ERROR()) {
        printf("ERROR: Typechecking failed\n");
        exit(1);
    }

    PrintAST(ast);
    printf("\n\n\n");
    ast = Desugar(ast);
    PrintAST(ast);
    return 0;
}    
