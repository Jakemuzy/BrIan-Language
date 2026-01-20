#ifndef _COMPILER_H__
#define _COMPILER_H__

/*  TODO: 
        - Everything

    HIGH LEVEL:
        - Takes the AST and converts it to LLVM
            - Converts LLVM to assembly of choice
            - Converts assembly of choice to binary
*/

#include "Parser.h"

void ASTConvertToLLVM(AST* ast);

#endif 