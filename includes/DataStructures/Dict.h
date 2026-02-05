#ifndef _DICT_H__
#define _DICT_H__

/* Linked List Hash Table */
#include <stdio.h>
#include <stdlib.h>

#define SIZE 109

typedef enum HASH_TYPE {
    HASH_INT, HASH_CHAR, 
    HASH_STR, HASH_PTR,
    HASH_UINT
} HASH_TYPE;

unsigned int Hash(void* key, HASH_TYPE keyType, size_t size);

typedef struct Bucket {
    union {
        int i; char c; unsigned int ui;
        char* s; void* p;
    } key;

    void* val;
} Bucket;

typedef struct Dict {
    Bucket** buckets;
    size_t size;

    HASH_TYPE keyType;
} Dict;

Dict  DictInit(HASH_TYPE keyType, size_t size);
void* DictPush(Dict* dict, void* key, void* val);
void* DictPop(Dict* dict, void* key);
void* DictLookup(Dict* dict, void* key);
void* DictResize(Dict* dict, size_t newSize);   /* Auto, dont call */

#endif 