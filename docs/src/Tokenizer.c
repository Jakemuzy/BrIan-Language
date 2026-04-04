#include "STokenizer.h"

/* ----- Double Buffered Context ----- */

TokenizerContext* InitalizeTokenizerContext(FILE* fptr)
{
    TokenizerContext* ctx = malloc(sizeof(TokenizerContext));
    ctx->buffer1[TOKENIZER_BUFFER_SIZE -1] = TOKENIZER_SENTINEL;
    ctx->buffer2[TOKENIZER_BUFFER_SIZE -1] = TOKENIZER_SENTINEL;

    ctx->lexemeBegin = ctx->buffer1;
    ctx->forward = ctx->buffer1;

    ctx->row = 0;
    ctx->col = 0;

    ctx->fptr = fptr;

    LoadBuffer(ctx, 1);
    return ctx;
}

void DestroyTokenizerContext(TokenizerContext* ctx) 
{
    fclose(ctx->fptr);
    free(ctx);
}

void LoadBuffer(TokenizerContext* ctx, int bufferNum)
{
    char* buffer = (bufferNum == 1) ? ctx->buffer1 : ctx->buffer2;
    size_t n = fread(buffer, sizeof(char), TOKENIZER_BUFFER_SIZE-1, ctx->fptr);
    if (n < TOKENIZER_BUFFER_SIZE - 1) buffer[n] = TOKENIZER_SENTINEL;
}

void RetractBuffer(TokenizerContext* ctx, char* pos) 
{
    while (ctx->forward != pos) {
        ctx->forward--;
        if (*ctx->forward == TOKENIZER_SENTINEL)  
            ctx->forward--;
    }
}

char AdvanceBuffer(TokenizerContext* ctx)
{
    if (*ctx->forward == TOKENIZER_SENTINEL) {
        if (ctx->forward == &ctx->buffer1[TOKENIZER_BUFFER_SIZE - 1]) {
            LoadBuffer(ctx, 2);
            ctx->forward = ctx->buffer2;
        }
        else if (ctx->forward == &ctx->buffer2[TOKENIZER_BUFFER_SIZE - 1]) {
            LoadBuffer(ctx, 1);
            ctx->forward = ctx->buffer1;
        }
        else {
            return EOF;
        }
    }
    return *ctx->forward++;
}

Token ExtractTokenFromBuffer(TokenizerContext* ctx)
{
    size_t lexLength = ctx->forward - ctx->lexemeBegin;
    char* lexeme = malloc(lexLength + 1);
    memcpy(lexeme, ctx->lexemeBegin, lexLength);
    lexeme[lexLength] = '\0';

    ctx->lexemeBegin = ctx->forward;

    if (lexLength >= TOKEN_MAX_LENGTH) {
        printf("ERROR: Identifier is limited to %d characters\n", TOKEN_MAX_LENGTH);
        abort();
    }

    /* Caller fills TokenType */
    return (Token) {ERR, ctx->row, ctx->col, lexeme, lexLength};
}


/* ----- Function Driven DFA ----- */


/* ----- Tokenization ----- */

Token GetNextToken(TokenizerContext* ctx) 
{
    int c = SkipWhitespace(ctx);
    if (c == EOF) return (Token) { END, ctx->row, ctx->col, "", 0 };
    CharClass class = CHAR_MAP[(unsigned int)c];


    switch (class) {
        /* Error likely eof */
        case (CC_ERROR): printf("Invalid character found"); abort();
        case (CC_DIGIT): return ScanNumber(ctx);
        case (CC_ALPHA): return ScanIdentOrKeyword(ctx);
        case (CC_HASH): return ScanDirective(ctx);
        case (CC_SINGLE_QUOTE): return ScanCharacter(ctx);
        case (CC_DOUBLE_QUOTE): return ScanString(ctx);
        case (CC_DIVIDE): return SkipComment(ctx);     /* Gets next token after comment */
        default: return ScanOperator(ctx);
    }
}

Token SkipComment(TokenizerContext* ctx)
{
    // Returns next token AFTER comment is consumed 
    char* past = ctx->forward;

    int c = AdvanceBuffer(ctx);
    if (c == '/') {
        c = AdvanceBuffer(ctx);
        if (c == '/') {
            while (c != '\n') {
                c = AdvanceBuffer(ctx);
                if (c == EOF) { printf("EOF reached before comment end\n"); abort(); }
            }
            ctx->lexemeBegin = ctx->forward;
            return GetNextToken(ctx);
        }
        else if (c == '*') {
            int last = ' ';
            while (!(c == '/' && last == '*')) {
                if (c == EOF) { printf("EOF reached before comment end\n"); abort(); }
                last = c; 
                c = AdvanceBuffer(ctx); 
            }
            ctx->lexemeBegin = ctx->forward;
            return GetNextToken(ctx);
        }

    } 
    
    RetractBuffer(ctx, past);
    return ScanOperator(ctx);   /* If not a comment, probably an operator */
}

int SkipWhitespace(TokenizerContext* ctx) 
{
    int c = AdvanceBuffer(ctx);
    if (c == EOF) return EOF;

    while (isspace(c)) {
        ctx->col++;
        if (c == '\n') { ctx->row++; ctx->col = 0; }

        ctx->lexemeBegin = ctx->forward;  
        c = AdvanceBuffer(ctx);
    }

    RetractBuffer(ctx, ctx->forward - 1);
    return c;
}

Token ScanOperator(TokenizerContext* ctx)
{
    int c;
    int current = 1, lastAccept = 0;
    char* lastAcceptPos = ctx->forward;
  
    while (current != 0) {
        c = AdvanceBuffer(ctx);
        if (c == EOF) break;

        CharClass cc = CHAR_MAP[c];
        current = TABLE_DFA[current][(unsigned int)cc];

        if (ACCEPT_STATES[current] != ERR) {
            lastAccept = current;
            lastAcceptPos = ctx->forward;
        }
    }

    if (lastAccept == 0) { printf("Invalid Table DFA\n"); abort(); }
    RetractBuffer(ctx, lastAcceptPos);

    Token tok = ExtractTokenFromBuffer(ctx);
    tok.type = ACCEPT_STATES[lastAccept];
    ctx->col += tok.lexLength;
    return tok;
}

Token ScanDirective(TokenizerContext* ctx)
{
    int c = AdvanceBuffer(ctx);
    while (c != '\n') c = AdvanceBuffer(ctx);
    RetractBuffer(ctx, ctx->forward - 1);

    Token tok = ExtractTokenFromBuffer(ctx);
    tok.type = DIRECTIVE;
    ctx->col += tok.lexLength;
    return tok;
}

Token ScanNumber(TokenizerContext* ctx)
{
    int c = AdvanceBuffer(ctx);
    bool isReal = false;

    if (!isdigit(c)) { printf("Invalid number\n"); abort(); }
    while (isdigit(c)) c = AdvanceBuffer(ctx);

    if (c == '.') {
        isReal = true;
        c = AdvanceBuffer(ctx);
        if (!isdigit(c)) { printf("Expected digit after '.'\n"); abort(); }
        while (isdigit(c)) c = AdvanceBuffer(ctx);
    }

    if (c == 'e') {
        isReal = true;
        c = AdvanceBuffer(ctx);
        if (c == '+' || c == '-') c = AdvanceBuffer(ctx);
        if (!isdigit(c)) { printf("Expected digit after exponent\n"); abort(); }
        while (isdigit(c)) c = AdvanceBuffer(ctx);
    }

    RetractBuffer(ctx, ctx->forward - 1);
    Token tok = ExtractTokenFromBuffer(ctx);
    tok.type = isReal ? REAL : INTEGRAL;
    ctx->col += tok.lexLength;
    return tok;
}

Token ScanString(TokenizerContext* ctx)
{
    int c = AdvanceBuffer(ctx); 

    while (1) {
        c = AdvanceBuffer(ctx);

        if (c == EOF)  { printf("EOF reached before string closed\n"); abort(); }
        if (c == '\n') { printf("Newline in string literal at line %d\n", ctx->row); abort(); }
        if (c == '\0') { printf("Null byte in string literal\n"); abort(); }

        if (c == '\\') {
            c = AdvanceBuffer(ctx);
            switch (c) {
                case '"': case '\\': case '/':
                case 'n': case 't': case 'r':
                case 'b': case 'f':             break;
                case 'u': /* TODO: \uXXXX */    break;
                default: printf("Unknown escape sequence '\\%c'\n", c); abort();
            }
            continue;
        }

        if (c == '"') break; 
    }

    Token tok = ExtractTokenFromBuffer(ctx);
    tok.type = SLITERAL;
    ctx->col += tok.lexLength;
    return tok;
}

Token ScanCharacter(TokenizerContext* ctx)
{
    int c = AdvanceBuffer(ctx);
    c = AdvanceBuffer(ctx);

    if (c == '\n' || c == '\0') { printf("Invalid character in string\n"); abort(); }
    if (c == EOF) { printf("EOF reached before string closed\n"); abort(); }

    c = AdvanceBuffer(ctx);
    if (c != '\'') { printf("No closing \' found for character\n"); abort(); }

    Token tok = ExtractTokenFromBuffer(ctx);
    tok.type = CLITERAL;
    return tok;
}

Token ScanIdentOrKeyword(TokenizerContext* ctx)
{
    int c = '_';
    while (isalpha((unsigned int)c) || isdigit((unsigned int)c) || c == '_') 
        c = AdvanceBuffer(ctx);
    RetractBuffer(ctx, ctx->forward - 1);    

    Token tok = ExtractTokenFromBuffer(ctx);
    tok.type = KeywordLookup(tok.lexeme);
    return tok;
}
