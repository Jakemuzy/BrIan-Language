#ifndef _TOKEN_H__
#define _TOKEN_H__

/* TODO: Add
    BOOL - bool keyword 
    QUES - (?) 

    SMEM - Safe member access ?.
    SREF - Safe member access by ref ?->
    SPLICE - Array splicing and Ternary op :

    REG - Regsiter access @

    BCLEAR - Bitwise clear &<2>
    BSET - Bitwise set |<3>

    CCONST - Char
    SCONST - Short
    ICONST - Int
    FCONST - Float
    DCONST - Double
    LCONST - Long
    RCONST - String 
*/

typedef enum {
    /* Ctrl Stmt Key Words */
    IF, ELIF, ELSE, DO, WHILE, FOR, SWITCH, CASE, DEFAULT,

    /* Data Type Key Words */
    IDENT, CHAR, SHORT, INT, FLOAT, DOUBLE, LONG, VOID,

    /* Assignment Operators */
    EQ, PEQ, SEQ, MEQ, DEQ, MODEQ, ANDEQ, OREQ, ANDLEQ, ORLEQ, NEGEQ, XOREQ, RIGHTEQ, LEFTEQ, PP, SS, 
    /* Equation Operators */
    PLUS, MINUS, DIV, MULT, POW, MOD, 

    /* Comparison Operators */
    EQQ, NEQQ, GEQQ, LEQQ, NOT, ANDL, ORL, GREAT, LESS, 

    /* Bitwise Operators */
    NEG, XOR, OR, AND, LSHIFT, RSHIFT, 

    /* Brackets */
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACK, RBRACK, /* Langle and Rangle are < and > so less and great */

    /* Data Types REPLACE WITH CONST */
    INTEGRAL, DECIMAL,

    /* Memory */
    TERN, COND_ACCESS, CONDP_ACCESS, REG, /* TODO: ?. ?-> tern operator and register access */

    /* Other */
    SEMI, COLON, HASH, COMMENT, CLITERAL, SLITERAL, END, ERR,

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

static const int KEYWORDS[] = { IF, ELIF, ELSE, DO, WHILE, FOR, SWITCH, CHAR, SHORT, INT, FLOAT, DOUBLE, LONG, VOID };
static const size_t KEYWORDS_COUNT = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);

static const int ASSIGNS[] = { EQ, PEQ, SEQ, MEQ, DEQ, MODEQ, LEFTEQ, RIGHTEQ, ANDEQ, OREQ, XOREQ, NEGEQ, ORLEQ, ANDLEQ };
static const size_t ASSIGNS_COUNT = sizeof(ASSIGNS) / sizeof(ASSIGNS[0]);

static const int RELATIONAL[] = { GREAT, LESS, LEQQ, GEQQ };
static const size_t RELATIONAL_COUNT = sizeof(RELATIONAL) / sizeof(RELATIONAL[0]);

static const int ADDS[] = { PLUS, MINUS };
static const size_t ADDS_COUNT = sizeof(ADDS) / sizeof(ADDS[0]);

static const int MULTS[] = { MULT, DIV, MOD };
static const size_t MULTS_COUNT = sizeof(MULTS) / sizeof(MULTS[0]);

static const int PREFIXS[] = { POW, PP, SS, NOT, NEG, /* Cast */ MULT, AND };
static const size_t PREFIXS_COUNT = sizeof(PREFIXS) / sizeof(PREFIXS[0]);

static const int POSTFIXS[] = { PP, SS, REG };
static const size_t POSTFIXS_COUNT = sizeof(POSTFIXS) / sizeof(POSTFIXS[0]);

static const TYPES[] = { CHAR, SHORT, INT, FLOAT, DOUBLE, LONG };
static const size_t TYPES_COUNT = sizeof(TYPES) / sizeof(TYPES[0]);

#endif
