#include "Symbol.h"

/* ---------- Symbols ---------- */

Symbol* InitSymbol(ASTNode* decl, Symbol* prev, SymbolType stype) 
{
    Symbol* sym = malloc(sizeof(Symbol));
    sym->decl = decl;
    sym->name = decl->token.lex.word;
    sym->prev = prev;
    sym->type = NULL;
    sym->stype = stype;

    return sym;
}

/* ---------- Symbol Table ---------- */

SymbolTable* STInit() 
{
    SymbolTable* env = malloc(sizeof(SymbolTable));
    env->buckets = calloc(INIT_SIZE, sizeof(Symbol*));
    env->maxSize = INIT_SIZE; env->currSize = INIT_SIZE;

    return env;
}

Symbol* STLookup(SymbolTable* env, char* name)
{
    int index = Hash(name, HASH_STR, env->maxSize);
    Symbol* sym, *syms = env->buckets[index];
    for (sym=syms; sym; sym=sym->prev)
        if (0 == strcmp(sym->name, name)) return sym;

    return sym;
}

Symbol* STPush(SymbolTable* env, ASTNode* key, SymbolType stype)
{
    char* name = key->token.lex.word;
    int index = Hash(name, HASH_STR, env->maxSize);

    Symbol* sym, *syms = env->buckets[index];
    sym = InitSymbol(key, syms, stype);

    env->buckets[index] = sym;
    return sym;
}

Symbol* STPop(SymbolTable* env, char* name)
{
    int index = Hash(name, HASH_STR, env->maxSize);
    Symbol* top = env->buckets[index];
    if (!top)
        return NULL;
    
    env->buckets[index] = top->prev;
    return top;
}
