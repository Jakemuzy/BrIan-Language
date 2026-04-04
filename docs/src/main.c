#include "STokenizer.h"

int main(int argc, char* argv[]) {
    if (argc <= 1) { printf("INVALID ARG COUNT"); abort(); }

    FILE* fptr = fopen(argv[1], "r");
    if (!fptr) { printf("NO SUCH FILE\n"); abort(); }

    TokenizerContext* ctx = InitalizeTokenizerContext(fptr);
    Token tok;
    while ( (tok = GetNextToken(ctx)).type != ERR && tok.type != END)
        printf("Token: %s\tRow: %d\tCol:%d\tTokNum: %d\n", tok.lexeme, tok.row, tok.col, tok.type);
    DestroyTokenizerContext(ctx);

    return 0;
}
