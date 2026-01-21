#include "Dict.h"

/* Generic hash function for strings 
unsigned int Hash(char* key)
{
    char* str;
    unsigned int hash = 0;
    for (str = key; *str; str++)
        hash = hash * 65599 + *str;
    return hash;
}

Bucket* InitBucket(char* key, void* binding, Bucket* next) 
{
    Bucket* buck = malloc(sizeof(*buck));
    buck->key = key; 
    buck->binding = binding;
    buck->next = next;
    return buck;
}

void BucketPush(char* key, void* binding)
{
    int index = Hash(key) % SIZE;
    Dicts[index] = InitBucket(key, binding, Dicts[index]);
}

void BucketPop(char* key)
{
    int index = Hash(key) % SIZE;
    Dicts[index] = Dicts[index]->next;
}

void* BucketLookup(char* key)
{
    int index = Hash(key) % SIZE;
    Bucket* buck;
    for (buck = Dicts[index]; buck; buck = buck->next)
        if (strcmp(buck->key, key) == 0) return buck->binding;
    return NULL;
}

*/