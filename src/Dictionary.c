#include "Dictionary.h"

/* Random Hash I found on Stack Overflow */
unsigned int Hash(char* key)
{
    unsigned long hash = 5381;
    int c;

    while (c = *key++)
        hash = ((hash << 5) + hash) + c;

    return hash % DICT_CAP;
}

Entry* DictLookup(Dict d, char* key)
{
    Entry* np;

    for(np = d[Hash(key)]; np != NULL; np = np->next)
    {
        if(np->key == key)
            return np;
        return NULL;
    }
}

Entry* DictInstall(Dict* d, char* key, int val)
{
    Entry* np;
    unsigned int hashval;

    if((np = DictLookup((*d), key)) == NULL)
    {
        np = (Entry*)malloc(sizeof(*np));
        if(np == NULL)
            return NULL;
		
        np->val = val;
        hashval = Hash(key);
        np->next = (*d)[hashval];
        (*d)[hashval] = np;
    } 
    else 
        free((void *) np->key);

    if((np->key = key) == NULL)
        return NULL;

    return np;
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
        DictInstall(d, kv.key, kv.val);
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
            printf("Key: %s  \tVal: %d\n", e->key, e->val);
            e = e->next;
        }
    }
}
