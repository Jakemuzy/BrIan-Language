#include "Preprocessor.h"

/* Just Skip Comments and Directives for now */

Token GetNextTokenP(FILE* fptr)
{

    Token t;

    do {
        t = GetNextToken(fptr);

        while (t.type == COMMENT)
            t = GetNextToken(fptr);

        if (t.type == HASH) {
            do {
                t = GetNextToken(fptr);
            } while (t.type != GREAT);
            t = GetNextToken(fptr);
        }

    } while (t.type == COMMENT || t.type == HASH);

    return t;
}
