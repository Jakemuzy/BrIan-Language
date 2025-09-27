#include <stdio.h>
#include <stdbool.h>

#include <Any.h>
#include <Pair.h>
#include <Queue.h>
#include <FileStream.h>

typedef struct {
    Token type;
    char* lexeme;
    int line, col;
} Tok;

/*
Map KeywordMap {
    ("if", IF),
    ("else", ELSE),
    ("while", WHILE),

}
*/

int main(int argc, char* argv[])
{
	printf("Hello Cruel World: \n Param Count: %d \n Program Name: %s\n", argc, argv[0]);
    Queue Tokens  = MakeQueue();

    /* Open code file to read */
    FILE* file;
    file = fopen(argv[1], "r");
    if(!file)
    {
        printf("ERROR: Opening source file %s\n", argv[1]);
        return 1;
    }
    
    /* Pair testing */
    B_ANY b = ANY(54);

    /* Iterate through each character */
    int charCount = 0;
    char curr;
    char putbackChar; 
    
    while((curr = fgetc(file)) != EOF)
    {
        /* Enqueue(&Tokens, CHAR); 
        bool identifier = keyword = integral = floating = hex = binary = operator = separator = cliteral = sliteral = true;
        */

        putbackChar = 'a';
        printf("%d", putbackChar);
        charCount++;
    }
    
    FreeQueue(&Tokens);
	return 0;
}
