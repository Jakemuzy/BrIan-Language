#include "Symbol.h"

/* ---------- Symbols ---------- */

Symbol* InitSymbol(ASTNode* decl, Symbol* prev) 
{
    Symbol* sym = malloc(sizeof(Symbol));
    sym->decl = decl;
    sym->name = decl->token.lex.word;
    sym->stype = (decl->type == VAR_DECL_NODE) ? S_VAR : S_FUNC;   /* Determine Type Here */
    sym->prev = prev;

    return sym;
}

/* ---------- Symbol Table ---------- */

SymbolTable* STInit() 
{
    SymbolTable* env = malloc(sizeof(SymbolTable));
    env->buckets = malloc(sizeof(Symbol*) * INIT_SIZE);
    env->maxSize = INIT_SIZE; env->currSize = INIT_SIZE;
    env->currentScope = NULL;

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

Symbol* STPush(SymbolTable* env, ASTNode* key)
{
    char* name = key->token.lex.word;
    int index = Hash(name, HASH_STR, env->maxSize);

    Symbol* sym, *syms = env->buckets[index];
    sym = InitSymbol(key, syms);

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

/* ---------- Scope ---------- */

void BeginScope(SymbolTable* env, ScopeType type)
{
    Scope* newScope = malloc(sizeof(Scope));
    newScope->prev = env->currentScope;
    newScope->symCount = 0; 
    newScope->symbols = NULL;
    newScope->stype = type;

    env->currentScope = newScope;
}

void ExitScope(SymbolTable* env)
{
    Scope* scope = env->currentScope;

    size_t i;
    for (i = 0; i < scope->symCount; i++) {
        Symbol* sym = scope->symbols[i];
        STPop(env, sym->name);
    } 

    env->currentScope = scope->prev;
}

void PushScope(SymbolTable* env, Symbol* sym) 
{
    Scope* scope = env->currentScope;
    size_t symCount = scope->symCount;

    scope->symbols = realloc(scope->symbols, (symCount + 1) * sizeof(Symbol*));     /* Incrementing by one is inefficient, we can fix this later by making a vector DS */
    scope->symbols[symCount] = sym;

    scope->symCount++;
}

bool LookupCurrentScope(SymbolTable* env, char* name)
{
    Scope* scope = env->currentScope;

    int i = 0;
    for (i = 0; i < scope->symCount; i++) {
        Symbol* sym = scope->symbols[i];
        if(0 == strcmp(name, sym->name)) 
            return true;
    }
    return false;
}