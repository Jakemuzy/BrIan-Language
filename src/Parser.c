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
        return NAP;
    }

    return VALID;
}

int Body(FILE* fptr, AST* ast)
{
    Token t = GetNextToken(fptr);
    if(t.type != LBRACK)
    {
        PutTokenBack(&t);
        return NAP;
    }

    StmtList(fptr, ast);

    t = GetNextToken(fptr);
    if(t.type != RBRACK)
    {
        PutTokenBack(&t);
        return ERRP;
    }

    return VALID;
}
int StmtList(FILE* fptr, AST* ast)
{
    int retCode = VALID;

    while(retCode != ERRP && retCode != NAP)
        retCode = Stmt(fptr, ast);
    return retCode;
}
int Stmt(FILE* fptr, AST* ast)
{
    Token t;
    if(!CtrlStmt(fptr, ast) && !LineStmt(fptr, ast))
        return NAP;

    t = GetNextToken(fptr);
    if(t.type != SEMI)
    {
        perror("ERROR: Semicolon missing\n");
        PutTokenBack(&t);
        return ERRP;
    }
    return VALID;
}
