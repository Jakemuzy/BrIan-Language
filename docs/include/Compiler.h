#ifndef _BRIAN_COMPILER_H_
#define _BRIAN_COMPILER_H_

#define DEBUG
#ifdef DEBUG
    #include "DebugTools.h"
#endif

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

typedef struct CompilerFlags {
    /* Debug Flags */
    enum { TOKENIZE, PARSE, PREPROCESS, NAMERES, TYPECHECK, IR, ASSEMBLE, NOT_APPLICABLE } stopAfter;
    
    /* LLVM Flags */
    char* target;

    /* Arena size */
    size_t arena;
} CompilerFlags;

typedef struct CompilationState {
    CompilerFlags flags;

    FILE* fptr;
    size_t fileSize; /* In bytes. Useful for estimating arena size */

    AST* ast;

    TokenizerContext* tokenizer;
    ParserContext* parser;

    /* TEMPORARY */
    void* preprocessor, *nameres, *typecheck, *irgen, *assembler;
} CompilationState;

/* Handle Flags, and Cleanup */
void CompileBrian(int argc, char* argv[]);
void CleanupBrian(CompilationState* cs);
static CompilationState* ParseFlagsBrian(int argc, char* argv[]);

void OpenFile(CompilationState* cs, int argc, char* argv[]);

/* ----- Running Each Phase ------ */

void RunTokenizer(CompilationState* cs);
void RunParser(CompilationState* cs);
void RunPreprocessor(CompilationState* cs);
void RunNameResolver(CompilationState* cs);
void RunTypeChecker(CompilationState* cs);
void RunLLVMGenerateIR(CompilationState* cs);
void RunAssemble(CompilationState* cs);

#endif
