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
    int key;     /* Convert this to be any */
    char* val;
} Entry;

typedef struct KeyVal
{
    int key;
    char* val;
} KeyVal;

unsigned int Hash(unsigned int name);

/* Dicitonary */
typedef Entry* Dict[DICT_CAP];

Entry* DictLookup(Dict d, int key);
Entry* DictInstall(Dict* d, int key, char* val);

Dict* DictMake(int count, ...);
void DictPrint(Dict d);  

#endif 
