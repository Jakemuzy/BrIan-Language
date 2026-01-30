#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "Tokenizer.h"


int main(int argc, char* argv[])
{
    
    /* Open code file to read */
    FILE* fptr;
    fptr = fopen(argv[1], "r");
    if(!fptr)
    {
        printf("ERROR: Opening source file %s\n", argv[1]);
        return 1;
    }

    /* Iterate through each character */
    int tokenCount = 0;
    char c;

    Token next;
    next.type = IDENT;

    while(next.type != ERR && next.type != END)
    {
        next = GetNextToken(fptr);
        printf("Token: %d\tValue: %d\tLine: %d \tWord: %s\n", tokenCount, next.type, next.line, next.lex.word); 

        tokenCount++;
    }
    
	return 0;
}
