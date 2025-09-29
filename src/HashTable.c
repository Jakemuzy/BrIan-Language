#include "DataStructures/Dictionary.h"

/* djb2 Hash Function */
int HashFunction(Pair p)
{
    unsigned char* bytes = first.RawData;
    unsigned long h = 5381;

    for(size_t i = 0; i < p.size; i++)
    {
        h = ((h << 5) + h) + bytes[i];
    }

    return h;
}

/* Hash Table */
void MakeDictionary(Pair p1, ...)
{
    
}

void AddHash(Dictionary* dict, Pair p)
{

}
