#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "Tokenizer.h"

int main(int argc, char* argv[])
{
    int retcode = 0;

    FILE* fptr
    fptr = fopen(argv[1], "r");
    if(!fptr)
    {
        perror("ERROR: Opening source file %s\n", argv[1]);
        return -1;
    }


    Token next;
    while(1)
    {
        Token correct = *correctList++;
        next = GetNextToken(fptr);

        if(next.token != correct)
        {
            printf("FAIL: %s\n", correctList.second);
            retcode++;
        }
    }

    printf("\nFailed Test Cases: %d\n", retcode);
    return retcode;
}
