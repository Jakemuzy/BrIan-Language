#include "Symbol.h"

/* ---------- Symbols ---------- */

Symbol* InitSymbol(ASTNode* decl, Symbol* prev, TYPE* type) 
{
    Symbol* sym = malloc(sizeof(Symbol));
    sym->decl = decl;
    sym->name = decl->token.lex.word;
    sym->prev = prev;
    sym->type = type;
    switch(decl->type) {
        case(VAR_DECL_NODE): sym->stype = S_VAR; break;
        /* Check parent for type? */
        case(FUNC_NODE) : sym->stype = S_FUNC; break;
        case(ARR_INDEX_NODE) : sym->stype = S_INDEX; break;
        case(CALL_FUNC_NODE) : sym->stype = S_CALL; break;
        case(TYPEDEF_DECL_NODE) : sym->stype = S_TYPEDEF; break;
        case(STRUCT_DECL_NODE) : sym->stype = S_STRUCT; break;
        case(ENUM_DECL_NODE) : sym->stype = S_ENUM; break;
        case(CTRL_SCOPE) : sym->stype = S_CTRL; break;
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

Symbol* STPush(SymbolTable* env, ASTNode* key, TYPE* type)
{
    char* name = key->token.lex.word;
    int index = Hash(name, HASH_STR, env->maxSize);

    Symbol* sym, *syms = env->buckets[index];
    sym = InitSymbol(key, syms, type);

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

Namespace* NamespaceInit(NamespaceKind kind)
{
    Namespace* ns = malloc(sizeof(Namespace));
    ns->env = STInit();
    ns->kind = kind;
    BeginNamespaceScope(ns);

    return ns;
}

void BeginNamespaceScope(Namespace* namespace) 
{
    NamespaceScope* ns = malloc(sizeof(NamespaceScope));
    ns->symbols = NULL;
    ns->symCount = 0;
    ns->prev = namespace->nsScope;
    namespace->nsScope = ns;
}

void ExitNamespaceScope(Namespace* namespace) 
{
    NamespaceScope* nsScope = namespace->nsScope;
    if (!nsScope) return;

    for (size_t j = 0; j < nsScope->symCount; j++) {
        Symbol* sym = nsScope->symbols[j];
        if (!sym) continue;
        STPop(namespace->env, sym->name);
    }

    free(nsScope->symbols);
    namespace->nsScope = nsScope->prev;
}

void PushNamespaceScope(Namespace* namespace, Symbol* sym)
{
    size_t symCount = namespace->nsScope->symCount;

    namespace->nsScope->symbols = realloc(namespace->nsScope->symbols, (symCount + 1) * sizeof(Symbol*));      /* TODO: More elegant resizing */
    namespace->nsScope->symbols[symCount] = sym;

    namespace->nsScope->symCount++;
}

Symbol* LookupNamespaceCurrentScope(Namespace* namespace, char* name)
{
    NamespaceScope* nsScope = namespace->nsScope;
    size_t symCount = nsScope->symCount;

    for (size_t j = 0; j < symCount; j++) {
        Symbol* sym = nsScope->symbols[j];
        if (!sym) continue;
        if (0 == strcmp(name, sym->name))
            return sym;
    }
    return NULL;
}

/* ---------- Scope ---------- */

Scope* ScopeInit(size_t count, ...)
{
    Scope* scope = malloc(sizeof(Scope));
    scope->namespaces = malloc(sizeof(Namespaces));
    scope->namespaces->count = count;
    scope->namespaces->nss = calloc(count, sizeof(Namespace*));
    scope->prev = NULL;
    scope->stype = PROG_SCOPE;

    va_list args;
    va_start(args, count);

    for (size_t i = 0; i < count; i++) {
        NamespaceKind kind = va_arg(args, NamespaceKind);
        Namespace* ns = NamespaceInit(kind);
        scope->namespaces->nss[i] = ns;
    }

    va_end(args);
    return scope;
}

Scope* BeginScope(Scope* scope, ScopeType type)
{
    Scope* newScope = malloc(sizeof(Scope));
    newScope->prev = scope;
    newScope->namespaces = scope->namespaces;
    newScope->stype = type;

    for (size_t i = 0; i < newScope->namespaces->count; i++) {
        BeginNamespaceScope(newScope->namespaces->nss[i]);
    }
    return newScope;
}

Scope* ExitScope(Scope* scope)
{
    Scope* prev = scope->prev;
    for (size_t i = 0; i < scope->namespaces->count; i++) {
        Namespace* ns = scope->namespaces->nss[i];
        ExitNamespaceScope(ns);
    }
    free(scope);
    return prev;
}

void PushScope(Scope* scope, Symbol* sym, NamespaceKind nsKind) 
{
    /* TODO: Use find namespace */
    for (size_t i = 0; i < scope->namespaces->count; i++) {
        if (scope->namespaces->nss[i]->kind != nsKind) continue;

        Namespace* ns = scope->namespaces->nss[i];
        PushNamespaceScope(ns, sym);
    }
}

Symbol* LookupCurrentScope(Scope* scope, char* name, NamespaceKind nsKind)
{
    for (size_t i = 0; i < scope->namespaces->count; i++) {
        if (scope->namespaces->nss[i]->kind != nsKind) continue;

        Namespace* ns = scope->namespaces->nss[i];
        return LookupNamespaceCurrentScope(ns, name);
    }

    return NULL;
}

Symbol* LookupAllScopes(Scope* scope, char* name, NamespaceKind kind)
{
    for (size_t i = 0; i < scope->namespaces->count; i++) {
        if (scope->namespaces->nss[i]->kind != kind) continue;

        Namespace* ns = scope->namespaces->nss[i];
        NamespaceScope* nsScope = ns->nsScope;
        while (nsScope) {
            for (size_t j = 0; j < nsScope->symCount; j++) {
                Symbol* sym = nsScope->symbols[j];
                if (!sym) continue;

                if (0 == strcmp(sym->name, name))
                    return sym;
            }

            nsScope = nsScope->prev;
        }
    }

    return NULL;
}

Symbol* STPushNamespace(Scope* scope, ASTNode* key, NamespaceKind kind, TYPE* type)
{
    for (size_t i = 0; i < scope->namespaces->count; i++) {
        if (scope->namespaces->nss[i]->kind != kind) continue;

        Namespace* ns = scope->namespaces->nss[i];
        return STPush(ns->env, key);
    }

    return NULL;
}
