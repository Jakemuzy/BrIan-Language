#include "Tokenizer.h"

/* KNOWN BUGS */

/*
 * TODO: Keywords, Idents, Factorial, 
 *       Order of checks 
*/


Token GetNextToken(FILE* fptr)
{
    /*  ORDER OF OERATIONS */
    /* Is Unary -> Is Comparison -> Is Comment -> ... */

    Token next;
    next.type = NA;
    
    int c = fgetc(fptr);
 
    if((next.type = IsEnd(fptr, c)) != NA)
        ;
    else if((next.type = IsNumber(fptr, c)) != NA)
        ;
    else if((next.type = IsLiteral(fptr, c)) != NA)
        ;
    else if((next.type = IsOperator(fptr, c)) != NA)
        ;
    else if((next.type = IsBracket(fptr, c)) != NA)
        ;
    else if((next.type = IsComparison(fptr, c)) != NA)
        ;
    else if((next.type = IsBitwise(fptr, c)) != NA)
        ;
    else if((next.type = IdentOrKeyword(fptr, c)) != NA)
        ;

    if(next.type == NA)
    {
        printf("%c ", c);
        /* Check for ident or KW */
    }


    return next;
}

/* ---------- CATEGORIES ---------- */

TokenType IsOperator(FILE* fptr, int c)
{
    /* TODO: Make this a switch stmt */
    TokenType type = NA;

    if((type = IsDiv(fptr, c)) != NA)
        ;
    else if((type = IsPlus(fptr, c)) != NA)
        ;
    else if((type = IsMinus(fptr, c)) != NA)
        ;
    else if((type = IsEqual(fptr, c)) != NA) 
        ;
    else if((type = IsMult(fptr, c)) != NA)
        ;
    else if((type = IsMod(fptr, c)) != NA)
        ;
    return type;
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
            /* TODO: Need to send to an IsIdent function if ANY function returns Ident to check next chars */
            state = IDENT;
            break;
        }
        else 
        {
            ungetc(next, fptr);
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
        if(next != EOF)
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
        while((next = fgetc(fptr)) != '\"')
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

TokenType IsBracket(FILE* fptr, int c)
{
    switch (c)
    {
        case '[':
            return LBRACE;
        case ']':
            return RBRACE;
        case '{':
            return LBRACK;
        case '}':
            return RBRACK;
        case '(':
            return LPAREN;
        case ')':
            return RPAREN;
        case '<':
            return LANGLE;
        case '>':
            return RANGLE;
        default:
            return NA;
    }
}

TokenType IsComparison(FILE* fptr, int c)
{

    int next = fgetc(fptr);
    if(c == '!')
    {
        if(next == '=')
            return NEQQ;

        ungetc(next, fptr);
        return NOT;
    }
    else if(c == '>')
    {
        if(next == '=')
            return GEQQ;
        else if(next == '>')
            return RSHIFT;

        ungetc(next, fptr);
        return GREAT;
    }
    else if(c == '<')
    {
        if(next == '=')
            return LEQQ;
        else if(next == '<')
            return LSHIFT;

        ungetc(next, fptr);
        return LESS;
    }
    else if(c == '&')
    {
        if(next == '&')
            return ANDL;

        ungetc(next, fptr);
        return AND;     /* Bitwise */
    }
    else if(c == '|')
    {
        if(next == '|')
            return ORL;

        ungetc(next, fptr);
        return OR;
    }

    return NA;
}

TokenType IsBitwise(FILE* fptr, int c)
{
    if(c == '~')
        return NEG;
    else if(c == '^')
        return XOR;

    return NA;
}

/* Others */

TokenType IsEnd(FILE* fptr, int c)
{
    if(c == EOF)
        return END;
    return NA;
}
/* Just eats EOL for now */
/*
TokenType IsEOL(FILE* fptr, int c)
{
    if(c != '\n')
        ;
    return NA;
}
*/

TokenType IsSemi(FILE* fptr, int c)
{
    return c == ';' ? SEMI : NA;
}

TokenType IsColon(FILE* fptr, int c)
{
    return c == ':' ? COLON : NA;
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
    int prev = 0;
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


TokenType IdentOrKeyword(FILE* fptr, int c)
{
    TokenType tok = (isalpha(c)) ? IDENT : NA;

    while(isalpha(c = fgetc(fptr)) || c == '_' || isdigit(c))
        ; 

    /*
    TODO: Add kw checking
    for(i = 0; i < KeywordMap.size(); i++)
    {
        Token kw = KeywordMap.At(i);
        if(word == kw)
        {
            return kw;
        }
    }
    */
    return tok;

}

