#ifndef _STRUCTS_H__
#define _STRUCTS_H__

typedef enum {
    /* Ctrl Stmts */
    IF, ELIF, ELSE, DO, WHILE, FOR, 

    /* Assignment Operators */
    EQ, PEQ, SEQ, MEQ, DEQ, MODEQ, PP, SS,

    /* Equation Operators */
    PLUS, MINUS, DIV, MULT, POW, MOD, 

    /* Comparison Operators */
    EQQ, NEQQ, GEQQ, LEQQ, NOT, ANDL, ORL, GREAT, LESS, 

    /* Bitwise Operators */
    NEG, XOR, OR, AND, LSHIFT, RSHIFT, 

    /* Unary Operators */
    FACTORIAL, /* Also NOT */

    /* Brackets */
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACK, RBRACK,

    /* Data types */
    IDENT, CHAR, SHORT, INT, FLOAT, DOUBLE, LONG, 

    /* Other */
    SEMI, 
} TokenType;

/* Helper Structures */
typedef struct {
    TokenType type;
    char* lexeme;
    int line, col;
} Token;


#endif
