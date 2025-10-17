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
    FACTORIAL, /* Also NOT */

    /* Brackets */
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACK, RBRACK,

    /* Data Types */
    INTEGRAL, DECIMAL,

    /* Other */
    SEMI, COMMENT, ERR 
} TokenType;

/* Helper Structures */
typedef struct {
    TokenType type;
    char* lexeme;
    int line, col;
} Token;


#endif
