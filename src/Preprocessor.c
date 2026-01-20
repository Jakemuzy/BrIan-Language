#include "Preprocessor.h"

/* Just Skip Comments and Directives for now */
/* TODO: Imports */

Token GetNextTokenP(FILE* fptr)
{
    Token t;

    do {
        t = GetNextToken(fptr);

        while (t.type == COMMENT)
            t = GetNextToken(fptr);

        if (t.type == HASH) {           /* Ignore Preprocess Directives for now */
            do {
                t = GetNextToken(fptr);
            } while (t.type != GREAT);
            t = GetNextToken(fptr);
        }

    } while (t.type == COMMENT || t.type == HASH);

    /* printf("%s\n", t.lex.word); */

    return t;
}

TokenType PeekNextTokenP(FILE* fptr)
{
    Token tok = GetNextTokenP(fptr);
    PutTokenBack(&tok);
    
    return tok.type;
}