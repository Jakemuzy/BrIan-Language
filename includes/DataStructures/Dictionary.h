#ifndef _DICTIONARY_H__
#define _DICTIONARY_H__

#define DICT(p1, ...) MakeDictionary(p1, ...)
#define DICT_MAX_CAP 1024
#define DICT_INIT_CAP 32

#include <stdarg.h>
#include <stdlib.h>
#include "DataStructures/Pair.h"

/* Buckets */
typedef struct {
    int capacity;
    Pair keyvalue;
} Bucket;

/* Hashing Functions */
void Hashfunction(Pair p);

/* Hash Table */
typedef struct {
    int numElements;
    Bucket** buckets;
} Dictionary;

/* Declare table with one or more keyvalue pairs */
void MakeDictionary(Pair p1, ...);
void Insert(Dictionary* dict, Pair p);


#endif 
