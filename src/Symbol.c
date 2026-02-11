#include "Symbol.h"

/* ---------- Symbols ---------- */

Symbol* InitSymbol(ASTNode* decl, Symbol* prev) 
{
    Symbol* sym = malloc(sizeof(Symbol));
    sym->decl = decl;
    sym->name = decl->token.lex.word;
    sym->prev = prev;
    sym->stype = (decl->type == VAR_DECL_NODE) ? S_VAR : S_FUNC;   /* Determine Type Here */
    switch(decl->type) {
        case(VAR_DECL_NODE): sym->stype = S_VAR; break;
        case(FUNC_NODE) : sym->stype = S_FUNC; break;
        case(ARR_INDEX_NODE) : sym->stype = S_INDEX; break;
        case(CALL_FUNC_NODE) : sym->stype = S_CALL; break;
        case(TYPEDEF_DECL_NODE) : sym->stype = S_TYPEDEF; break;
        case(STRUCT_DECL_NODE) : sym->stype = S_STRUCT; break;
        case(ENUM_DECL_NODE) : sym->stype = S_ENUM; break;
        default: sym->stype = S_ERROR; break;
    }

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

/* ---------- Namespace Scope ---------- */

void BeginNamespaceScope(Namespace* namespace) 
{
    NamespaceScope* ns = malloc(sizeof(NamespaceScope));
    ns->symbols = NULL;
    ns->symCount = 0;
    ns->prev = namespace->scope;
    namespace->scope = ns;
}

void ExitNamespaceScope(Namespace* namespace) 
{
    NamespaceScope* nsScope = namespace->scope;

    for (size_t j = 0; j < nsScope->symCount; j++) {
        Symbol* sym = nsScope->symbols[j];
        STPop(namespace, sym->name);
    }
}

void PushNamespaceScope(Namespace* namespace, Symbol* sym)
{
    size_t symCount = namespace->scope->symCount;

    namespace->scope->symbols = realloc(namespace->scope->symbols, (symCount + 1) * sizeof(Symbol*));      /* TODO: More elegant resizing */
    namespace->scope->symbols[symCount] = sym;

    namespace->scope->symCount++;
}

bool LookupNamespaceCurrentScope(Namespace* namespace, char* name)
{
    NamespaceScope* nsScope = namespace->scope;
    size_t symCount = nsScope->symCount;

    for (size_t j = 0; j < symCount; j++) {
        Symbol* sym = nsScope->symbols[j];
        if (0 == strcmp(name, sym->name))
            return true;
    }
    return false;
}

/* ---------- Scope ---------- */

Scope* BeginScope(Scope* scope, ScopeType type)
{
    Scope* newScope = malloc(sizeof(Scope));
    newScope->prev = scope;
    newScope->namespaceCount = scope->namespaceCount;
    newScope->namespaces = scope->namespaces;
    newScope->stype = type;

    for (size_t i = 0; i < newScope->namespaceCount; i++) {
        BeginNamespaceScope(newScope->namespaces[i]);
    }
    return newScope;
}

void ExitScope(Scope* scope)
{
    for (size_t i = 0; i < scope->namespaceCount; i++) {
        Namespace* ns = scope->namespaces[i];
        ExitNamespaceScope(ns);
    }
}

void PushScope(Scope* scope, Symbol* sym, NamespaceKind nsKind) 
{

    /* TODO: Use find namespace */
    for (size_t i = 0; i < scope->namespaceCount; i++) {
        if (scope->namespaces[i]->kind != nsKind) continue;

        Namespace* ns = scope->namespaces[i];
        PushNamespaceScope(ns, sym);
    }
}

bool LookupCurrentScope(Scope* scope, char* name, NamespaceKind nsKind)
{
    for (size_t i = 0; i < scope->namespaceCount; i++) {
        if (scope->namespaces[i]->kind != nsKind) continue;

        Namespace* ns = scope->namespaces[i];
        return LookupNamespaceCurrentScope(ns, name);
    }

    return false;
}