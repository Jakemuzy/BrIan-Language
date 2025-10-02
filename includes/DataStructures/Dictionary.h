#ifndef _DICTIONARY_H__
#define _DICTIONARY_H__

#define DICT(p1, ...) MakeDictionary(p1, ...)

#include "DataStructures/Pair.h"
#include <stdarg.h>

/* Buckets */
typedef struct {
    int capacity;
    void* data;
} Bucket;

/* Hashing Functions */
void Hashfunction(Pair p);

/* Hash Table */
typedef struct {
    int code;
    Bucket* buckets;
} Dictionary;

/* Declare table with one or more keyvalue pairs */
void MakeDictionary(Pair p1, ...);
void AddHash(Dictionary* dict, Pair p);


#endif 
