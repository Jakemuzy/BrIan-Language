#include "Compiler.h"


/* 

Tight coupling may be happening between compiler state 
and all the phases of the pipeline. This can maybe be
solved by simply passing the information each phase needs,
rather than the whole CompilationState. This is more ideal, 
since it prevents circular dependencies and allows for each
phase to own their own context, rather than "borrowing", the 
god file of compiler state.

*/

void CompileBrian(int argc, char* argv[])
{
    CompilationState* cs = ParseFlagsBrian(argc, argv);

    RunTokenizer(cs);
    if (cs->flags.stopAfter == TOKENIZE) { 
        CleanupBrian(cs); return; 
    }

    RunParser(cs);
    if (cs->flags.stopAfter == PARSE) { 
        DEBUG_PRINT_AST(cs->parser->ast); 
        CleanupBrian(cs); return; 
    }
    // .. Other phases get implemented 

    CleanupBrian(cs);
}

void CleanupBrian(CompilationState* cs)
{
    if (cs->tokenizer) DestroyTokenizerContext(cs->tokenizer);
    if (cs->parser) DestroyParserContext(cs->parser);
}

static CompilationState* ParseFlagsBrian(int argc, char* argv[])
{
    CompilationState* cs = malloc(sizeof(CompilationState));
    OpenFile(cs, argc, argv);

    /* Tokenizer */
    for (int i = 2; i < argc; i++) {
        if (argv[i][0] != '-') ERROR(ERR_FLAG_ABORT, COMPILER_ERR, "BrIan compiler flag '%s' does not start with '-' ", argv[i]);

        if (strcmp(argv[i], "-tok") == 0) cs->flags.stopAfter = TOKENIZE;
        else if (strcmp(argv[i], "-parse") == 0) cs->flags.stopAfter = PARSE;
        else if (strcmp(argv[i], "-pre") == 0) cs->flags.stopAfter = PREPROCESS;
        else if (strcmp(argv[i], "-name") == 0) cs->flags.stopAfter = NAMERES;
        else if (strcmp(argv[i], "-type") == 0) cs->flags.stopAfter = TYPECHECK;
        else if (strcmp(argv[i], "-ir") == 0) cs->flags.stopAfter = IR;
        else if (strcmp(argv[i], "-assemble") == 0) cs->flags.stopAfter = ASSEMBLE;

        /* Contains arena and target */
    }

    return cs;
}

void OpenFile(CompilationState* cs, int argc, char* argv[]) 
{
    if (argc <= 1) ERROR(ERR_FLAG_EXIT, COMPILER_ERR, "No filename specified\n");

    /* Filesize */
    FILE *fp = fopen(argv[1], "rb"); 
    if (!fp) ERROR(ERR_FLAG_EXIT, COMPILER_ERR, "Unable to open file '%s'\n", argv[1]);
    /* "native" target should be default */

    /* Store file pointer */
    fseek(fp, 0L, SEEK_END);      
    long fsize = ftell(fp);         
    rewind(fp);
    cs->fileSize = fsize;
    cs->fptr = fp;
}

/* ----- Running Each Phase ------ */

void RunTokenizer(CompilationState* cs)
{
    TokenizerContext* ctx = InitalizeTokenizerContext(cs->fptr);
    Token tok;
    if (cs->flags.stopAfter == TOKENIZE) {
        while ( (tok = GetNextToken(ctx)).type != ERR && tok.type != END) {
            printf("Token: %s\tRow: %d\tCol:%d\tTokNum: %d\n", tok.lexeme, tok.row, tok.col, tok.type);
        }
        DestroyTokenizerContext(ctx);
    }
    cs->tokenizer = ctx;
}

void RunParser(CompilationState* cs)
{
    ParserContext* ctx = InitalizeParserContext(cs->tokenizer, cs->fileSize);
    Program(ctx);
    if (cs->flags.stopAfter == PARSE) {
        // Print AST Here 
        DestroyParserContext(ctx);
    }
    cs->parser = ctx;
}

void RunPreprocessor(CompilationState* cs)
{
    cs->preprocessor = NULL;
}

void RunNameResolver(CompilationState* cs)
{
    cs->nameres = NULL;
}

void RunTypeChecker(CompilationState* cs)
{
    cs->typecheck = NULL;
}

void RunLLVMGenerateIR(CompilationState* cs)
{
    cs->irgen = NULL;
}

void RunAssemble(CompilationState* cs)
{
    cs->assembler = NULL;
}
