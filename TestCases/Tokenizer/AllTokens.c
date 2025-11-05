#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "Tokenizer.h"
#include "Dictionary.h"

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

    KeyVal kv1 = {0, "hi"};
    KeyVal kv2 = {1, "Jake"};
    KeyVal kv3 = {2, "Joe"};
    Dict d = DictMake(3, kv1, kv2, kv3);
    DictPrint(d);


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
