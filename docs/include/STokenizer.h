#ifndef _BRIAN_TOKENIZER_H_ 
#define _BRIAN_TOKENIZER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "SToken.h"

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

TokenizerContext* InitalizeTokenizerContext(FILE* fptr);
void DestroyTokenizerContext(TokenizerContext* ctx);

void LoadBuffer(TokenizerContext* ctx, int bufferNum);
void RetractBuffer(TokenizerContext* ctx);
char AdvanceBuffer(TokenizerContext* ctx);

/* ----- Table Driven DFA ----- */

typedef enum {
    CC_ERROR,
    CC_DIGIT, CC_ALPHA, CC_PLUS, CC_MINUS, CC_EQUALS, 
    CC_MULT, CC_DIVIDE, CC_MODULUS, CC_AND, CC_OR, CC_NOT, 
    CC_NEGATE, CC_XOR, CC_RANGLE, CC_LANGLE, CC_LBRACE, 
    CC_RBRACE, CC_LPAREN, CC_RPAREN, CC_LBRACK, CC_RBRACK,
    CC_DOT, CC_AT, CC_COLON, CC_SEMI, CC_HASH, CC_QUESTION,
    CC_COMMA, CC_SINGLE_QUOTE, CC_DOUBLE_QUOTE,
    CC_COUNT
} CharClass;

static const CharClass CHAR_MAP[256] = {
    /* Initalizes to 0 if doesn't match (CC_ERROR) */
    ['0' ... '9'] = CC_DIGIT,
    ['a' ... 'z'] = CC_ALPHA,
    ['A' ... 'Z'] = CC_ALPHA,
    ['_']         = CC_ALPHA,   
    ['+']  = CC_PLUS,   ['-']  = CC_MINUS,  ['=']  = CC_EQUALS,
    ['*']  = CC_MULT,   ['/']  = CC_DIVIDE, ['%']  = CC_MODULUS,
    ['&']  = CC_AND,    ['|']  = CC_OR,     ['~']  = CC_NEGATE,
    ['^']  = CC_XOR,    ['>']  = CC_RANGLE, ['<']  = CC_LANGLE,
    ['{']  = CC_LBRACE, ['}']  = CC_RBRACE,
    ['(']  = CC_LPAREN, [')']  = CC_RPAREN,
    ['[']  = CC_LBRACK, [']']  = CC_RBRACK,
    ['.']  = CC_DOT,    ['@']  = CC_AT,     [':']  = CC_COLON,
    [';']  = CC_SEMI,   ['#']  = CC_HASH,   ['?']  = CC_QUESTION,
    ['!']  = CC_NOT,    [',']  = CC_COMMA, 
    ['\''] = CC_SINGLE_QUOTE, ['"'] = CC_DOUBLE_QUOTE
};

// Yes I manually wrote this, yes it was hell
static const int TABLE_DFA[][CC_COUNT] = {
    {  }, // Error 
    { 
        [CC_PLUS] = 2, [CC_MINUS] = 3, [CC_MULT] = 4, [CC_DIVIDE] = 5, [CC_MODULUS] = 6, [CC_EQUALS] = 7, 
        [CC_AND] = 8, [CC_OR] = 9, [CC_NEGATE] = 10, [CC_XOR] = 11, [CC_RANGLE] = 12, [CC_LANGLE] = 13,
        [CC_LBRACE] = 14, [CC_RBRACE] = 15, [CC_LPAREN] = 16, [CC_RPAREN] = 17, [CC_LBRACK] = 18, [CC_RBRACK] = 19,
        [CC_DOT] = 20, [CC_AT] = 21, [CC_COLON] = 22, [CC_SEMI] = 23, [CC_QUESTION] = 24, [CC_NOT] = 25, [CC_COMMA] = 26,
    }, // Start
    { [CC_EQUALS] = 27, [CC_PLUS] = 28 }, // +
    { [CC_EQUALS] = 29, [CC_MINUS] = 30, [CC_RANGLE] = 31 }, // -
    { [CC_EQUALS] = 32, [CC_MULT] = 33 }, // *
    { [CC_EQUALS] = 34 }, // / Maybe comments here?
    { [CC_EQUALS] = 35 }, // %
    { [CC_EQUALS] = 36 }, // =
    { [CC_EQUALS] = 37, [CC_AND] = 38 }, // &
    { [CC_EQUALS] = 39, [CC_OR] = 40 }, // |
    { [CC_EQUALS] = 41 }, // ~
    { [CC_EQUALS] = 42 }, // ^
    { [CC_EQUALS] = 43, [CC_RANGLE] = 44 }, // >
    { [CC_EQUALS] = 45, [CC_LANGLE] = 46, [CC_MINUS] = 47 }, // <
    {  }, // {
    {  }, // }
    {  }, // (
    {  }, // )
    {  }, // [
    {  }, // ]
    { [CC_QUESTION] = 48 }, // .
    {  }, // @
    {  }, // :
    {  }, // ;
    {  }, // ?
    { [CC_EQUALS] = 49, }, // !
    {  }, // ,
    {  }, // +=
    {  }, // ++
    {  }, // -=
    {  }, // --
    {  }, // *= 
    {  }, // ** 
    { [CC_QUESTION] = 50 }, // ->
    {  }, // /=
    {  }, // %=
    {  }, // ==
    {  }, // &=
    { [CC_EQUALS] = 51 }, // &&
    {  }, // |=
    { [CC_EQUALS] = 52 }, // ||
    {  }, // ~=
    {  }, // ^=
    {  }, // >= 
    { [CC_EQUALS] = 53 }, // >>
    {  }, // <=
    { [CC_EQUALS] = 54 }, // <<
    {  }, // <-
    {  }, // .?
    {  }, // ->?
    {  }, // &&=
    {  }, // ||=
    {  }, // >>=
    {  }, // <<=
}; 

static const int ACCEPT_STATES[] = {
    [PLUS] = 1, [MINUS] = 2, [MULT] = 3, [DIV] = 4, [MOD] = 5, [EQ] = 6,
    [AND] = 7, [OR] = 8, [NEG] = 9, [XOR] = 10, [GREAT] = 11, [LESS] = 12,
    [LBRACE] = 13, [RBRACE] = 14, [LPAREN] = 15, [RPAREN] = 16, [LBRACK] = 17, [RBRACE] = 18,
    [MEM] = 19, [DOTPROD] = 20, [COLON] = 21, [SEMI] = 22, [QUESTION] = 23,
    
};

bool IsAcceptState(int c);

/* ----- Function Driven DFA ----- */



/* ----- Tokenization ----- */

Token GetNextToken(TokenizerContext* ctx);

int SkipWhitespace(TokenizerContext* ctx);
Token ScanOperator(TokenizerContext* ctx);
Token ScanDirective(TokenizerContext* ctx);
Token ScanNumber(TokenizerContext* ctx);
Token ScanString(TokenizerContext* ctx);
Token ScanCharacter(TokenizerContext* ctx);
Token ScanIdentOrKeyword(TokenizerContext* ctx);


#endif
