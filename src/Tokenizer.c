#include "Tokenizer.h"

Token GetNextToken(FILE* fptr)
{
    Token next;
    next.type = IDENT;

    int c;
    while((c = fgetc(fptr) != EOF))
    {
        if((next.type = IsComment(fptr)) != ERR)
            break;
    }
    
    return next;
}

/* = and == */
TokenType IsEqual(FILE* fptr, int c)
{
    TokenType state = ERR;
    if(c == '=')
         {
             char next = fgetc(file);
             state = EQ;
             if(next == '=')
             {
                 state = EQQ;
             }
             else 
             {
                 ungetc(next, file);
             }
         }
    }
    return state;
}

/* + - / * ** % and their eqq versions */
TokenType IsOperator(FILE* fptr, int c)
{
    
    TokenType state;
}

TokenType IsComment(FILE* fptr, int c)
{
    /* Only called in IsDiv so first char already div */
    int next = c;
    int prev = '';
    if(next == '*')
    {
        while((next = fgetc(file)) != '/' && prev != '*')
        {
            if(next == EOF)
                return ERR; /* ERR 1 comment doesnt end */

            prev = next;
        }
    } 
    else if (next == '/')
    {
        /* Add EOF check */
        while((next = fgetc(file)) != '\n')
        {
            if(next == EOF)
                break;
        }
    }
    return COMMENT;
}


/* More Operators */
TokenType IsPlus(FILE* fptr, int c)
{
    if(c == '+')
    {
        char next = fgetc(fptr);
        if(next == '=')
            return PEQ;
        else if(next == '+')
            return PP;  

        ungetc(next, file);
        return PLUS;
    }
    return ERR;
}

TokenType IsMinus(FILE* fptr, int c)
{

    if(c == '-')
    {
        char next = fgetc(fptr);
        if(next == '=')
            return SEQ;
        else if(next == '-')
            return SS;

        ungetc(next, file);
        return MINUS;
    }
    return ERR;
}

TokenType IsDiv(FILE* fptr, int c)
{

    if(c == '/')
    {
        char next = fgetc(file);
        if(next == '=')
            return DEQ;
        else if (IsComment(next))
            return COMMENT; 
        ungetc(next, file);
        return DIV    
    }
    return ERR;
}

TokenType IsMult(FILE* fptr, int c)
{

    if(c == '*')
    {
        char next = fgetc(file);
        state = MULT;
        if(next == '=')
            return MEQ;
        else if(next == '*')
            return POW;

        ungetc(next, file);
        return MULT;
    }
    return ERR;
}

TokenType IsMod(FILE* fptr, int c)
{

    if(c == '%')
    {
        char next = fgetc(file);
        if(next == '=')
            state = MODEQ;

        ungetc(next, file);
        return MOD;
    }
    return ERR;
}
