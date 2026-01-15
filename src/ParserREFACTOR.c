#include "Parser.h"

/* ----------- HELPER ---------- */

int ValidTokType(const int types[], int arrSize, int type)
{
    int i;
    for(i = 0; i < arrSize; i++)
    {
        if(types[i] == type)
            return VALID;
    }
    return NAP;
}

/* Reads current token, displays error message and returns error type if not valid */ 
int CompareToken(FILE* fptr, TokenType desired, char* errMessage, int errType)
{
    Token current = GetNextToken(fptr);
    if(current.type != desired)
    {   
        if(errType == ERRP)
            printf("ERROR: %s\n", errMessage);
        else if (errType == NAP) 
            PutTokenBack(&current);
        return errType;
    }
   
    return VALID;
}

bool ParseOperatorToken(FILE* fptr, TokenType desired, ASTNode** resultNode)
{
    Token current = GetNextToken(fptr);
    if(current.type != desired)
    {
        PutTokenBack(&current);
        return false;
    }

    ASTNode* tokNode = InitASTNode();
    ASTMakeTokNode(tokNode, current);
    *resultNode = tokNode;

    return true;
}
