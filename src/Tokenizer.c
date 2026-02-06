#include "Tokenizer.h"

#define ERRT -1      /* Token with err */
#define NAT 0       /* Not a token */
#define VALID 1

/* KNOWN BUGS */

/* TODO: 
        - Add Factorials, Register Access, Bit Manipulation and other Tokens
        - Add Support for line and column tracking for better debugging 
        - Refactor the Dictionary System to use something more elegant
        - Refactor the Entire Tokenizer to reduce boilerplate and make it more modular
        - Instead of checking for '\n' in each case, just put the token back and let the 
          main GetNextToken function handle the logic regarding line numbers
*/

/* ---------- Helpers ---------- */

void KWMapInit()
{
    KWmap = malloc(sizeof(Dict));
    *KWmap = DictInit(HASH_STR, 109);

    DictPush(KWmap, (void*) "if", (void*)(uintptr_t) IF);      DictPush(KWmap, (void*) "elif", (void*)(uintptr_t) ELIF);  
    DictPush(KWmap, (void*) "else", (void*)(uintptr_t) ELSE);    DictPush(KWmap, (void*) "do", (void*)(uintptr_t) DO);      
    DictPush(KWmap, (void*) "while", (void*)(uintptr_t) WHILE);  DictPush(KWmap, (void*) "break", (void*)(uintptr_t) BREAK);  
    DictPush(KWmap, (void*) "for", (void*)(uintptr_t) FOR);      DictPush(KWmap, (void*) "switch", (void*)(uintptr_t) SWITCH);
    DictPush(KWmap, (void*) "case", (void*)(uintptr_t) CASE);    DictPush(KWmap, (void*) "default", (void*)(uintptr_t) DEFAULT); 
    DictPush(KWmap, (void*) "return", (void*)(uintptr_t) RET);   DictPush(KWmap, (void*) "char", (void*)(uintptr_t) CHAR);     
    DictPush(KWmap, (void*) "short", (void*)(uintptr_t) SHORT);   DictPush(KWmap, (void*) "int", (void*)(uintptr_t) INT);      
    DictPush(KWmap, (void*) "float", (void*)(uintptr_t) FLOAT);   DictPush(KWmap, (void*) "double", (void*)(uintptr_t) DOUBLE); 
    DictPush(KWmap, (void*) "long", (void*)(uintptr_t) LONG);     DictPush(KWmap, (void*) "void", (void*)(uintptr_t) VOID);    
    DictPush(KWmap, (void*) "string", (void*)(uintptr_t) STRING); DictPush(KWmap, (void*) "enum", (void*)(uintptr_t) ENUM);    
    DictPush(KWmap, (void*) "struct", (void*)(uintptr_t) STRUCT); DictPush(KWmap, (void*) "const", (void*)(uintptr_t) CONST); 
    DictPush(KWmap, (void*) "signed", (void*)(uintptr_t) SIGNED);DictPush(KWmap, (void*) "unsigned", (void*)(uintptr_t) UNSIGNED);
    DictPush(KWmap, (void*) "static", (void*)(uintptr_t) STATIC);DictPush(KWmap, (void*) "typedef", (void*)(uintptr_t) TYPEDEF); 
    DictPush(KWmap, (void*) "U8", (void*)(uintptr_t) U8);        DictPush(KWmap, (void*) "U16", (void*)(uintptr_t) U16);     
    DictPush(KWmap, (void*) "U32", (void*)(uintptr_t) U32);      DictPush(KWmap, (void*) "U64", (void*)(uintptr_t) U64);     
    DictPush(KWmap, (void*) "I8", (void*)(uintptr_t) I8);        DictPush(KWmap, (void*) "I16", (void*)(uintptr_t) I16);     
    DictPush(KWmap, (void*) "I32", (void*)(uintptr_t) I32);      DictPush(KWmap, (void*) "I64", (void*)(uintptr_t) I64);     
    DictPush(KWmap, (void*) "bool", (void*)(uintptr_t) BOOL);

}

int GetLineNum() { return LINE_NUM; }
int CheckBuffer(Token* out)
{
    if (Buff.tokCount == 0)
        return NAT;
    *out = Buff.toks[--Buff.tokCount];

    return VALID;
}

Token GetNextToken(FILE* fptr)
{
    Token next;
    if (CheckBuffer(&next) != NAT)
        return next;

    /* Fetch new Token From File */
    next.lex.size = 0;
    next.lex.max = INIT_LEXEME;
    next.lex.word = malloc(next.lex.max * sizeof(char));
    next.lex.word[0] = '\0';
    
    int c;
    GET_CHAR(fptr, c);

    if (IsEnd(fptr, &next, c) != NAT)
        ;
    else if (IsNumber(fptr, &next, c) != NAT)
        ;
    else if (IsLiteral(fptr, &next, c) != NAT)
        ;
    else if (IsOperator(fptr, &next, c) != NAT)
        ;
    else if (IsComp(fptr, &next, c) != NAT)
        ;
    else if (IsBracket(fptr, &next, c) != NAT)
        ;
    else if (IsBitwise(fptr, &next, c) != NAT)
        ;
    else if (IsOther(fptr, &next, c) != NAT)
        ;
    else if (isalpha(c) || c == '_')
        IdentOrKeyword(fptr, &next, c);
    else 
        printf("ERROR: Unknown char: %c\n", c);

    return next;
}

int PutTokenBack(const Token* t)
{
    Token* tmp = realloc(Buff.toks, sizeof(Token) * (Buff.tokCount + 1));
    if (!tmp)
        return NAT;

    Buff.toks = tmp;
    Buff.toks[Buff.tokCount] = *t; 
    Buff.tokCount++;

    return VALID;
}

void UpdateLexeme(Token* t, int c)
{
    if (t->lex.size + 1 >= t->lex.max)      /* +1 for null terminator */
    {
        t->lex.max *= 2;
        t->lex.word = realloc(t->lex.word, t->lex.max * sizeof(char));
    }

    t->lex.word[t->lex.size] = c;
    t->lex.size++;
    t->lex.word[t->lex.size] = '\0'; 
    t->line = LINE_NUM;
}

/* ---------- CATEGORIES ---------- */

int IsOperator(FILE* fptr, Token* t, int c)
{

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

    if(isdigit(c)) {
        UpdateLexeme(t, c);
        t->type = INTEGRAL;
    }
    else if(c == '.')
    {
        UpdateLexeme(t, c);

        int next = fgetc(fptr);
        if(next == '?') {
            t->type = SMEM;
            UpdateLexeme(t, next);
            return VALID;
        }
        else if (isdigit(next)) {
            t->type = DECIMAL;
            UpdateLexeme(t, next);
            decimalSeen = true;
        } 
        else {
            ungetc(next, fptr);
            t->type = MEM;
            return VALID;
        }

    }
    else 
        return NAT;

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
        else if (isspace(next)) {
            if (next == '\n') LINE_NUM++;
            break;
        }
        else if (isalpha(next))
        {
            /* TODO: Need to send to an IsIdent function if ANY function returns Ident to check next chars */
            ungetc(next, fptr);
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
        UpdateLexeme(t, c);

        int next = fgetc(fptr);
        UpdateLexeme(t, next);
        if(next != EOF)
        {
            next = fgetc(fptr);
            if(next != '\'')
            {
                printf("ERROR: Cannot have char literal with length > 1\n");
                t->type = ERR;
                return ERRT;
            }
            
            UpdateLexeme(t, next);
        }
        t->type = CLITERAL;
        return VALID;
    }
    else if(c == '\"')
    {
        UpdateLexeme(t, c);
        int next;
        while((next = fgetc(fptr)) != '\"')
        {
            if(next == EOF)
            {
                printf("ERROR: EOF reached before string literal ended\n");
                t->type = ERR;
                return ERRT;
            }
            UpdateLexeme(t, next);
        }
        UpdateLexeme(t, next);
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
            t->type = LBRACK;       /* I accidentally switched up braces and brackets OOPS */
            break;
        case ']':
            t->type = RBRACK;
            break;
        case '{':
            t->type = LBRACE;
            break;
        case '}':
            t->type = RBRACE;
            break;
        case '(':
            t->type = LPAREN;
            break;
        case ')':
            t->type = RPAREN;
            break;
        default:
            t->type = NA;
            return NAT;
    }
    
    UpdateLexeme(t, c);
    return VALID;
}

int IsComp(FILE* fptr, Token* t, int c)
{
    
    if(IsNeqq(fptr, t, c) != NAT)
        return VALID;
    else if(IsGeqq(fptr, t, c) != NAT)
        return VALID;
    else if(IsLeqq(fptr, t, c) != NAT)
        return VALID;
    else if(IsAndl(fptr, t, c) != NAT)
        return VALID;
    else if(IsOrl(fptr, t, c) != NAT)
        return VALID;

    t->type = NA;
    return NAT;
}

int IsBitwise(FILE* fptr, Token* t, int c)
{
    if(c == '~') {
        UpdateLexeme(t, c);
        int next = fgetc(fptr);
        if(next == '=')
        {
            t->type = NEGEQ;
            UpdateLexeme(t, next);
            return VALID;
        }
            
        ungetc(next, fptr);
        t->type = NEG;
        return VALID;
    }
    else if(c == '^') {
        UpdateLexeme(t, c);
        int next = fgetc(fptr);
        if(next == '=')
        {
            t->type = XOREQ;
            UpdateLexeme(t, next);
            return VALID;
        }

        ungetc(next, fptr);
        t->type = XOR;
        return VALID;
    }

    t->type = NA;
    return NAT;
}

int IsOther(FILE* fptr, Token* t, int c)
{
    
    if(c == ';') {
        t->type = SEMI;
        UpdateLexeme(t, c);
        return VALID;
    }
    if(c == ':') {
        t->type = COLON;
        UpdateLexeme(t, c);
        return VALID;
    }
    if(c == '#'){
        t->type = HASH;
        UpdateLexeme(t, c);
        return VALID;
    }
    if(c == ','){
        t->type = COMMA;
        UpdateLexeme(t, c);
        return VALID;
    }
    if(c == '@'){
        t->type = REGISTER;
        UpdateLexeme(t, c);
        return VALID;
    }
    if(c == '?') {
        t->type = QUESTION;
        UpdateLexeme(t, c);
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
        UpdateLexeme(t, c);
        return VALID;
    }

    t->type = NA;
    return NAT;
}

/* ---------- OPERATORS ---------- */

/* = and == */
int IsEqual(FILE* fptr, Token* t, int c)
{
    if(c == '=')
    {
        UpdateLexeme(t, c);

        int next = fgetc(fptr);
        if(next == '=') {
            t->type = EQQ;
            UpdateLexeme(t, next);
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
        UpdateLexeme(t, c);

        int next = fgetc(fptr);
        if(next == '=') {
            t->type = PEQ;
            UpdateLexeme(t, next);
            return VALID;
        }
        else if(next == '+') {
            t->type = PP;
            UpdateLexeme(t, next);
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
        UpdateLexeme(t, c);

        int next = fgetc(fptr);
        if(next == '=') {
            t->type = SEQ;
            UpdateLexeme(t, next);
            return VALID;
        }
        else if(next == '-') {
            t->type = SS;
            UpdateLexeme(t, next);
            return VALID;
        }
        else if(next == '>') {      /* Accessing Pointers */
            UpdateLexeme(t, next);

            next = fgetc(fptr);
            if (next == '?') {
                t->type = SREF;
                UpdateLexeme(t, next);
                return VALID;
            }

            ungetc(next, fptr);
            t->type = REF;
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
        UpdateLexeme(t, c);

        int next = fgetc(fptr);
        if(next == '=') {
            t->type = DEQ;
            UpdateLexeme(t, next);
            return VALID;
        }
        else if (IsComm(fptr, t, next) != NAT) 
            return VALID; 

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
        UpdateLexeme(t, c);

        int next = fgetc(fptr);
        if(next == '=') {
            t->type = MEQ;
            UpdateLexeme(t, next);
            return VALID;
        }
        else if(next == '*') {
            t->type = POW;
            UpdateLexeme(t, next);
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
        UpdateLexeme(t, c);

        int next = fgetc(fptr);
        if(next == '=') {
            t->type = MODEQ;
            UpdateLexeme(t, next);
            return VALID;
        }

        ungetc(next, fptr);
        t->type = MOD;
        return MOD;
    }
    t->type = NA;
    return NAT;
}

/* ---------- COMPARISONS  ----------*/

int IsNeqq(FILE* fptr, Token* t, int c)
{
    if(c == '!')
    {
        UpdateLexeme(t, c);

        int next = fgetc(fptr);
        if(next == '=') {
            t->type = NEQQ;
            UpdateLexeme(t, next);
            return VALID;
        }

        ungetc(next, fptr);
        t->type = NOT;
        return VALID;
    }

    t->type = NA;
    return NAT;
}
int IsGeqq(FILE* fptr, Token* t, int c)
{

    if(c == '>')
    {
        UpdateLexeme(t, c);

        int next = fgetc(fptr);
        if(next == '=') {
            t->type = GEQQ;
            UpdateLexeme(t, next);
            return VALID;
        }
        else if(next == '>') {
            UpdateLexeme(t, c);
            int next = fgetc(fptr);
            if(next == '=')
            {
                t->type = RIGHTEQ;
                UpdateLexeme(t, next);
                return VALID;
            }
            else
                ungetc(next, fptr);

            t->type = RSHIFT;
            UpdateLexeme(t, next);
            return VALID;
        }

        ungetc(next, fptr);
        t->type = GREAT;
        return VALID;
    }

    t->type = NA;
    return NAT;
}
int IsLeqq(FILE* fptr, Token* t, int c)
{
    
    if(c == '<')
    {
        UpdateLexeme(t, c);

        int next = fgetc(fptr);
        if(next == '=') {
            t->type = LEQQ;
            UpdateLexeme(t, next);
            return VALID;
        }
        else if(next == '<') {
            UpdateLexeme(t, c);
            int next = fgetc(fptr);
            if(next == '=')
            {
                t->type = LEFTEQ;
                UpdateLexeme(t, next);
                return VALID;
            }
            else
                ungetc(next, fptr);

            t->type = LSHIFT;
            UpdateLexeme(t, next);
            return VALID;
        }

        ungetc(next, fptr);
        t->type = LESS;
        return VALID;
    }

    t->type = NA;
    return NAT;
}
int IsAndl(FILE* fptr, Token* t, int c)
{

    if(c == '&')
    {
        UpdateLexeme(t, c);

        int next = fgetc(fptr);
        if(next == '&') {
            UpdateLexeme(t, next);

            next = fgetc(fptr);
            if(next == '=') {
                t->type = ANDLEQ;
                UpdateLexeme(t, next);
                return VALID;
            }
            else 
                ungetc(next, fptr);

            t->type = ANDL;
            UpdateLexeme(t, next);
            return VALID;
        }

        if(next == '=') {
            t->type = ANDEQ;
            UpdateLexeme(t, next);
            return VALID;
        }

        ungetc(next, fptr);
        t->type = AND;
        return VALID;     /* Bitwise */
    }

    t->type = NA;
    return NAT;
}
int IsOrl(FILE* fptr, Token* t, int c) 
{

    if(c == '|')
    {
        UpdateLexeme(t, c);

        int next = fgetc(fptr);
        if(next == '|'){ 
            UpdateLexeme(t, next);

            next = fgetc(fptr);
            if(next == '=') {
                t->type = ORLEQ;
                UpdateLexeme(t, next);
                return VALID;
            }
            else 
                ungetc(next, fptr);

            t->type = ORL;
            UpdateLexeme(t, next);
            return VALID;
        }

        if(next == '=') {
            t->type = OREQ;
            UpdateLexeme(t, next);
            return VALID;
        }

        ungetc(next, fptr);
        t->type = OR;
        return VALID;
    }

    t->type = NA;
    return NAT;
}

/* ---------- OTHERS ---------- */

int IsComm(FILE* fptr, Token* t, int c)
{
    /* Only called in IsDiv so first char already div */
    int next = c;
    int prev = 0;
    if(next == '*')
    {
        UpdateLexeme(t, next);
        while((next = fgetc(fptr)) != '/' && prev != '*')
        {
            if(next == EOF)
            {
                printf("ERROR: Comment doesn't end before EOF reached\n");
                t->type = ERR;
                return ERRT;
            }
            else if (next == '\n') LINE_NUM++;

            prev = next;
            UpdateLexeme(t, next);
        }

        UpdateLexeme(t, next);
        t->type = COMMENT;
        return VALID;
    } 
    else if (next == '/')
    {
        UpdateLexeme(t, next);
        /* Add EOF check */
        while((next = fgetc(fptr)) != '\n')
        {
            UpdateLexeme(t, next);
            if(next == EOF)
                break;
        }
        LINE_NUM++;
        t->type = COMMENT;
        return VALID;
    }
    t->type = NA;
    return NAT;
}


int IdentOrKeyword(FILE* fptr, Token* t, int c)
{
    if (!KWmap) KWMapInit();

    int next = c;
    while(next != '\n' && next != EOF)
    {
        if(!isalnum(next) && next != '_')
        {
            ungetc(next, fptr);
            break;
        }
        UpdateLexeme(t, next);
        next = fgetc(fptr);
    }
    if (next == '\n') ungetc(next, fptr);

    /* KW or IDENT */
    UpdateLexeme(t, '\0');  /* Null Terminator */
    void* val = DictLookup(KWmap, t->lex.word);
    if(val)
        t->type = (TokenType)(uintptr_t)val;
    else
        t->type = IDENT;

    return VALID;
}

