#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "Tokenizer.h"


int main()
{
    FILE* fptr = fopen("TestCases/Tokenizer/AllTokens.b", "r");
    if(!fptr) {
        printf("ERROR: Opening source file\n");
        return 1;
    }


    Token next;
    next.type = IDENT;
    int tokenCount = 0;

    while(next.type != ERR && next.type != END) {
        next = GetNextToken(fptr);
        printf("Token: %d\tValue: %d\tLine: %d \tWord: %s\n", tokenCount, next.type, next.line, next.lex.word); 

        tokenCount++;
    }
    
	return 0;
}
