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

void ImportList(FILE* fptr, AST* ast)
{
}

void Body(FILE* fptr, AST* ast)
{
}
