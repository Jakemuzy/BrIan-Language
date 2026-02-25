#include "ScopeContext.h"

/* ---------- Scope ---------- */

ScopeContext* ScopeInit(ScopeType type, size_t count, ...)
{
    ScopeContext* scope = malloc(sizeof(ScopeContext));
    scope->stype = type;
    scope->prev = NULL;

    scope->namespaces = malloc(sizeof(Namespaces));
    scope->namespaces->count = count;
    scope->namespaces->nss = calloc(count, sizeof(Namespace*));

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

void BeginScope(ScopeContext** scope, ScopeType type)
{
    ScopeContext* newScope = malloc(sizeof(ScopeContext));
    newScope->stype = type;
    newScope->prev = *scope;  

    size_t count = (*scope)->namespaces->count;
    newScope->namespaces = malloc(sizeof(Namespaces));
    newScope->namespaces->count = count;
    newScope->namespaces->nss = calloc(count, sizeof(Namespace*));

    for (size_t i = 0; i < count; i++) {
        NamespaceKind kind = (*scope)->namespaces->nss[i]->kind;
        newScope->namespaces->nss[i] = BeginNamespaceScope((*scope)->namespaces->nss[i]);
    }

    *scope = newScope;
}

void ExitScope(ScopeContext** scope)
{
    ScopeContext* prev = (*scope)->prev;
    for (size_t i = 0; i < (*scope)->namespaces->count; i++) {

        Namespace* ns = (*scope)->namespaces->nss[i];
        ExitNamespaceScope(ns); /* Frees symbols keeps env */
        free(ns);
    }
    free((*scope)->namespaces->nss);
    free((*scope)->namespaces);
    free(*scope);
    
    *scope = prev;
}

void PushScope(ScopeContext* scope, Symbol* sym, NamespaceKind nsKind) 
{
    /* TODO: Use find namespace */
    for (size_t i = 0; i < scope->namespaces->count; i++) {
        if (scope->namespaces->nss[i]->kind != nsKind) continue;

        Namespace* ns = scope->namespaces->nss[i];
        PushNamespaceScope(ns, sym);
    }
}

void BeginPersistentScope(ScopeContext** scope, ScopeType type)
{
    /* Logical wrapper to avoid confusion between scope begin/exit mismatch */
    BeginScope(scope, type);
}

void ExitPersistentScope(ScopeContext** scope)
{
    ScopeContext* prev = (*scope)->prev;
    free(*scope);
    
    *scope = prev;
}

/* ---------- Namespace Scope ----------- */

Symbol* LookupCurrentScope(ScopeContext* scope, char* name, NamespaceKind nsKind)
{
    for (size_t i = 0; i < scope->namespaces->count; i++) {
        if (scope->namespaces->nss[i]->kind != nsKind) continue;

        Namespace* ns = scope->namespaces->nss[i];
        return LookupNamespaceCurrentScope(ns, name);
    }

    return NULL;
}

Symbol* LookupAllScopes(ScopeContext* scope, char* name, NamespaceKind kind)
{
    ScopeContext* current = scope;
    while (current) {
        Symbol* sym = LookupCurrentScope(current, name, kind);
        if (sym) 
            return sym;
        current = current->prev;
    }

    return NULL;
}

Symbol* STPushNamespace(ScopeContext* scope, ASTNode* key, NamespaceKind kind, TYPE* type, char* typeLex)
{
    for (size_t i = 0; i < scope->namespaces->count; i++) {
        if (scope->namespaces->nss[i]->kind != kind) continue;

        Namespace* ns = scope->namespaces->nss[i];
        return STPush(ns->env, key, type, typeLex);
    }

    return NULL;
}

/* Type Checker Stuff */
Symbol* STLookupNamespace(Namespaces* nss, char* name, NamespaceKind kind)
{
    for (size_t i = 0; i < nss->count; i++) {
        if (nss->nss[i]->kind != kind) continue;
        Symbol* sym = STLookup(nss->nss[i]->env, name);
        if (sym)
            return sym;
        /* TODO: Weirdly ST should be the same, but it depends on scope looks like */
    }
    return NULL;
}