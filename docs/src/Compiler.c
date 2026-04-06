#include "Compiler.h"

void CompileBrian(int argc, char* argv[])
{
    CompilationState cs = ParseFlagsBrian(argc, argv);

    /* 
    Pass cs to each phase explicitly as well, that way
    upon an error occuring, errors can cleanup any remaining
    compilation artifcts. Free symbol tables, asts, etc. 
    
    Could potentially store this as a function pointer
    */
    RunTokenizer(cs);

    // .. Other phases get implemented 

    BrianCleanup: 

    // .. cleaup here, called by EXIT errors to allow cleanup
}


static CompilationState ParseFlagsBrian(int argc, char* argv[])
{
    CompilationState cs;

    if (argc <= 1) ERROR(ERR_FLAG_ABORT, COMPILER_ERR, "No filename specified\n");

    cs.fptr = fopen(argv[1], "r");

    if (!cs.fptr) ERROR(ERR_FLAG_ABORT, COMPILER_ERR, "Unable to open file '%s'\n", argv[1]);
    if (argc < 2) return (CompilationState){ NOT_APPLICABLE, "native" };

    for (size_t i = 2; i < argc; i++) {
        if (argv[i][0] != '-') ERROR(ERR_FLAG_ABORT, COMPILER_ERR, "BrIan compiler flag '%s' does not start with '-' ", argv[i]);

        if (strcmp(argv[i], "-tok") == 0) cs.flags.stopAfter = TOKENIZE;
    }

    return cs;
}

/* ----- Running Each Phase ------ */

void RunTokenizer(CompilationState cs)
{
    if (cs.flags.stopAfter == TOKENIZE) {
        TokenizerContext* ctx = InitalizeTokenizerContext(cs.fptr);
        Token tok;
        while ( (tok = GetNextToken(ctx)).type != ERR && tok.type != END)
            printf("Token: %s\tRow: %d\tCol:%d\tTokNum: %d\n", tok.lexeme, tok.row, tok.col, tok.type);
        DestroyTokenizerContext(ctx);
    }
}

void RunParser(CompilationState cs)
{

}

void RunPreprocessor(CompilationState cs)
{

}

void RunNameResolver(CompilationState cs)
{

}

void RunTypeChecker(CompilationState cs)
{

}

void RunLLVMGenerateIR(CompilationState cs)
{

}

void RunAssemble(CompilationState cs)
{

}