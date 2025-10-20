#include "Tokenizer.h"

Token GetNextToken(FILE* fptr)
{
    Token next;
    next.type = IDENT;

    int c = fgetc(fptr);

    if((next.type = IsNumber(fptr, c)) != NA)
        ;
    else if((next.type = IsLiteral(fptr, c)) != NA)
        ;
    else if((next.type = IsEqual(fptr, c)) != NA)
        ;
    else if((next.type = IsPlus(fptr, c)) != NA)
        ;
    else if((next.type = IsMinus(fptr, c)) != NA)
        ;
    else if((next.type = IsDiv(fptr, c)) != NA) 
        ;
    else if((next.type = IsMult(fptr, c)) != NA)
        ;
    else if((next.type = IsMod(fptr, c)) != NA)
        ;

    return next;
}

/* ---------- CATEGORIES ---------- */

TokenType IsOperator(FILE* fptr, int c)
{
}

TokenType IsNumber(FILE* fptr, int c)
{
    TokenType state;
    bool decimalSeen = false;

    if(isdigit(c))
        state = INTEGRAL;
    else if(c == '.')
    {
        state = DECIMAL;
        decimalSeen = true;
    }
    else 
    {
        return NA;
    }

    while(state == INTEGRAL || state == DECIMAL)
    {
        int next = fgetc(fptr);
        if(isdigit(next))
            ;
        else if (next == '.' && !decimalSeen)
        {
            state = DECIMAL;
            decimalSeen = true;
        }
        else if (next == '.')
            return ERR;     /* Don't even finish */
        else if (isspace(next))
            break;
        else if (isalpha(next))
        {
            /* Need to send to an IsIdent function if ANY function returns Ident to check next chars */
            state = IDENT;
            break;
        }

    }
    return state;
}

TokenType IsLiteral(FILE* fptr, int c)
{
    if(c == '\'')
    {
        int next = fgetc(fptr);
        if(next != EOF);
        {
            next = fgetc(fptr);
            if(next != '\'')
            {
                printf("ERROR: Cannot have char literal with length > 1\n");
                return ERR;
            }
        }
        return CLITERAL;
    }
    else if(c == '\"')
    {
        int next;
        while((next = fgetc(fptr) != '\"'))
        {
            if(next == EOF)
            {
                printf("ERROR: EOF reached before string literal ended\n");
                return ERR;
            }
        }
        return SLITERAL;
    }

    return NA;

}

TokenType IsEnd(FILE* fptr, int c)
{
    if(c == EOF)
        return END;
    return NA;
}


/* ---------- OPERATORS ---------- */

/* = and == */
TokenType IsEqual(FILE* fptr, int c)
{
    if(c == '=')
    {
        int next = fgetc(fptr);
        if(next == '=')
            return EQQ;
        
        ungetc(next, fptr);
        return EQ; 
    }
    return NA;
}

/* +, +=, ++ */
TokenType IsPlus(FILE* fptr, int c)
{
    if(c == '+')
    {
        int next = fgetc(fptr);
        if(next == '=')
            return PEQ;
        else if(next == '+')
            return PP;  

        ungetc(next, fptr);
        return PLUS;
    }
    return NA;
}

TokenType IsMinus(FILE* fptr, int c)
{

    if(c == '-')
    {
        int next = fgetc(fptr);
        if(next == '=')
            return SEQ;
        else if(next == '-')
            return SS;

        ungetc(next, fptr);
        return MINUS;
    }
    return NA;
}

TokenType IsDiv(FILE* fptr, int c)
{

    if(c == '/')
    {
        int next = fgetc(fptr);
        if(next == '=')
            return DEQ;
        else if (IsComment(fptr, next) == COMMENT)
            return COMMENT; 

        ungetc(next, fptr);
        return DIV;    
    }
    return NA;
}

TokenType IsMult(FILE* fptr, int c)
{

    if(c == '*')
    {
        int next = fgetc(fptr);
        if(next == '=')
            return MEQ;
        else if(next == '*')
            return POW;

        ungetc(next, fptr);
        return MULT;
    }
    return NA;
}

TokenType IsMod(FILE* fptr, int c)
{

    if(c == '%')
    {
        int next = fgetc(fptr);
        if(next == '=')
            return MODEQ;

        ungetc(next, fptr);
        return MOD;
    }
    return NA;
}

/* ---------- OTHERS ---------- */

TokenType IsComment(FILE* fptr, int c)
{
    /* Only called in IsDiv so first char already div */
    int next = c;
    int prev;
    if(next == '*')
    {
        while((next = fgetc(fptr)) != '/' && prev != '*')
        {
            if(next == EOF)
            {
                printf("ERROR: Comment doesn't end before EOF reached\n");
                return ERR;
            }

            prev = next;
        }
        return COMMENT;
    } 
    else if (next == '/')
    {
        /* Add EOF check */
        while((next = fgetc(fptr)) != '\n')
        {
            if(next == EOF)
                break;
        }
        return COMMENT;
    }
    return NA;
}

/*
TokenType IdentOrKeyword(Token t)
{
    for(i = 0; i < KeywordMap.size(); i++)
    {
        Token kw = KeywordMap.At(i);
        if(word == kw)
        {
            return kw;
        }
    }
    return IDENTIFIER;

}
*/
