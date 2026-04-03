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
}

void LoadBuffer(TokenizerContext* ctx, int bufferNum)
{
    char* buffer = (bufferNum == 1) ? ctx->buffer1 : ctx->buffer2;
    fread(buffer, sizeof(char), TOKENIZER_BUFFER_SIZE-1, ctx->fptr);
}

void RetractBuffer(TokenizerContext* ctx, char* pos) 
{
    while (ctx->forward != pos) {
        ctx->forward--;
        if (*ctx->forward == '\0')  
            ctx->forward--;
    }
}

char AdvanceBuffer(TokenizerContext* ctx)
{
    if (*ctx->forward == TOKENIZER_SENTINEL) {
        if (ctx->forward == &ctx->buffer1[TOKENIZER_BUFFER_SIZE]) {
            LoadBuffer(ctx, 2);
            ctx->forward = ctx->buffer2;
        }
        if (ctx->forward == &ctx->buffer2[TOKENIZER_BUFFER_SIZE]) {
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

    if (lexLength >= TOKENIZER_BUFFER_SIZE) {
        printf("ERROR: Identifier is too long \n");
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
    CharClass class = CHAR_MAP[(unsigned int)c];

    switch (class) {
        case (CC_ERROR): printf("Invalid character found"); abort();
        case (CC_DIGIT): return ScanNumber(ctx);
        case (CC_ALPHA): return ScanIdentOrKeyword(ctx);
        case (CC_HASH): return ScanDirective(ctx);
        case (CC_SINGLE_QUOTE): return ScanCharacter(ctx);
        case (CC_DOUBLE_QUOTE): return ScanString(ctx);
        default: return ScanOperator(ctx);
    }
}

int SkipWhitespace(TokenizerContext* ctx) 
{
    int c = AdvanceBuffer(ctx);
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
    return (Token) {ERR, -1, -1, "", 0};
}

Token ScanNumber(TokenizerContext* ctx)
{
    return (Token) {ERR, -1, -1, "", 0};
}

Token ScanString(TokenizerContext* ctx)
{
    return (Token) {ERR, -1, -1, "", 0};
}

Token ScanCharacter(TokenizerContext* ctx)
{
    return (Token) {ERR, -1, -1, "", 0};
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
