#ifndef _BRIAN_TOKEN_H_ 
#define _BRIAN_TOKEN_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 1 Less than buffer tokenizer buffer size */
#define TOKEN_MAX_LENGTH 511
#define MAX_KW_HASH 378

/*   BrIan Tokens and TokenTypes 
    ----------------------------
      Refer to docs/tokens.md 
           for details.   

*/

typedef enum TokenType {
    /* Control Flow Keywords */
    IF, ELIF, ELSE, DO, WHILE, FOR, SWITCH,
    CASE, DEFAULT, BREAK, CONTINUE, RETURN,

    /* Concurrency Keywords */
    LOCK, CRITICAL, SPAWN, AWAIT, 

    /* Data Type Keywords */
    CHAR, BOOL, INT, FLOAT, DOUBLE, LONG, 
    I8, I16, I32, I64, U8, U16, U32, U64,
    MAT, VEC, CHANNEL, MUTEX, SEMAPHORE, TASK, 
    VOID, STRING, 

    /* Type Declarators */
    AS, LET,

    /* Custom Types Keywords */
    ENUM, STRUCT, TYPEDEF, FUNCTION, FUNCPTR, 
    CLOSURE, INTERFACE, OPERATOR, LAMBDA, CAPTURES,

    /* Predefined Types */
    TRUE, FALSE, NILL,

    /* Qualifiers */
    CONST, STATIC, VOLATILE, INLINE, ATOMIC, EXTERN,

    /* Asignment Operators */
    EQ, PEQ, SEQ, MEQ, DEQ, MODEQ, ANDEQ, OREQ, 
    ANDLEQ, ORLEQ, NEGEQ, XOREQ, RIGHTEQ, LEFTEQ, 
    INC, DEC,

    /* Binary Operators */
    PLUS, MINUS, DIV, MULT, POW, MOD,

    /* Logical Operators */
    EQQ, NEQQ, GEQQ, LEQQ, NOT, ANDL, ORL, GREAT, LESS,

    /* Bitwise Operators */
    NEG, XOR, OR, AND, LSHIFT, RSHIFT,

    /* Structures Operators */
    SREF, SMEM, REF, MEM,

    /* Other Operators */
    SIZEOF, DOTPROD, QUESTION, COLON, SEND, SEMI, COMMA, 

    /* Brackets */
    LPAREN, RPAREN, LBRACK, RBRACK, LBRACE, RBRACE, 

    /* Idenifiers and Literals */
    IDENT, SLITERAL, CLITERAL, REAL, INTEGRAL, HEX,

    /* Others */
    DIRECTIVE, COMMENT, NA, ERR, END

} TokenType;

typedef struct Token {
    TokenType type; 
    int row, col;

    char* lexeme;
    size_t lexLength;
} Token;

#define DUMMY_TOKEN (Token){0}

/* ----- Keyword Map ----- */

// Each value of the string is added together and 
typedef struct { char* str; TokenType type; } KeywordTypePair;
static const KeywordTypePair KEYWORD_MAP[378] = {
    [76]  = {"if", IF},        [173] = {"elif", ELIF},
    [6]   = {"else", ELSE},    [170] = {"do", DO},
    [82]  = {"while", WHILE},  [192] = {"for", FOR},
    [255] = {"switch", SWITCH},[203] = {"case", CASE},
    [102] = {"default", DEFAULT},[106] = {"break", BREAK},
    [116] = {"continue", CONTINUE},[257] = {"return", RETURN},
    [300] = {"lock", LOCK},    [250] = {"critical", CRITICAL},
    [54]  = {"spawn", SPAWN},  [41]  = {"await", AWAIT},
    [37]  = {"sizeof", SIZEOF},[39]  = {"char", CHAR},
    [201] = {"bool", BOOL},    [128] = {"int", INT},
    [27]  = {"float", FLOAT},  [208] = {"double", DOUBLE},
    [283] = {"long", LONG},    [243] = {"void", VOID},
    [358] = {"string", STRING},[148] = {"i8", I8},
    [11]  = {"i16", I16},      [235] = {"i32", I32},
    [18]  = {"i64", I64},      [326] = {"u8", U8},
    [99]  = {"u16", U16},      [61]  = {"u32", U32},
    [338] = {"u64", U64},      [262] = {"mutex", MUTEX},
    [269] = {"semaphore", SEMAPHORE}, [284] = {"task", TASK},
    [17]  = {"chan", CHANNEL},  [230] = {"enum", ENUM},
    [136] = {"struct", STRUCT}, [178] = {"typedef", TYPEDEF},
    [90]  = {"interface", INTERFACE}, [375] = {"operator", OPERATOR},
    [200] = {"lambda", LAMBDA}, [9]   = {"true", TRUE},
    [366] = {"false", FALSE},   [2]   = {"null", NILL},
    [20]  = {"const", CONST},  [73]  = {"static", STATIC},
    [341] = {"volatile", VOLATILE}, [160] = {"inline", INLINE},
    [348] = {"atomic", ATOMIC}, [95]  = {"extern", EXTERN},
    [339]  = {"fn", FUNCTION}, [335] = {"mat", MAT},
    [109] = {"vec", VEC},      [325] = {"as", AS}, 
    [164] = {"let", LET},      [227] = {"fp", FUNCPTR},
    [298] = {"closure", CLOSURE}, [186] = {"captures", CAPTURES}
};

int KeywordHash(const char* word);
TokenType KeywordLookup(const char* word);


#endif
