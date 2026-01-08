#include "Preprocessor.h"

/* Just Skip Comments and Directives for now */

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

    /*printf("%s\n", t.lex.word);*/

    return t;
}

/*
int ImportList(FILE* fptr, AST* ast)
{
   TODO: This should be a preprocessor step 

    Token t;
    while(true)
    {
        t = GetNextTokenP(fptr);
        if(t.type != HASH)
        {
            PutTokenBack(&t);
            return NAP;
        }
        
        t = GetNextTokenP(fptr);
        if(!strcmp(t.lex.word, "include"))
        {
            TODO: Check other prepocess types
            return ERRP;
        }

        t = GetNextTokenP(fptr);
        if(t.type != LESS)
            return ERRP;

        t = GetNextTokenP(fptr);
        if(t.type != IDENT)
            return ERRP;

        t = GetNextTokenP(fptr);
        if(t.type != GREAT)
            return ERRP;

    }
    return VALID;
}
*/
