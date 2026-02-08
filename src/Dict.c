#include "Dict.h"

/* ---------- Hash Functions ---------- */

static unsigned int HashStr(char* key, size_t size) 
{
    char* str;
    unsigned int hash = 0;
    for (str = key; *str; str++)
        hash = hash * 65599 + *str;
    return hash % size;
}

static unsigned int HashUint(unsigned int key, size_t size)
{
    return ((key * 2654435761U) >> 8) % size;
}

static unsigned int HashChar(char key, size_t size)
{
    return (unsigned int)(key) % size;
}

static unsigned int HashInt(int key, size_t size)
{
    return (unsigned int)(key) % size;
}

static unsigned int HashPtr(const void* ptr, size_t size)
{
    return 1;
}

unsigned int Hash(const void* key, HASH_TYPE keyType, size_t size)
{
    switch (keyType) {
        case (HASH_STR):
            return HashStr((char*)key, size);
        case (HASH_UINT):
            return HashUint(*(unsigned int*)key, size);
        case (HASH_INT):
            return HashInt(*(int*)key, size);
        case (HASH_CHAR):
            return HashChar(*(char*)key, size);
        case (HASH_PTR):
            return HashPtr(key, size);
        default:
            printf("Hashing invalid type\n");
            abort();
    }
}

/* ---------- Helpers ----------*/ 

static const void* BucketKeyPtr(const Bucket* b, HASH_TYPE type)
{
    switch (type) {
        case HASH_INT:  return &b->key.i;
        case HASH_UINT: return &b->key.ui;
        case HASH_CHAR: return &b->key.c;
        case HASH_STR:  return b->key.s;
        case HASH_PTR:  return b->key.p;
    }
    return NULL;
}

static  int KeyEquals(Bucket* b, void* key, HASH_TYPE type)
{
    switch (type) {
        case HASH_INT:  return b->key.i  == *(int*)key;
        case HASH_UINT: return b->key.ui == *(unsigned int*)key;
        case HASH_CHAR: return b->key.c  == *(char*)key;
        case HASH_PTR:  return b->key.p  == key;
        case HASH_STR:  return strcmp(b->key.s, (char*)key) == 0;
    }
    return 0;
}

/* ---------- Dictionary ---------- */

Dict DictInit(HASH_TYPE keyType, size_t size)
{
    Dict dict; 
    dict.buckets = calloc(size, sizeof(Bucket*));
    dict.size = size;
    dict.currentSize = 0;
    dict.keyType = keyType;
    return dict;
}

void DictPush(Dict* dict, void* key, void* val)
{
    if (dict->currentSize * 2 >= dict->size) 
        DictResize(dict, dict->size * 2);

    size_t index = Hash(key, dict->keyType, dict->size);
    Bucket* bucket = malloc(sizeof(Bucket));

    while (dict->buckets[index] && dict->buckets[index] != TOMBSTONE) 
        index = (index + 1) % dict->size;

    dict->buckets[index] = bucket;

    HASH_TYPE keyType = dict->keyType;
    switch (keyType) {
        case (HASH_UINT): bucket->key.ui = *(unsigned int*)key; break;
        case (HASH_INT): bucket->key.i = *(int*)key; break;
        case (HASH_CHAR): bucket->key.c = *(char*)key; break;
        case (HASH_STR): bucket->key.s = (char*)key; break;
        case (HASH_PTR): bucket->key.p = key; break;
    }
    
    bucket->val = val;
    dict->currentSize++;
}

Bucket* DictPop(Dict* dict, void* key)
{
    size_t index = Hash(key, dict->keyType, dict->size);

    while (dict->buckets[index]) {
        if (dict->buckets[index] != TOMBSTONE && KeyEquals(dict->buckets[index], key, dict->keyType)) {
            Bucket* b = dict->buckets[index];
            dict->buckets[index] = TOMBSTONE;   
            dict->currentSize--;
            return b;
        }
        index = (index + 1) % dict->size;
    }
    return NULL;
}

void* DictLookup(Dict* dict, void* key)
{
    size_t index = Hash(key, dict->keyType, dict->size);

    while (dict->buckets[index]) {
        if (dict->buckets[index] != TOMBSTONE &&
            KeyEquals(dict->buckets[index], key, dict->keyType))
            return dict->buckets[index]->val;

        index = (index + 1) % dict->size;
    }
    return NULL;
}

void DictResize(Dict* dict, size_t newSize)
{
    Bucket** buckets = calloc(newSize, sizeof(Bucket*));

    size_t i;
    for (i = 0; i < dict->size; i++) {
        Bucket* bucket = dict->buckets[i];

        if (bucket && bucket != TOMBSTONE) {
            const void* key = BucketKeyPtr(bucket, dict->keyType);
            int index = Hash(key, dict->keyType, newSize);
            while (buckets[index] && buckets[index] != TOMBSTONE) 
                index = (index + 1) % newSize;
            buckets[index] = bucket;
        }

    }

    free(dict->buckets);
    dict->buckets = buckets;
    dict->size = newSize;
}
