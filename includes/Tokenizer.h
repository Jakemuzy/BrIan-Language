#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "DataStructures/Token.h"

/*
Map KeywordMap {
    ("if", IF),
    ("else", ELSE),
    ("while", WHILE),

}
*/

Token GetNextToken(FILE* fptr);

/* Categories */
int IsOperator(FILE* fptr, Token* t, int c);
int IsNumber(FILE* fptr, Token* t, int c);
int IsLiteral(FILE* fptr, Token* t, int c);
int IsBracket(FILE* fptr, Token* t, int c);
int IsComparison(FILE* fptr, Token* t, int c);
int IsBitwise(FILE* fptr, Token* t, int c);
int IsUnary(FILE* fptr, Token* t, int c);

/* Others */
int IsComment(FILE* fptr, Token* t, int c);
int IsEnd(FILE* fptr, Token* t, int c);

/* Operators */
int IsEqual(FILE* fptr, Token* t, int c);
int IsPlus(FILE* fptr, Token* t, int c);
int IsMinus(FILE* fptr, Token* t, int c);
int IsDiv(FILE* fptr, Token* t, int c);
int IsMult(FILE* fptr, Token* t, int c);
int IsMod(FILE* fptr, Token* t, int c);

int IdentOrKeyword(FILE* fptr, Token* t, int c);
