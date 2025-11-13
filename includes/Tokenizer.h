#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#include "Token.h"
#include "Dictionary.h"
/*
Map KeywordMap {
    ("if", IF),
    ("else", ELSE),
    ("while", WHILE),

}
*/

Token GetNextToken(FILE* fptr);
void UpdateLexeme(Token* t, int c);

/* Categories */
int IsOperato(FILE* fptr, Token* t, int c);
int IsNumber (FILE* fptr, Token* t, int c);
int IsLiteral(FILE* fptr, Token* t, int c);
int IsBracket(FILE* fptr, Token* t, int c);
int IsComp   (FILE* fptr, Token* t, int c);
int IsBitwise(FILE* fptr, Token* t, int c);
int IsUnary  (FILE* fptr, Token* t, int c);

/* Others */
int IsComm(FILE* fptr, Token* t, int c);
int IsEnd (FILE* fptr, Token* t, int c);

/* Operators */
int IsEqual(FILE* fptr, Token* t, int c);
int IsPlus (FILE* fptr, Token* t, int c);
int IsMinus(FILE* fptr, Token* t, int c);
int IsDiv  (FILE* fptr, Token* t, int c);
int IsMult (FILE* fptr, Token* t, int c);
int IsMod  (FILE* fptr, Token* t, int c);

/* Comparisons */
int IsNeqq(FILE* fptr, Token* t, int c);
int IsGeqq(FILE* fptr, Token* t, int c);
int IsLeqq(FILE* fptr, Token* t, int c);
int IsAndl(FILE* fptr, Token* t, int c);
int IsOrl (FILE* fptr, Token* t, int c);


int IdentOrKeyword(FILE* fptr, Token* t, int c);
