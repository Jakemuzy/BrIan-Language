#include <stdio.h>
#include <stdlib.h>

#include "NameResolver.h"

typedef struct {
    const char* filename;
    bool shouldPass;
} TestCase;

int main(int argc, char* argv[])
{
    TestCase tests[] = {
        {"TestCases/NameResolver/AllCases.b", true},
        {"TestCases/NameResolver/ForBodyInit.b", true},
        {"TestCases/NameResolver/ForEscape.b", false},
        {"TestCases/NameResolver/ForScope.b", true},
        {"TestCases/NameResolver/ForShadow.b", false},
        {"TestCases/NameResolver/FuncLeak.b", false},
        {"TestCases/NameResolver/FuncShadow.b", true},
        {"TestCases/NameResolver/FuncUndefined.b", false},
        {"TestCases/NameResolver/IfNest.b", true},
        {"TestCases/NameResolver/IfNestDouble.b", true},
        {"TestCases/NameResolver/IfNestShadow.b", false},
        {"TestCases/NameResolver/IfScope.b", true},
        {"TestCases/NameResolver/IfShadow.b", true},
        {"TestCases/NameResolver/MultiVar.b", true},
        {"TestCases/NameResolver/ParamShadow.b", true},
        {"TestCases/NameResolver/Recursion.b", true},
        {"TestCases/NameResolver/Redeclaration.b", false},
        {"TestCases/NameResolver/Resolved.b", true},
        {"TestCases/NameResolver/Undefined.b", false},
        {"TestCases/NameResolver/VarCall.b", true},
        {"TestCases/NameResolver/VarEscape.b", false},
        {"TestCases/NameResolver/VarGlob.b", true},
        {"TestCases/NameResolver/VarInvalidScope.b", false},
        {"TestCases/NameResolver/VarShadow.b", true}
    };

    int expectedPass = -1;
    size_t numTests = sizeof(tests) / sizeof(tests[0]);
    for (size_t i = 0; i < numTests; i++) {
        if (strcmp(tests[i].filename, argv[1]) == 0) {
            expectedPass = tests[i].shouldPass;
            break;
        }
    }

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

    if (!ast) {
        printf("ERROR: AST failed to build for %s\n", argv[1]);
        return expectedPass ? 1 : 0;  
    }

    Namespaces* nss = ResolveNames(ast);
    int resultPass = (nss != NULL) ? 1 : 0;

    return (resultPass == expectedPass) ? 0 : 1;
}    