#include "Tokenizer.h"

TokenizerContext InitalizeTokenizerContext(FILE* fptr)
{
    TokenizerContext ctx;
    ctx.buffer1[TOKENIZER_BUFFER_SIZE - 1] = TOKENIZER_SENTINEL;
    ctx.buffer2[TOKENIZER_BUFFER_SIZE - 1] = TOKENIZER_SENTINEL;

    ctx.lexemeBegin = &ctx.buffer1[0];
    ctx.forward = &ctx.buffer1[0];

    ctx.row = 0;
    ctx.col = 0;

    ctx.fptr = fptr;

    LoadBuffer(ctx);
    return ctx;
}

void LoadBuffer(TokenizerContext ctx, int bufferNum)
{

}

char AdvanceBuffer(TokenizerContext ctx)
{
    if (*ctx.forward == TOKENIZER_SENTINEL) {
        if (c == &ctx.buffer1[TOKENIZER_BUFFER_SIZE]) {
            LoadBuffer(ctx, 2);
            ctx.forward = ctx.buffer2[0];
        }
        if (c == &ctx.buffer2[TOKENIZER_BUFFER_SIZE]) {
            LoadBuffer(ctx, 1);
            ctx.forward = ctx.buffer1[0];
        }
        else {
            return EOF;
        }
    }
    return *ctx.forward++;
}