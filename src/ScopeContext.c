#include "ScopeContext.h"

/* ---------- Scope Stack ----------*/

ScopeStack InitScopeStack()
{
    ScopeStack stack;
    stack.size = 1;              
    stack.maxSize = 4;           
    stack.stypes = malloc(sizeof(ScopeType) * stack.maxSize);
    return stack;
}

void PushScopeStack(ScopeStack* stack, ScopeType stype)
{
    if (stack->size >= stack->maxSize) {
        stack->maxSize *= 2;
        stack->stypes = realloc(stack->stypes, sizeof(ScopeType) * stack->maxSize);
    }

    size_t old = stack->size++;
    stack->stypes[old] = stype;
}

ScopeType PopScopeStack(ScopeStack* stack)
{
    if (stack->size == 0) return INVALID_SCOPE;  // underflow 

    ScopeType stype = stack->stypes[--stack->size];
    return stype;
}

ScopeType PeekScopeStack(ScopeStack* stack)
{
    if (stack->size == 0) return INVALID_SCOPE;
    return stack->stypes[stack->size - 1];
}

/* ---------- Scope ---------- */

ScopeContext* ScopeInit(size_t count, ...)
{
    ScopeContext* scope = malloc(sizeof(ScopeContext));
    scope->namespaces = malloc(sizeof(Namespaces));
    scope->namespaces->count = count;
    scope->namespaces->nss = calloc(count, sizeof(Namespace*));
    scope->scopeTypes = InitScopeStack();

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

void BeginScope(ScopeContext* scope, ScopeType type)
{
    PushScopeStack(&scope->scopeTypes, type);

    /* Applies new Scope to Namespaces */
    for (size_t i = 0; i < scope->namespaces->count; i++) 
        BeginNamespaceScope(scope->namespaces->nss[i]);
}

void ExitScope(ScopeContext* scope)
{
    PopScopeStack(&scope->scopeTypes);
    for (size_t i = 0; i < scope->namespaces->count; i++) 
        ExitNamespaceScope(scope->namespaces->nss[i]);
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
    for (size_t i = 0; i < scope->namespaces->count; i++) {
        if (scope->namespaces->nss[i]->kind != kind) continue;

        Namespace* ns = scope->namespaces->nss[i];
        NamespaceScope* nsScope = ns->nsScope;
        while (nsScope) {
            for (size_t j = 0; j < nsScope->symCount; j++) {
                Symbol* sym = nsScope->symbols[j];
                if (!sym) continue;
                /* Just use NSLookup here */
                if (0 == strcmp(sym->name, name))
                    return sym;
            }

            nsScope = nsScope->prev;
        }
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