#ifndef _BRIAN_TOKEN_H_ 
#define _BRIAN_TOKEN_H_

#include <stdio.h>
#include "Token.h"

#define TOKENIZER_BUFFER_SIZE 512
#define TOKENIZER_SENTINEL '\0'

/*         BrIan Tokenizer
    ----------------------------
     Refer to docs/pipeline.md 
           for details.  
           
   Hand rolled, table driven DFA
   for operators. Function driven
  DFA for strings, directives, etc.
       Hashmap for keywords.

*/

/* ----- Double Buffered Context ----- */

typedef struct {
    char buffer1[TOKENIZER_BUFFER_SIZE];
    char buffer2[TOKENIZER_BUFFER_SIZE];
    char* lexemeBegin, *forward;

    int row, col;

    FILE* fptr;
} TokenizerContext;

TokenizerContext InitalizeTokenizerContext(FILE* fptr);

void LoadBuffer(TokenizerContext ctx, int bufferNum);
void RetractBuffer(TokenizerContext ctx);
char AdvanceBuffer(TokenizerContext ctx);

/* ----- Table Driven DFA ----- */

/* I did in fact code this by hand */
static int edges[][256] = {
    /* */
}; 

/* ----- Function Driven DFA ----- */

#endif