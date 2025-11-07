#ifndef _DICTIONARY_H__
#define _DICTIONARY_H__

#define DICT(p1, ...) DictMake(p1, ...)
#define DICT_CAP 256
/* TODO: Make dictionary start small size, and grow as needed */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Entry 
{
    struct Entry* next;
    char* key;     /* TODO: Convert this to be any */
    int val;
} Entry;

typedef struct KeyVal
{
    char* key;
    int val;
} KeyVal;

unsigned int Hash(char* key);

/* Dicitonary */
typedef Entry* Dict[DICT_CAP];

Entry* DictLookup(Dict d, char* key);
Entry* DictInstall(Dict* d, char* key, int val);

Dict* DictMake(int count, ...);
void DictPrint(Dict d);  

#endif 
