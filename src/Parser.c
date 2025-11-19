#include "Parser.h"

AST* ConstructAST(FILE* fptr)
{
    AST tree;
    tree.root = malloc(sizeof(ASTNode));


    return;
}

/* ---------- EBNF ---------- */

void Program(FILE* fptr, AST* ast)
{
    Token t;

    ImportList(fptr, ast);

    t = GetNextToken(fptr);
    if(!strcmp(t.lex.word, "START"))
    {
        /* TODO: output line num and col */
        perror("ERROR: Program must have START function\n");
        exit(1);
    }

    Body(fptr, ast);

    /* TODO: For now we will force an update function, fix this later */
    t = GetNextToken(fptr);

    if(!strcmp(t.lex.word, "START"))
    {
        /* TODO: output line num and col */
        perror("ERROR: Program must have UPDATE function\n");
        exit(1);
    }
}

int ImportList(FILE* fptr, AST* ast)
{
    Token t = GetNextToken(fptr);
    if(!strcmp(t.lex.word, "#import"))
    {
        PutTokenBack(&t);
        return -1;
    }

    return 0;
}

int Body(FILE* fptr, AST* ast)
{
    Token t = GetNextToken(fptr);
    
    return 0;
}
