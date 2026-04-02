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

/* ----- Table Driven DFA ----- */

bool IsAcceptState(int c)
{
    for (size_t i = 0; i < sizeof(ACCEPT_STATES) / sizeof(int); i++)
        if (c == ACCEPT_STATES[i]) return true;
    return false;
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
    /* Table driven */

    /* Function Driven */

    /* Keyword Map For Rest */
}

int SkipWhitespace(TokenizerContext* ctx) 
{
    int c = ' ';
    while (isspace(c)) c = AdvanceBuffer(ctx);
    return c;
}

Token ScanOperator(TokenizerContext* ctx)
{
    int c;
    int current = 1, lastAccept = -1;
    char* lastAcceptPos = NULL;
   
    while (current != 0) {
        int c = AdvanceBuffer(ctx);
        current = TABLE_DFA[current][(unsigned int)c];

        if (c == '\n') { ctx->row++; ctx->col = 0; }
        else ctx->col++;

        if (IsAcceptState(current)) {
            lastAccept = current;
            lastAcceptPos = ctx->forward;
        }
    }

    if (lastAccept == -1) { printf("Invalid Table DFA\n"); abort(); }
    if (current != lastAccept) {
        ctx->forward = lastAcceptPos;
        if (*ctx->forward == '\0') ctx->forward--;
    }

    size_t lexLength = ctx->forward - ctx->lexemeBegin;
    char lexeme[lexLength + 1];
    strncpy(lexeme, ctx->lexemeBegin, lexLength);
    lexeme[lexLength] = '\0';

    ctx->lexemeBegin = ctx->forward;
    return (Token) {ACCEPT_STATES[lastAccept], ctx->row, ctx->col, lexeme, lexLength};
}

Token ScanDirective(TokenizerContext* ctx)
{

}

Token ScanNumber(TokenizerContext* ctx)
{

}

Token ScanString(TokenizerContext* ctx)
{

}

Token ScanCharacter(TokenizerContext* ctx)
{

}

Token ScanIdentOrKeyword(TokenizerContext* ctx)
{

{

}
}
