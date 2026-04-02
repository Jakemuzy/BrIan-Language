#include "STokenizer.h"

int main(int argc, char* argv[]) {
    if (argc <= 1) { printf("INVALID ARG COUNT"); abort(); }

    FILE* fptr = fopen(argv[1], "r");
    if (!fptr) { printf("NO SUCH FILE\n"); abort(); }

    int c;
    TokenizerContext* ctx = InitalizeTokenizerContext(fptr);

    return 0;
}
