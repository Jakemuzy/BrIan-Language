#include "DataStructures/Dictionary.h"

/* Random Hash I found on Stack Overflow */
unsigned int Hash(unsigned int key)
{
    key = ((key >> 16) ^ key) * 0x45d9f3bu;
    key = ((key >> 16) ^ key) * 0x45d9f3bu;
    key =  (key >> 16) ^ key;
    return key;
}

Entry* DictLookup(Dict d, int key)
{
    Entry* np;

    for(np = d[Hash(key)]; np != NULL; np = np->next)
    {
        if(np->key == key)
            return np;
        return NULL;
    }
}

Entry* DictInstall(Dict d, int key, char* val)
{
    Entry* np;
    unsigned int hashval;

    if((np = DictLookup(d, key)) == NULL)
    {
        np = (Entry*)malloc(sizeof(*np));
        if(np = NULL)
            return NULL;

        np->key = key;
        hashval = Hash(key);
        np->next = d[hashval];
        d[hashval] = np;
    } 
    else 
    {
        free((void *) np->val);
    
    if((np->val = val) == NULL)
        return NULL;
    return np;

    }

}

Dict* DictMake(int count, ...)
{
    Dict* d = malloc(sizeof(Dict));;

    va_list args;
    va_start(args, count);

    int i;
    for(i = 0; i < count; i++)
    {
        KeyVal kv = va_arg(args, KeyVal);
        DictInstall(*d, kv.key, kv.val);
    }

    va_end(args);
    return d;
}

void DictPrint(Dict d)
{
    int i;
    for(i = 0; i < DICT_CAP; i++)
    {
        Entry* e = d[i];
        while(e != NULL)
        {
            printf("Key: %d\tVal: %s\n", e->key, e->val);
            e = e->next;
        }
    }
}
