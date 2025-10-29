#include "DataStructures/Dictionary.h"

/* djb2 Hash Function */
int HashFunction(Pair p)
{
    unsigned char* bytes = p.first.RawData;
    unsigned long h = 5381;
    
    size_t i = 0;
    for(i; i < sizeof(bytes); i++)
    {
        h = ((h << 5) + h) + bytes[i];
    }

    return h;
}

/* Hash Table */
Dictionary MakeDictionary(Pair p1, ...)
{
    Dictionary dict;
    dict.numElements = 0;
    dict.buckets = (Bucket**)malloc(sizeof(Bucket*) * DICT_INIT_CAP);
    
    return dict;
}

void Insert(Dictionary* dict, Pair p)
{
    int bucketIndex;

    if(dict->numElements < DICT_MAX_CAP)
        dict->numElements++;
    else 
    {
        perror("Dictionary at max capacity\n");
        return;
    }

    
    bucketIndex = HashFunction(p);
    Bucket* b = malloc(sizeof(Bucket));
    /*setBucket(b, p);*/
}
