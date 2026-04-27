#include "Environment.h"

/* ----- Symbol ----- */

int SymbolHash(char* key)
{
    int h = 0;
    for (char* c = key; *c; c++) 
        h = h * 65599 + *c;
    return h;
}

/* ----- Symbol Table ----- */

Environment* InitalizeEnvironment(Arena* arena, NamespaceKind nskind)
{
    // Must malloc since it can grow (lots of wasted space in arena otherwise)   
    Environment* env = AllocateArena(arena, sizeof(Environment));
    env->maxSize = ENV_INIT_SIZE;
    env->currSize = 0;

    env->prev = NULL;
    env->nskind = nskind;
    env->buckets = calloc(env->maxSize, sizeof(Symbol*));
    return env;
}

Symbol* LookupEnvironment(Environment* env, char* key)
{
    int bucket = SymbolHash(key) % env->maxSize;
    return env->buckets[bucket];
}

Symbol* PushEnvironment(Arena* arena, Environment* env, ASTNode* key, SymbolType stype)
{
    char* name = key->token.lexeme;
    int bucket = SymbolHash(name) % env->maxSize;
    Symbol* sym = env->buckets[bucket];

    // Already Exists
    if (sym) {
        int firstCol = sym->node->token.col, firstRow = sym->node->token.row;
        int secondCol = key->token.col, secondRow = key->token.row;
        ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, "Symbol '%s' on line %d, col %d, is already defined. First defined on line %d, col %d.", name, secondRow, secondCol, firstRow, firstCol);
        
        return SYM_ALREADY_EXISTS;
    }

    // Resizing 
    if (env->currSize + 1 > env->maxSize) {
        ResizeEnvironment(env, env->maxSize * 2);
        bucket = SymbolHash(name) % env->maxSize;
    }
    
    sym = AllocateArena(arena, sizeof(Symbol));
    sym->type = NULL;
    sym->node = key;
    sym->stype = stype;

    sym->name = name;

    env->buckets[bucket] = sym;
    env->currSize++;

    key->sym = sym;
    return sym;
}

void DestroyEnvironment(Environment* env)
{
    // Remember: symbols are in the Arena, only free buckets
    free(env->buckets);
}

void ResizeEnvironment(Environment* env, unsigned int newSize)
{
    // Unfortunately, resizing requires manual remapping, thus we pick a nice prime inital size (109)
    Symbol** newBuckets = calloc(newSize, sizeof(Symbol*)); 
    
    for (int i = 0; i < env->maxSize; i++) {
        Symbol* sym = env->buckets[i];
        if (sym) {
            int newIdx = SymbolHash(sym->name) % newSize;
            newBuckets[newIdx] = sym;
        }
    }
    
    free(env->buckets);
    env->buckets = newBuckets;
    env->maxSize = newSize;
}
