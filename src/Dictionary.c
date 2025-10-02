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
void MakeDictionary(Pair p1, ...)
{
    
}

void AddHash(Dictionary* dict, Pair p)
{

}
