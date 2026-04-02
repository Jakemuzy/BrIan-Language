#include "Tokenizer.h"

int main(int argc, char* argv[]) {
    if (argc < 2) abort();

    FILE* fptr = fopen(argv[1], "r");

    TokenizerContext ctx = InitalizeTokenizerContext(fptr);
    return 0;
}