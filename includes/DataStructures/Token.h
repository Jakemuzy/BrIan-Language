#ifndef _STRUCTS_H__
#define _STRUCTS_H__

typedef enum {
    /* Ctrl Stmt Key Words */
    IF, ELIF, ELSE, DO, WHILE, FOR, 

    /* Data Type Key Words */
    IDENT, CHAR, SHORT, INT, FLOAT, DOUBLE, LONG, 

    /* Assignment Operators */
    EQ, PEQ, SEQ, MEQ, DEQ, MODEQ, PP, SS,

    /* Equation Operators */
    PLUS, MINUS, DIV, MULT, POW, MOD, 

    /* Comparison Operators */
    EQQ, NEQQ, GEQQ, LEQQ, NOT, ANDL, ORL, GREAT, LESS, 

    /* Bitwise Operators */
    NEG, XOR, OR, AND, LSHIFT, RSHIFT, 

    /* Unary Operators */
    /*FACT,  Also NOT */

    /* Brackets */
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACK, RBRACK, LANGLE, RANGLE,

    /* Data Types */
    INTEGRAL, DECIMAL,

    /* Other */
    SEMI, COLON, COMMENT, CLITERAL, SLITERAL, END, ERR,

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

typedef struct {
    TokenType type;
    Lexeme lex;
    int line, col;
} Token;


#endif
