#ifndef _BRIAN_COMPILER_H_
#define _BRIAN_COMPILER_H_

#include "ErrorHandler.h"

#include "Tokenizer.h"
#include "Parser.h"

/*      BrIan Compiler Entry
    ----------------------------
    Entry point and fatal error 
           cleanup logic.
     Refer to docs/pipeline.md 
      for details and flags.   

*/

typedef struct {
    /* Debug Flags */
    enum { TOKENIZE, PARSE, PREPROCESS, NAMERES, TYPECHECK, IR, ASSEMBLE, NOT_APPLICABLE } stopAfter;
    
    /* LLVM Flags */
    char* target;
} CompilerFlags;

typedef struct {
    CompilerFlags flags;

    /* 
    Store actual state as well like the ast, 
    or the symbol table, for passing down to 
    next pipeline stages
    */
    FILE* fptr;
} CompilationState;

/* Handle Flags, and Cleanup */
void CompileBrian(int argc, char* argv[]);
static CompilationState ParseFlagsBrian(int argc, char* argv[]);

/* ----- Running Each Phase ------ */

void RunTokenizer(CompilationState cs);
void RunParser(CompilationState cs);
void RunPreprocessor(CompilationState cs);
void RunNameResolver(CompilationState cs);
void RunTypeChecker(CompilationState cs);
void RunLLVMGenerateIR(CompilationState cs);
void RunAssemble(CompilationState cs);

#endif
