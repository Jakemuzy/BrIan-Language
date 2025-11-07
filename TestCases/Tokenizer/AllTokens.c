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
    KeyVal kv4 = {DO, "do"};
    KeyVal kv5 = {WHILE, "while"};
    KeyVal kv6 = {FOR, "for"};
    KeyVal kv7 = {CHAR, "char"};
    KeyVal kv8 = {SHORT, "short"};
    KeyVal kv9 = {INT, "int"};
    KeyVal kv10 = {DOUBLE, "double"};
    KeyVal kv11 = {LONG, "long"};
   
    Dict* d = DictMake(11, kv1, kv2, kv3, kv4, kv5, kv6, kv7, kv8, kv9, kv10, kv11);
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
