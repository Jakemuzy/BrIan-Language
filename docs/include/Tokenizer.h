#ifndef _BRIAN_TOKENIZER_H_ 
#define _BRIAN_TOKENIZER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "ErrorHandler.h"
#include "ArenaAllocator.h"

#include "Token.h"

#define TOKENIZER_BUFFER_SIZE 512
#define TOKENIZER_SENTINEL '\0'
#define DFA_ERROR_STATE 0

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

typedef struct TokenizerContext {
    char buffer1[TOKENIZER_BUFFER_SIZE];
    char buffer2[TOKENIZER_BUFFER_SIZE];
    char* lexemeBegin, *forward;

    int row, col;

    /* Weird Edge Case Flags */
    bool nestedChan;  // Channels, chan<chan<...>>, where >> tokenizes as rshift

    FILE* fptr;
    Arena* arena;
} TokenizerContext;

TokenizerContext* InitalizeTokenizerContext(FILE* fptr, size_t fileSize);
void DestroyTokenizerContext(TokenizerContext* ctx);
void SetEdgeCaseFlag(TokenizerContext* ctx, bool val);

void LoadBuffer(TokenizerContext* ctx, int bufferNum);
void RetractBuffer(TokenizerContext* ctx, char* pos);
int AdvanceBuffer(TokenizerContext* ctx);
Token ExtractTokenFromBuffer(TokenizerContext* ctx);

/* ----- Table Driven DFA ----- */

typedef enum CharClass {
    CC_ERROR,
    CC_DIGIT, CC_ALPHA, CC_PLUS, CC_MINUS, CC_EQUALS, 
    CC_MULT, CC_DIVIDE, CC_MODULUS, CC_AND, CC_OR, CC_NOT, 
    CC_NEGATE, CC_XOR, CC_RANGLE, CC_LANGLE, CC_LBRACE, 
    CC_RBRACE, CC_LPAREN, CC_RPAREN, CC_LBRACK, CC_RBRACK,
    CC_DOT, CC_AT, CC_COLON, CC_SEMI, CC_HASH, CC_QUESTION,
    CC_COMMA, CC_SINGLE_QUOTE, CC_DOUBLE_QUOTE,
    CC_COUNT
} CharClass;

static const CharClass CHAR_MAP[] = {
    /* Initalizes to 0 if doesn't match (CC_ERROR) */
    ['0'] = CC_DIGIT, ['1'] = CC_DIGIT, ['2'] = CC_DIGIT, ['3'] = CC_DIGIT, ['4'] = CC_DIGIT,
    ['5'] = CC_DIGIT, ['6'] = CC_DIGIT, ['7'] = CC_DIGIT, ['8'] = CC_DIGIT, ['9'] = CC_DIGIT,

    ['a'] = CC_ALPHA, ['b'] = CC_ALPHA, ['c'] = CC_ALPHA, ['d'] = CC_ALPHA, ['e'] = CC_ALPHA,
    ['f'] = CC_ALPHA, ['g'] = CC_ALPHA, ['h'] = CC_ALPHA, ['i'] = CC_ALPHA, ['j'] = CC_ALPHA,
    ['k'] = CC_ALPHA, ['l'] = CC_ALPHA, ['m'] = CC_ALPHA, ['n'] = CC_ALPHA, ['o'] = CC_ALPHA,
    ['p'] = CC_ALPHA, ['q'] = CC_ALPHA, ['r'] = CC_ALPHA, ['s'] = CC_ALPHA, ['t'] = CC_ALPHA,
    ['u'] = CC_ALPHA, ['v'] = CC_ALPHA, ['w'] = CC_ALPHA, ['x'] = CC_ALPHA, ['y'] = CC_ALPHA,
    ['z'] = CC_ALPHA,

    ['A'] = CC_ALPHA, ['B'] = CC_ALPHA, ['C'] = CC_ALPHA, ['D'] = CC_ALPHA, ['E'] = CC_ALPHA,
    ['F'] = CC_ALPHA, ['G'] = CC_ALPHA, ['H'] = CC_ALPHA, ['I'] = CC_ALPHA, ['J'] = CC_ALPHA,
    ['K'] = CC_ALPHA, ['L'] = CC_ALPHA, ['M'] = CC_ALPHA, ['N'] = CC_ALPHA, ['O'] = CC_ALPHA,
    ['P'] = CC_ALPHA, ['Q'] = CC_ALPHA, ['R'] = CC_ALPHA, ['S'] = CC_ALPHA, ['T'] = CC_ALPHA,
    ['U'] = CC_ALPHA, ['V'] = CC_ALPHA, ['W'] = CC_ALPHA, ['X'] = CC_ALPHA, ['Y'] = CC_ALPHA,
    ['Z'] = CC_ALPHA, ['_'] = CC_ALPHA,   
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
    { 0 }, // Error 
    { 
        [CC_PLUS] = 2, [CC_MINUS] = 3, [CC_MULT] = 4, [CC_DIVIDE] = 5, [CC_MODULUS] = 6, [CC_EQUALS] = 7, 
        [CC_AND] = 8, [CC_OR] = 9, [CC_NEGATE] = 10, [CC_XOR] = 11, [CC_RANGLE] = 12, [CC_LANGLE] = 13,
        [CC_LBRACE] = 14, [CC_RBRACE] = 15, [CC_LPAREN] = 16, [CC_RPAREN] = 17, [CC_LBRACK] = 18, [CC_RBRACK] = 19,
        [CC_DOT] = 20, [CC_AT] = 21, [CC_COLON] = 22, [CC_SEMI] = 23, [CC_QUESTION] = 24, [CC_NOT] = 25, [CC_COMMA] = 26,
    }, // Start
    { [CC_EQUALS] = 27, [CC_PLUS] = 28 }, // +
    { [CC_EQUALS] = 29, [CC_MINUS] = 30, [CC_RANGLE] = 31 }, // -
    { [CC_EQUALS] = 32, [CC_MULT] = 33 }, // *
    { [CC_EQUALS] = 34 }, // /
    { [CC_EQUALS] = 35 }, // %
    { [CC_EQUALS] = 36 }, // =
    { [CC_EQUALS] = 37, [CC_AND] = 38 }, // &
    { [CC_EQUALS] = 39, [CC_OR] = 40 }, // |
    { [CC_EQUALS] = 41 }, // ~
    { [CC_EQUALS] = 42 }, // ^
    { [CC_EQUALS] = 43, [CC_RANGLE] = 44 }, // >
    { [CC_EQUALS] = 45, [CC_LANGLE] = 46, [CC_MINUS] = 47 }, // <
    { 0 }, // {
    { 0 }, // }
    { 0 }, // (
    { 0 }, // )
    { 0 }, // [
    { 0 }, // ]
    { [CC_QUESTION] = 48 }, // .
    { 0 }, // @
    { 0 }, // :
    { 0 }, // ;
    { 0 }, // ?
    { [CC_EQUALS] = 49, }, // !
    { 0 }, // ,
    { 0 }, // +=
    { 0 }, // ++
    { 0 }, // -=
    { 0 }, // --
    { [CC_QUESTION] = 50 }, // ->
    { 0 }, // *= 
    { 0 }, // ** 
    { 0 }, // /=
    { 0 }, // %=
    { 0 }, // ==
    { 0 }, // &=
    { [CC_EQUALS] = 51 }, // &&
    { 0 }, // |=
    { [CC_EQUALS] = 52 }, // ||
    { 0 }, // ~=
    { 0 }, // ^=
    { 0 }, // >= 
    { [CC_EQUALS] = 53 }, // >>
    { 0 }, // <=
    { [CC_EQUALS] = 54 }, // <<
    { 0 }, // <-
    { 0 }, // .?
    { 0 }, // !=
    { 0 }, // ->?
    { 0 }, // &&=
    { 0 }, // ||=
    { 0 }, // >>=
    { 0 }, // <<=
}; 

static const int ACCEPT_STATES[] = {
    [0] = ERR,
    [2] = PLUS, [3] = MINUS, [4] = MULT, [5] = DIV, [6] = MOD, [7] = EQ,
    [8] = AND, [9] = OR, [10] = NEG, [11] = XOR, [12] = GREAT, [13] = LESS,
    [14] = LBRACE, [15] = RBRACE, [16] = LPAREN, [17] = RPAREN, [18] = LBRACK, [19] = RBRACK,
    [20] = MEM, [21] = DOTPROD, [22] = COLON, [23] = SEMI, [24] = QUESTION, [25] = NOT,
    [26] = COMMA, [27] = PEQ, [28] = INC, [29] = SEQ, [30] = DEC, [31] = REF, [32] = MEQ, 
    [33] = POW, [34] = DEQ, [35] = MODEQ, [36] = EQQ, [37] = ANDEQ, [38] = ANDL, [39] = OREQ,
    [40] = ORL, [41] = NEGEQ, [42] = XOREQ, [43] = GEQQ, [44] = RSHIFT, [45] = LEQQ, [46] = LSHIFT,
    [47] = SEND, [48] = SMEM, [49] = NEQQ, [50] = SREF, [51] = ANDLEQ, [52] = ORLEQ,
    [53] = RIGHTEQ, [54] = LEFTEQ
};


/* ----- Tokenization ----- */

Token GetNextToken(TokenizerContext* ctx);

int SkipWhitespace(TokenizerContext* ctx);
Token SkipComment(TokenizerContext* ctx);
Token ScanOperator(TokenizerContext* ctx);
Token ScanDirective(TokenizerContext* ctx);
Token ScanNumber(TokenizerContext* ctx);
Token ScanString(TokenizerContext* ctx);
Token ScanCharacter(TokenizerContext* ctx);
Token ScanIdentOrKeyword(TokenizerContext* ctx);


#endif
