#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "DataStructures/Structs.h"

/*
Map KeywordMap {
    ("if", IF),
    ("else", ELSE),
    ("while", WHILE),

}
*/

Token GetNextToken(FILE* fptr);

/* Categories */
TokenType IsOperator(FILE* fptr, int c);
TokenType IsNumber(FILE* fptr, int c);
TokenType IsLiteral(FILE* fptr, int c);
TokenType IsBracket(FILE* fptr, int c);
TokenType IsComparison(FILE* fptr, int c);
TokenType IsBitwise(FILE* fptr, int c);
TokenType IsUnary(FILE* fptr, int c);

/* Others */
TokenType IsComment(FILE* fptr, int c);
TokenType IsEnd(FILE* fptr, int c);
TokenType IsSemi(FILE* fpr, int c);
TokenType IsColon(FILE* fptr, int c);

/* Operators */
TokenType IsEqual(FILE* fptr, int c);
TokenType IsPlus(FILE* fptr, int c);
TokenType IsMinus(FILE* fptr, int c);
TokenType IsDiv(FILE* fptr, int c);
TokenType IsMult(FILE* fptr, int c);
TokenType IsMod(FILE* fptr, int c);

TokenType IdentOrKeyword(Token t);
