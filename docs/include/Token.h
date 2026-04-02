#ifndef _BRIAN_TOKEN_H_ 
#define _BRIAN_TOKEN_H_

#include <stdlib.h>

#define TOKEN_MAX_LENGTH

/*   BrIan Tokens and TokenTypes 
    ----------------------------
      Refer to docs/tokens.md 
           for details.   
*/

typedef enum {
    /* Control Flow Keywords */
    IF, ELIF, ELSE, DO, WHILE, FOR, SWITCH,
    CASE, DEFAULT, BREAK, CONTINUE, RETURN,

    /* Concurrency Keywords */
    LOCK, CRITICAl, SPAWN, AWAIT, 

    /* Data Type Keywords */
    CHAR, BOOL, INT, FLOAT, DOUBLE, LONG, 
    I8, I16, I32, I64, U8, U16, U32, U64,
    MUTEX, SEMAPHORE, TASK, CHANNEL,
    VOID, STRING, 

    /* Custom Types Keywords */
    ENUM, STRUCT, TYPEDEF, INTERFACE, OPERATOR, LAMBDA,

    /* Preprocessor Directive Keywords */
    IMPORT, ENDIF, PRAGMA, ERROR, 

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
    NEG, XOR, OR, AND, LSHFIT, RSHIFT,

    /* Structures Operators */
    SREF, SMEM, REF, MEM,

    /* Other Operators */
    DOTPROD, QUESTION, COLON, SEND, SEMI, COMMA, HASH,

    /* Brackets */
    LPAREN, RPAREN, LBRACK, RBRACK, LBRACE, RBRACE, 

    /* Errors */
    NA, ERR

} TokenType;

typedef struct {
    TokenType type; 
    int row, col;

    char* lexeme;
    size_t current, max;
} Token;

#endif