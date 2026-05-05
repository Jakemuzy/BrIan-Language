#include "Tokenizer.h"

// TODO: identifier at EOF is infinite loop
// this whole thing needs a redesign around the buffer system

/* ----- Double Buffered Context ----- */

TokenizerContext* InitalizeTokenizerContext(FILE* fptr, size_t fileSize)
{
    TokenizerContext* ctx = malloc(sizeof(TokenizerContext));
    ctx->buffer1[TOKENIZER_BUFFER_SIZE -1] = TOKENIZER_SENTINEL;
    ctx->buffer2[TOKENIZER_BUFFER_SIZE -1] = TOKENIZER_SENTINEL;

    ctx->lexemeBegin = ctx->buffer1;
    ctx->forward = ctx->buffer1;

    ctx->row = 1;
    ctx->col = 1;

    ctx->fptr = fptr;

    /* Make the constant variable depending on density */
    size_t arenaSize = fileSize * ARENA_CAPACITY_MULTIPLIER_FROM_FILESIZE;
    ctx->arena = CreateArena(arenaSize);

    LoadBuffer(ctx, 1);
    return ctx;
}

void DestroyTokenizerContext(TokenizerContext* ctx) 
{
    fclose(ctx->fptr);
    free(ctx);
}

void SetEdgeCaseFlag(TokenizerContext* ctx, bool val)
{
    // For now only handles '>>' ambiguity, but could easily make it an enum flag
    ctx->nestedChan = val;
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
        if (ctx->forward == &ctx->buffer1[TOKENIZER_BUFFER_SIZE - 1] ||
            ctx->forward == &ctx->buffer2[TOKENIZER_BUFFER_SIZE - 1])
            ctx->forward--;
    }
}

int AdvanceBuffer(TokenizerContext* ctx)
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
    char* sentinelPos1 = &ctx->buffer1[TOKENIZER_BUFFER_SIZE - 1];
    char* sentinelPos2 = &ctx->buffer2[TOKENIZER_BUFFER_SIZE - 1];

    size_t lexLength;
    char* lexeme;

    // Can't just blindly memcpy since cross boundaries would copy an extra \0 
    bool spansBuffer = (ctx->lexemeBegin <= sentinelPos1 && ctx->forward > sentinelPos1)
                || (ctx->lexemeBegin <= sentinelPos2 && ctx->forward > sentinelPos2)
                || (ctx->lexemeBegin >= ctx->buffer2 && ctx->forward < ctx->buffer2); 

    if (spansBuffer) {
        char* sentinel = (ctx->lexemeBegin < ctx->buffer2) ? sentinelPos1 : sentinelPos2;

        size_t part1 = sentinel - ctx->lexemeBegin;       
        char* secondBufferStart = (ctx->lexemeBegin < ctx->buffer2) ? ctx->buffer2 : ctx->buffer1;
        char* part2Src = secondBufferStart;
        size_t part2 = ctx->forward - secondBufferStart;

        lexLength = part1 + part2;
        if (lexLength >= TOKEN_MAX_LENGTH) 
            ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "Identifier is limited to %d characters.\n", TOKEN_MAX_LENGTH);
        lexeme = AllocateArena(ctx->arena, lexLength + 1);

        memcpy(lexeme, ctx->lexemeBegin, part1);
        memcpy(lexeme + part1, part2Src, part2);
    } else {
        lexLength = ctx->forward - ctx->lexemeBegin;
        if (lexLength >= TOKEN_MAX_LENGTH) 
            ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "Identifier is limited to %d characters.\n", TOKEN_MAX_LENGTH);
        lexeme = AllocateArena(ctx->arena, lexLength + 1);
        memcpy(lexeme, ctx->lexemeBegin, lexLength);
    }

    lexeme[lexLength] = '\0';
    size_t startCol = ctx->col;
    ctx->lexemeBegin = ctx->forward;
    ctx->col += lexLength;

    /* Caller fills TokenType */
    return (Token) {ERR, ctx->row, startCol, lexeme, lexLength};
}

/* ----- Tokenization ----- */

Token GetNextToken(TokenizerContext* ctx) 
{
    int c = SkipWhitespace(ctx);
    if (c == EOF) return (Token) { END, ctx->row, ctx->col, "", 0 };
    CharClass class = CHAR_MAP[(unsigned int)c];


    switch (class) {
        case (CC_ERROR): ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "Unknown character discovered %c on line %d row %d\n", c, ctx->row, ctx->col - 1);
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
            while (c != '\n' && c != EOF) 
                c = AdvanceBuffer(ctx);
            ctx->lexemeBegin = ctx->forward;
            ctx->row++; ctx->col = 1;
            return GetNextToken(ctx);
        }
        else if (c == '*') {
            int last = ' ';
            while (!(c == '/' && last == '*')) {
                if (c == EOF) 
                    ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "EOF reached before comment end");
                else if (c == '\n') { ctx->row++; ctx->col = 1; }
                last = c; 
                c = AdvanceBuffer(ctx); 
                ctx->col++;
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
        if (c == '\n') { ctx->row++; ctx->col = 1; }
        else ctx->col++;
        c = AdvanceBuffer(ctx);
    }
    if (c != EOF) ctx->forward--; // Overconsumption in space check
    ctx->lexemeBegin = ctx->forward;
    return c;
}

Token ScanOperator(TokenizerContext* ctx)
{
    int c;
    int current = 1, lastAccept = DFA_ERROR_STATE;
    char* lastAcceptPos = ctx->forward;
  
    while (current != DFA_ERROR_STATE) {
        c = AdvanceBuffer(ctx);
        if (c == EOF) break;

        CharClass cc = CHAR_MAP[c];
        current = TABLE_DFA[current][(unsigned int)cc];

        if (ACCEPT_STATES[current] != ERR) {
            lastAccept = current;
            lastAcceptPos = ctx->forward;
        }
    }

    if (lastAccept == DFA_ERROR_STATE) 
        ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "Invalid operator discovered %c on line %d row %d\n", c, ctx->row, ctx->col);

    if (ctx->nestedChan && ACCEPT_STATES[lastAccept] == RSHIFT)
        lastAcceptPos--;  // retract to after first '>' only
    RetractBuffer(ctx, lastAcceptPos);

    Token tok = ExtractTokenFromBuffer(ctx);
    tok.type = (ctx->nestedChan && ACCEPT_STATES[lastAccept] == RSHIFT) ? GREAT : ACCEPT_STATES[lastAccept];
    return tok;
}

Token ScanDirective(TokenizerContext* ctx)
{
    int c = AdvanceBuffer(ctx);
    while (c != '\n' && c != EOF) c = AdvanceBuffer(ctx); // EOF fail
    RetractBuffer(ctx, ctx->forward);

    Token tok = ExtractTokenFromBuffer(ctx);
    tok.type = DIRECTIVE;
    return tok;
}

Token ScanNumber(TokenizerContext* ctx)
{
    int c = AdvanceBuffer(ctx);
    bool isReal = false;

    if (c == '0') {
        c = AdvanceBuffer(ctx);
        if (c == 'x') {
            c = AdvanceBuffer(ctx);
            if (!isxdigit(c)) {
                RetractBuffer(ctx, ctx->forward - 1); Token t = ExtractTokenFromBuffer(ctx);
                ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "Expected hex digit after '0x' in token '%s' on line %d row %d\n", t.lexeme, t.row, t.col);
            }
            while (isxdigit(c)) c = AdvanceBuffer(ctx);
            RetractBuffer(ctx, ctx->forward - 1);
            Token tok = ExtractTokenFromBuffer(ctx);
            tok.type = HEX;
            return tok;
        }
    }

    while (isdigit(c)) c = AdvanceBuffer(ctx);

    if (c == '.') {
        isReal = true;
        c = AdvanceBuffer(ctx);
        if (!isdigit(c)) {
            RetractBuffer(ctx, ctx->forward - 1); Token t = ExtractTokenFromBuffer(ctx);
            ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "Expected digit after '.' in token '%s' on line %d row %d\n", t.lexeme, t.row, t.col);
        }
        while (isdigit(c)) c = AdvanceBuffer(ctx);
    }

    if (c == 'e') {
        isReal = true;
        c = AdvanceBuffer(ctx);
        if (c == '+' || c == '-') c = AdvanceBuffer(ctx);
        if (!isdigit(c)) { 
            RetractBuffer(ctx, ctx->forward - 1); Token t = ExtractTokenFromBuffer(ctx);
            ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "Expected digit after exponent in token '%s' on line %d row %d\n", t.lexeme, t.row, t.col);
        }
        while (isdigit(c)) c = AdvanceBuffer(ctx);
    }

    RetractBuffer(ctx, ctx->forward - 1);
    Token tok = ExtractTokenFromBuffer(ctx);
    tok.type = isReal ? REAL : INTEGRAL;
    return tok;
}

Token ScanString(TokenizerContext* ctx)
{
    int c = AdvanceBuffer(ctx); 

    while (1) {
        c = AdvanceBuffer(ctx);

        if (c == EOF)  {
            RetractBuffer(ctx, ctx->forward); Token t = ExtractTokenFromBuffer(ctx);
            ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "EOF discovered before string terminator ' %s ' on line %d row %d\n", t.lexeme, t.row, t.col);
        }
        if (c == '\n') {
            RetractBuffer(ctx, ctx->forward - 1); Token t = ExtractTokenFromBuffer(ctx);
            ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "Illegal newline in string literal ' %s ' on line %d row %d\n", t.lexeme, t.row, t.col);
        }
        if (c == '\0') { 
            RetractBuffer(ctx, ctx->forward); Token t = ExtractTokenFromBuffer(ctx);
            ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "Illegal null byte '\0' found in string literal ' %s ' on line %d row %d\n", t.lexeme, t.row, t.col);
        }

        if (c == '\\') {
            c = AdvanceBuffer(ctx);
            switch (c) {
                case '"': case '\\': case '/':
                case 'n': case 't': case 'r':
                case 'b': case 'f':             break;
                case 'u': /* TODO: \uXXXX */    break;
                default: 
                    Token t = ExtractTokenFromBuffer(ctx);
                    ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "Unknown escape sequence '\\%c' found in string literal ' %s ' on line %d row %d\n", c, t.lexeme, t.row, t.col);
            }
            continue;
        }

        if (c == '"') break; 
    }

    Token tok = ExtractTokenFromBuffer(ctx);
    tok.type = SLITERAL;
    return tok;
}

Token ScanCharacter(TokenizerContext* ctx)
{
    int c = AdvanceBuffer(ctx);
    c = AdvanceBuffer(ctx);

    if (c == '\n' || c == '\0') ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "Invalid newline or null byte found in character literal on line %d row %d\n", ctx->row, ctx->col);
    if (c == EOF) ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "Invalid EOF found in character literal on line %d row %d\n", ctx->row, ctx->col);

    if (c == '\\') {
        c = AdvanceBuffer(ctx);
        switch (c) {
            case '"': case '\\': case '/':
            case 'n': case 't': case 'r':
            case 'b': case 'f':             break;
            case 'u': /* TODO: \uXXXX */    break;
            default: ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "Unknown escape sequence '\\%c' found in character literal on line %d row %d\n", c, ctx->row, ctx->col);
        }
    }

    c = AdvanceBuffer(ctx);
    if (c != '\'') ERROR(ERR_FLAG_EXIT, TOKENIZER_ERR, "No closing \' found for character\n"); 

    Token tok = ExtractTokenFromBuffer(ctx);
    tok.type = CLITERAL;
    return tok;
}

Token ScanIdentOrKeyword(TokenizerContext* ctx)
{
    int c = AdvanceBuffer(ctx);
    while (c != EOF && ( isalpha((unsigned int)c) || isdigit((unsigned int)c) || c == '_' )) 
        c = AdvanceBuffer(ctx);
    RetractBuffer(ctx, ctx->forward - 1);    

    Token tok = ExtractTokenFromBuffer(ctx);
    tok.type = KeywordLookup(tok.lexeme);
    return tok;
}
