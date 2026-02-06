#ifndef _TOKEN_H__
#define _TOKEN_H__

#include <stddef.h>

/* TODO: Add
    BOOL - bool keyword 
    QUES - (?) 

    SMEM - Safe member access ?.
    SREF - Safe member access by ref ?->
    REGISTER - Regsiter access @

    SPLICE - Array splicing and Ternary op :

    ENUM - Enum
    STRUCT - Struct
    BREAK

    CONST
    SIGNED
    STATIC
    TYPEDEF

    I8  - 8  Bit Int 
    I16 - 16 Bit Int
    I32 - 32 Bit Int
    I64 - 64 Bit Int
    
    U8  - 8  Bit Unsigned Int
    U16 - 16 Bit Unsigned Int
    U32 - 32 Bit Unsigned Int
    U64 - 64 Bit Unsigned Int


    ADD true/false keywords
    ADD Hex type (same sizes as U and I) 
*/

typedef enum {
    /* Ctrl Stmt Key Words */
    IF = 1, ELIF, ELSE, DO, WHILE, FOR, SWITCH, CASE, DEFAULT, BREAK, RET,

    /* Data Type Key Words */
    CHAR, BOOL, SHORT, INT, FLOAT, DOUBLE, LONG, VOID, STRING,
    I8, I16, I32, I64, U8, U16, U32, U64, 
    ENUM, STRUCT, 

    /* Qualifier and Modifier Key Words */
    CONST, SIGNED, UNSIGNED, STATIC, TYPEDEF,

    /* Assignment Operators */
    EQ, PEQ, SEQ, MEQ, DEQ, MODEQ, ANDEQ, OREQ, ANDLEQ, ORLEQ, NEGEQ, XOREQ, RIGHTEQ, LEFTEQ, PP, SS, 

    /* Equation Operators */
    PLUS, MINUS, DIV, MULT, POW, MOD, 

    /* Comparison Operators */
    EQQ, NEQQ, GEQQ, LEQQ, NOT, ANDL, ORL, GREAT, LESS, 

    /* Bitwise Operators */
    NEG, XOR, OR, AND, LSHIFT, RSHIFT, 

    /* Brackets */
    LPAREN, RPAREN, LBRACK, RBRACK, LBRACE, RBRACE, /* Langle and Rangle are < and > so less and great */

    /* Data Types */
    IDENT, INTEGRAL, DECIMAL, CLITERAL, SLITERAL,

    /* Memory */
    REGISTER, SREF, SMEM, QUESTION, REF, MEM,

    /* Other */
    SEMI, COLON, COMMA, HASH, COMMENT, END, ERR,

    /* Currently Not Determinable */
    NA

} TokenType;

#define INIT_LEXEME 2
#define MAX_LEXEME 65536

/* Helper Structures */
typedef struct Lexeme {
    char* word;
    int max;
    int size;
} Lexeme;

typedef struct Token {
    TokenType type;
    Lexeme lex;
    int line, col;
} Token;

/* ---------- Determing Exprtype --------- */

static const TokenType KEYWORDS[] = { IF, ELIF, ELSE, DO, WHILE, FOR, SWITCH, CASE, DEFAULT, RET, CHAR, SHORT, INT, FLOAT, DOUBLE, LONG, VOID, STRING };
static const size_t KEYWORDS_COUNT = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);

static const TokenType ASSIGNS[] = { EQ, PEQ, SEQ, MEQ, DEQ, MODEQ, LEFTEQ, RIGHTEQ, ANDEQ, OREQ, XOREQ, NEGEQ, ORLEQ, ANDLEQ };
static const size_t ASSIGNS_COUNT = sizeof(ASSIGNS) / sizeof(ASSIGNS[0]);

static const TokenType RELATIONAL[] = { GREAT, LESS, LEQQ, GEQQ };
static const size_t RELATIONAL_COUNT = sizeof(RELATIONAL) / sizeof(RELATIONAL[0]);

static const TokenType ADDS[] = { PLUS, MINUS };
static const size_t ADDS_COUNT = sizeof(ADDS) / sizeof(ADDS[0]);

static const TokenType MULTS[] = { MULT, DIV, MOD };
static const size_t MULTS_COUNT = sizeof(MULTS) / sizeof(MULTS[0]);

static const TokenType PREFIXS[] = { PP, SS, NOT, NEG, MINUS, MULT, AND };
static const size_t PREFIXS_COUNT = sizeof(PREFIXS) / sizeof(PREFIXS[0]);

static const TokenType POSTFIXS[] = { PP, SS, REGISTER, SMEM, SREF, MEM };
static const size_t POSTFIXS_COUNT = sizeof(POSTFIXS) / sizeof(POSTFIXS[0]);

static const TokenType PRIMARYS[] = { IDENT, DECIMAL, INTEGRAL, SLITERAL, CLITERAL };
static const size_t PRIMARYS_COUNT = sizeof(PRIMARYS) / sizeof(PRIMARYS[0]);

static const TokenType TYPES[] = { CHAR, SHORT, INT, FLOAT, DOUBLE, LONG, BOOL, VOID, STRING };
static const size_t TYPES_COUNT = sizeof(TYPES) / sizeof(TYPES[0]);

#endif
