#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "Dictionary.h"
#include "Tokenizer.h"

int main(int argc, char* argv[])
{
    int retcode = 0;

    FILE* fptr;
    fptr = fopen(argv[1], "r");
    if(!fptr)
    {
        printf("ERROR: Opening source file %s\n", argv[1]);
        return -1;
    }

    KeyVal kv1 = {IF, "if"};
    KeyVal kv2 = {ELIF, "elif"};
    KeyVal kv3 = {ELSE, "else"};
    Dict* d = DictMake(3, kv1, kv2, kv3);
    DictPrint(*d);

/*
    Token next;
    while(1)
    {
        Token correct = NA; *correctList++;
        next = GetNextToken(fptr);

        if(next.TokenType != correct)
        {
            /*printf("FAIL: %s\n", correctList.second);
            retcode++;
        }
    }

    printf("\nFailed Test Cases: %d\n", retcode);
    */
    return retcode;
}
