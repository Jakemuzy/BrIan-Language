#include "SToken.h"

int KeywordHash(const char* word) {
    unsigned int h = 2166136261u;
    while (*word) {
        h ^= (unsigned char)*word++;
        h *= 16777619u;
    }
    return (int)(h % 378);
}

TokenType KeywordLookup(const char* word)
{
    int hash = KeywordHash(word);
    printf("%s\t%d\n", word, hash);
    if (hash > MAX_KW_HASH) return IDENT;

    KeywordTypePair pair = KEYWORD_MAP[hash];
    if (pair.str == NULL) return IDENT;
    if (strcmp(word, pair.str) != 0) return IDENT;
    return pair.type;
}
