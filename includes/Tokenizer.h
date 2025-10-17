#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

Token GetNextToken(FILE* fptr);

TokenType IsEqual(FILE* fptr, int c);
TokenType IsOperator(FILE* fptr, int c);
TokenType IsComment(FILE* fptr, int c);
TokenType IsNumber(FILE* fptr, int c);
TokenType IsLiteral(FILE* fptr, int c);

TokenType IsPlus(FILE* fptr, int c);
TokenType IsMinus(FILE* fptr, int c);
TokenType IsDiv(FILE* fptr, int c);
TokenType IsMult(FILE* fptr, int c);
TokenType IsMod(FILE* fptr, int c);


