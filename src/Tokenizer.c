#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "DataStructures/Structs.h"
#include "DataStructures/Pair.h"
#include "DataStructures/Queue.h"
#include "FileStream.h"

#include "DataStructures/Map.h"

typedef enum { 
    KEYWORD, IDENTIFIER,
    INTEGRAL, DECIMAL, HEX, BIN, 
    OPERATOR, SEPARATOR, 
    CHAR_LITERAL, STRING_LITERAL 
} TokenState;
/*
Map KeywordMap {
    ("if", IF),
    ("else", ELSE),
    ("while", WHILE),

}
*/


/* Helper Functions 
Token KeywordOrIdent(char* word)
{
    for(i = 0; i < KeywordMap.size(); i++)
    {
        Token kw = KeywordMap.At(i);
        if(word == kw)
        {
            return kw;
        }
    }
    return IDENTIFIER;
}
*/

/* Main loop */
int main(int argc, char* argv[])
{
	printf("Hello Cruel World: \n Param Count: %d \n Program Name: %s\n", argc, argv[0]);
    Queue Tokens = MakeQueue();
    
    /* Open code file to read */
    FILE* file;
    file = fopen(argv[1], "r");
    if(!file)
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

    Token curTok;
    curTok.type = IDENTIFIER;
    curTok.line = charCount;

    TokenState state = IDENTIFIER;
    while((c = fgetc(file)) != EOF)
    {
        /* Enqueue(&Tokens, CHAR); */
        
        /* Finish Token */
        if(isspace(c))
        {

        }
        
        charCount++;
    }
    
    FreeQueue(&Tokens);
	return 0;
}
