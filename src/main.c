#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "DataStructures/Structs.h"
#include "DataStructures/Pair.h"
#include "DataStructures/Queue.h"

#include "DataStructures/Map.h"
#include "Tokenizer.h"


int main(int argc, char* argv[])
{
	printf("Hello Cruel World: \n Param Count: %d \n Program Name: %s\n", argc, argv[0]);
    Queue Tokens = MakeQueue();
    
    /* Open code file to read */
    FILE* fptr;
    fptr = fopen(argv[1], "r");
    if(!fptr)
    {
        printf("ERROR: Opening source file %s\n", argv[1]);
        return 1;
    }

    Pair p;
    int i = 0;
    char* j = "hey";
    p = MakePair(ANY(i, C_INT), ANY(j, C_STR));

    /* Iterate through each character */
    int charCount = 0;
    char c;

    Token next;
    next.type = IDENT;

    while(next.type != ERR && next.type != END)
    {
        next = GetNextToken(fptr);
        printf("Token: %d\tValue: %d\n", charCount, next.type); 

        charCount++;

        if(charCount > 10)
            break;
    }
    
    FreeQueue(&Tokens);
	return 0;
}
