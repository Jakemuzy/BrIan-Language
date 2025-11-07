#include "Tokenizer.h"
#include "Dictionary.h"

#define ERRT -1      /* Token with err */
#define NAT 0       /* Not a token */
#define VALID 1

/* KNOWN BUGS */

/*
 * TODO: Keywords, Idents, Factorial, 
 *       Order of checks 
*/

/* Jankiest Map I've seen but temporary */
KeyVal kv1 = {"if", IF} ;
KeyVal kv2 = {"elif", ELIF};
KeyVal kv3 = {"else", ELSE};
KeyVal kv4 = {"do", DO};
KeyVal kv5 = {"while", WHILE};
KeyVal kv6 = {"for", FOR};
KeyVal kv7 = {"char", CHAR};
KeyVal kv8 = {"short", SHORT};
KeyVal kv9 = {"int", INT};
KeyVal kv10 = {"double", DOUBLE};	
KeyVal kv11 = {"long", LONG};
static Dict* KWmap;

Token GetNextToken(FILE* fptr)
{
    /*  ORDER OF OERATIONS */
    /* Is Unary -> Is Comparison -> Is Comment -> ... */

    Token next;
    next.lex.size = 0;
    next.lex.max = INIT_LEXEME;
    
    int c;
    do {
        c = fgetc(fptr);
    } while(isspace(c));

    if(IsEnd(fptr, &next, c) != NAT)
        ;
    else if(IsNumber(fptr, &next, c) != NAT)
        ;
    else if(IsLiteral(fptr, &next, c) != NAT)
        ;
    else if(IsOperator(fptr, &next, c) != NAT)
        ;
    else if(IsComparison(fptr, &next, c) != NAT)
        ;
    else if(IsBracket(fptr, &next, c) != NAT)
        ;
    else if(IsBitwise(fptr, &next, c) != NAT)
        ;
    else if(IsUnary(fptr, &next, c) != NAT)
        ;
    else if(IdentOrKeyword(fptr, &next, c) != NAT)
        ;

    UpdateLexeme(&next, '\0');  /* Null Terminator */
    return next;
}

void UpdateLexeme(Token* t, int c)
{
    int size = t->lex.size;
    if (size == t->lex.max)
        t->lex.max *= 2;

    t->lex.word[size-1] = c;
    t->lex.size++;
}

/* ---------- CATEGORIES ---------- */

int IsOperator(FILE* fptr, Token* t, int c)
{
    /* TODO: Make this a switch stmt */

    if(IsDiv(fptr, t, c) != NAT)
        return VALID;
    else if(IsPlus(fptr, t, c) != NAT)
        return VALID;
    else if(IsMinus(fptr, t, c) != NAT)
        return VALID;
    else if(IsMult(fptr, t, c) != NAT)
        return VALID;
    else if(IsMod(fptr, t, c) != NAT)
        return VALID;
    else if(IsEqual(fptr, t, c) != NAT)  
        return VALID;

    /* TODO: account for ERR */
    return NAT;
}

int IsNumber(FILE* fptr, Token* t, int c)
{
    bool decimalSeen = false;

    if(isdigit(c))
        t->type = INTEGRAL;
    else if(c == '.')
    {
        t->type = DECIMAL;
        decimalSeen = true;
    }
    else 
        return NAT;

    UpdateLexeme(t, c);
    while(t->type == INTEGRAL || t->type == DECIMAL)
    {
        int next = fgetc(fptr);
        if(isdigit(next))
            ;
        else if (next == '.' && !decimalSeen)
        {
            t->type = DECIMAL;
            decimalSeen = true;
        }
        else if (next == '.')
            return ERRT;     /* Don't even finish */
        else if (isspace(next))
            break;
        else if (isalpha(next))
        {
            /* TODO: Need to send to an IsIdent function if ANY function returns Ident to check next chars */
            ungenc(next, fptr);
            t->type = NA;
            break;
        }
        else 
        {
            ungetc(next, fptr);
            break;
        }

        UpdateLexeme(t, next);
    }
    return VALID;
}

int IsLiteral(FILE* fptr, Token* t, int c)
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
                t->type = ERR;
                return ERRT;
            }
        }
        t->type = CLITERAL;
        return VALID;
    }
    else if(c == '\"')
    {
        int next;
        while((next = fgetc(fptr)) != '\"')
        {
            if(next == EOF)
            {
                printf("ERROR: EOF reached before string literal ended\n");
                t->type = ERR;
                return ERRT;
            }
        }
        t->type = SLITERAL;
        return VALID;
    }

    t->type = NA;
    return NAT;
}

int IsBracket(FILE* fptr, Token* t, int c)
{
    switch (c)
    {
        case '[':
            t->type = LBRACE;
            break;
        case ']':
            t->type = RBRACE;
            break;
        case '{':
            t->type = LBRACK;
            break;
        case '}':
            t->type = RBRACK;
            break;
        case '(':
            t->type = LPAREN;
            break;
        case ')':
            t->type = RPAREN;
            break;
        case '<':
            t->type = LANGLE;
            break;
        case '>':
            t->type = RANGLE;
            break;
        default:
            t->type = NA;
            return NA;
    }
    return VALID;
}

int IsComparison(FILE* fptr, Token* t, int c)
{

    int next = fgetc(fptr);
    if(c == '!')
    {
        if(next == '=') {
            t->type = NEQQ;
            return VALID;
        }

        ungetc(next, fptr);
        t->type = NOT;
        return VALID;
    }
    else if(c == '>')
    {
        if(next == '=') {
            t->type = GEQQ;
            return VALID;
        }
        else if(next == '>') {
            t->type = RSHIFT;
            return VALID;
        }

        ungetc(next, fptr);
        t->type = GREAT;
        return VALID;
    }
    else if(c == '<')
    {
        if(next == '=') {
            t->type = LEQQ;
            return VALID;
        }
        else if(next == '<') {
            t->type = LSHIFT;
            return VALID;
        }

        ungetc(next, fptr);
        t->type = LESS;
        return VALID;
    }
    else if(c == '&')
    {
        if(next == '&') {
            t->type = ANDL;
            return VALID;
        }

        ungetc(next, fptr);
        t->type = AND;
        return VALID;     /* Bitwise */
    }
    else if(c == '|')
    {
        if(next == '|'){
            t->type = ORL;
            return VALID;
        }

        ungetc(next, fptr);
        t->type = OR;
        return VALID;
    }

    t->type = NA;
    return NAT;
}

int IsBitwise(FILE* fptr, Token* t, int c)
{
    if(c == '~') {
        t->type = NEG;
        return NEG;
    }
    else if(c == '^') {
        t->type = XOR;
        return XOR;
    }

    t->type = NA;
    return NAT;
}

int IsUnary(FILE* fptr, Token* t, int c)
{
    
    if(c == ';') {
        t->type = SEMI;
        return VALID;
    }
    if(c == ':') {
        t->type = COLON;
        return VALID;
    }

    t->type = NA;
    return NAT;
}

/* Others */

int IsEnd(FILE* fptr, Token* t, int c)
{
    if(c == EOF) {
        t->type = END;
        return VALID;
    }

    t->type = NA;
    return NAT;
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

/* ---------- OPERATORS ---------- */

/* = and == */
int IsEqual(FILE* fptr, Token* t, int c)
{
    if(c == '=')
    {
        int next = fgetc(fptr);
        if(next == '=') {
            t->type = EQQ;
            return VALID;
        }
        
        ungetc(next, fptr);
        t->type = EQ;
        return VALID; 
    }
    t->type = NA;
    return NAT;
}

/* +, +=, ++ */
int IsPlus(FILE* fptr, Token* t, int c)
{
    if(c == '+')
    {
        int next = fgetc(fptr);
        if(next == '=') {
            t->type = PEQ;
            return VALID;
        }
        else if(next == '+') {
            t->type = PP;
            return VALID;  
        }

        ungetc(next, fptr);
        t->type = PLUS;
        return VALID;
    }

    t->type = NA;
    return NAT;
}

int IsMinus(FILE* fptr, Token* t, int c)
{
    if(c == '-')
    {
        int next = fgetc(fptr);
        if(next == '=') {
            t->type = SEQ;
            return VALID;
        }
        else if(next == '-') {
            t->type = SS;
            return VALID;
        }

        ungetc(next, fptr);
        t->type = MINUS;
        return MINUS;
    }
    t->type = NA;
    return NAT;
}

int IsDiv(FILE* fptr, Token* t, int c)
{

    if(c == '/')
    {
        int next = fgetc(fptr);
        if(next == '=') {
            t->type = DEQ;
            return VALID;
        }
        else if (IsComment(fptr, t, next) == COMMENT) {
            t->type = COMMENT;
            return VALID; 
        }

        ungetc(next, fptr);

        t->type = DIV;
        return VALID;    
    }
    t->type = NA;
    return NAT;
}

int IsMult(FILE* fptr, Token* t, int c)
{

    if(c == '*')
    {
        int next = fgetc(fptr);
        if(next == '=') {
            t->type = MEQ;
            return VALID;
        }
        else if(next == '*') {
            t->type = POW;
            return VALID;
        }

        ungetc(next, fptr);
        t->type = MULT;
        return VALID;
    }
    t->type = NA;
    return NAT;
}

int IsMod(FILE* fptr, Token* t, int c)
{

    if(c == '%')
    {
        int next = fgetc(fptr);
        if(next == '=') {
            t->type = MODEQ;
            return VALID;
        }

        ungetc(next, fptr);
        t->type = MOD;
        return MOD;
    }
    t->type = NA;
    return NAT;
}

/* ---------- OTHERS ---------- */

int IsComment(FILE* fptr, Token* t, int c)
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
                t->type = ERR;
                return ERRT;
            }

            prev = next;
        }
        t->type = COMMENT;
        return VALID;
    } 
    else if (next == '/')
    {
        /* Add EOF check */
        while((next = fgetc(fptr)) != '\n')
        {
            if(next == EOF)
                break;
        }
        t->type = COMMENT;
        return VALID;
    }
    t->type = NA;
    return VALID;
}


int IdentOrKeyword(FILE* fptr, Token* t, int c)
{
    /* TODO: Fix this janky ass MAP creation, make static 

    KWmap = DictMake(11, kv1, kv2, kv3, kv4, kv5, kv6, kv7, kv8, kv9, kv10, kv11);


    TokenType tok = (isalpha(c)) ? IDENT : NA;
    while(isalpha(c = fgetc(fptr)) || c == '_' || isdigit(c))
        ; 

    return (DictLookup(d */
    return VALID;
}

