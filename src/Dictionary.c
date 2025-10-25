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
    const INITAL_SIZE = 10;

    Dictionary dict;
    dict.buckets = malloc(sizeof(Bucket)*INITAL_SIZE);
    dict.size=INITAL_SIZE;
    dict.count = 0;
    Pair curr;

    AddHash(&dict, p1);

    va_list args;
    while (true){
        curr = va_arg(args,Pair);
        //lowk dk how "varidaic function" works 
        if (curr.first.RawData==NULL && curr.second.RawData==NULL) {
            break;
        }
        AddHash(&dict,curr);
    }
    va_end(args);
    return dict;
}

void AddHash(Dictionary* dict, Pair p)
{
    const double LOAD_FACTOR = 0.75;

    //rebuild
    if(dict->count/dict->size>0.75){

    }
    //
    int hashcode = HashFunction(p);
    if(true){
        dict->count+=1;
    }    



}
