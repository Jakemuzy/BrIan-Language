#ifndef _STRUCTS_H__
#define _STRUCTS_H__
    
typedef enum {
    /* Ctrl Stmts */
    IF, ELIF, ELSE, DO, WHILE, FOR, 

    /* Operators */
    EQ, PEQ, SEQ, MEQ, DEQ, MODEQ, PP, SS, MM,

    /* Comparison Operators */
    EQQ, NEQ, NOT,

    /* Unary Operators */
    PLUS, MINUS, FACTORIAL,

    /* Data types */
    IDENT, CHAR, SHORT, INT, FLOAT, DOUBLE, LONG, 
} TOKEN;

#endif
