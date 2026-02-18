#include "Namespace.h"

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