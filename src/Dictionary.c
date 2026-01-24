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

unsigned int HashStr(char* key)
{
    char* str;
    unsigned int hash = 0;
    for (str = key; *str; str++)
        hash = hash * 65599 + *str;
    return hash;
}

Entry* DictLookup(Dict d, char* key)
{
    Entry* np;

    for(np = d[Hash(key)]; np != NULL; np = np->next)
    {
        if(strcmp(np->key, key) == 0)
            return np;
    }
    return NULL;
}

Entry* DictAdd(Dict* d, char* key, int val)
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

    np->key = malloc(strlen(key) + 1);
    if(np->key == NULL)
        return NULL;

    strcpy(np->key, key);
    return np;
}

Dict* DictMake(int count, ...)
{
    Dict* d = calloc(1, sizeof(Dict));

    va_list args;
    va_start(args, count);

    int i;
    for(i = 0; i < count; i++)
    {
        KeyVal* kv = va_arg(args, KeyVal*);
        DictAdd(d, kv->key, kv->val);
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
