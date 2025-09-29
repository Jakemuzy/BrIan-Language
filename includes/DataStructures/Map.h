#ifndef _MAP_H__
#define _MAP_H__

/* Generics (not implemented yet) */

/* Specifically for Tokens right now */
#define KV(k, v) MakeKeyValuePair(k, v)
typedef struct {
    char* key;
    TokenType value;
} KeyValuePair;

KeyValuePair MakeKeyValuePair(char* k, TokenType v);

#endif 
