#include "Dict.h"

/* ---------- Hash Functions ---------- */

unsigned int Hash(void* key, HASH_TYPE keyType, size_t size)
{
    switch (keyType) {
        case (HASH_STR):
            return HashStr((char*)key);
        case (HASH_UINT):
            return HashUint((unsigned int)key);
        case (HASH_INT):
            return HashInt((int)key);
        case (HASH_CHAR):
            return HashChar((char)key);
        case (HASH_PTR):
            return HashPtr(key);
        default:
            printf("Hashing invalid type\n");
            return -1;
    }
}

static unsigned int HashStr(char* key) 
{
    char* str;
    unsigned int hash = 0;
    for (str = key; *str; str++)
        hash = hash * 65599 + *str;
    return hash;
}

static unsigned int HashUint(unsigned int key)
{
    return ((key * 2654435761U) >> 8) % SIZE;
}

static unsigned int HashChar(char key)
{

}

static unsigned int HashInt(int key)
{

}

static unsigned int HashPtr(void* ptr)
{

}

/* ---------- Dictionary ---------- */

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



Dict DictInit(HASH_TYPE keyType, size_t size)
{
    Dict dict; 
    dict.buckets = malloc(sizeof(Bucket*) * size);
    dict.size = size;
    dict.keyType = keyType;
    return dict;
}

void* DictPush(Dict* dict, void* key, void* val)
{
    /* TODO: check for resize */
    void* val = DictLookup(dict, key);
    if (val) return val; /* TODO: return for freeing */

    size_t index = Hash(key, dict->keyType, dict->size);
    Bucket* bucket = malloc(sizeof(Bucket));

    HASH_TYPE keyType = dict->keyType;
    switch (keyType) {
        case (HASH_STR): bucket->key.s = (char*)key;
        case (HASH_UINT): bucket->key.ui = (unsigned int)key;
        case (HASH_INT): bucket->key.i = (int)key;
        case (HASH_CHAR): bucket->key.c = (char)key;
        case (HASH_PTR): bucket->key.p = key;
    }
    
    bucket->val = val;
    dict->buckets[index] = bucket;
}

void* DictPop(Dict* dict, void* key)
{
    size_t index = Hash(key, dict->keyType, dict->size);
    Bucket* bucket = dict->buckets[index];
    if (!bucket) return NULL;

    void* val = bucket->val;
    switch (keyType) {
        case (HASH_STR): bucket->key.s = (char*)key;
        case (HASH_UINT): bucket->key.ui = (unsigned int)key;
        case (HASH_INT): bucket->key.i = (int)key;
        case (HASH_CHAR): bucket->key.c = (char)key;
        case (HASH_PTR): bucket->key.p = key;
    }

    return val;
}

void* DictLookup(Dict* dict, void* key)
{
    size_t index = Hash(key, dict->keyType, dict->size);
    Bucket* bucket = dict->buckets[index];
    if (!bucket) return NULL;

    void* val = bucket->val;
    return val;
}