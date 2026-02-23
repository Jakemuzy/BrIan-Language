#include "Namespace.h"

/* ---------- Namespace Scope ---------- */

Namespace* NamespaceInit(NamespaceKind kind)
{
    Namespace* ns = malloc(sizeof(Namespace));
    ns->env = STInit();
    ns->kind = kind;

    ns->symbols = NULL;
    ns->symCount = 0;

    return ns;
}

Namespace* BeginNamespaceScope(Namespace* namespace) 
{
    /* SHOULD COPY ENV BUT new SYMBOLS AND KIND */
    Namespace* ns = malloc(sizeof(Namespace));
    ns->env = namespace->env;
    ns->kind = namespace->kind;

    ns->symbols = NULL;
    ns->symCount = 0;

    return ns;
}

void ExitNamespaceScope(Namespace* namespace) 
{
    for (size_t j = 0; j < namespace->symCount; j++) {
        Symbol* sym = namespace->symbols[j];
        if (!sym) continue;
        STPop(namespace->env, sym->name);
    }

    free(namespace->symbols);
}

void PushNamespaceScope(Namespace* namespace, Symbol* sym)
{
    size_t symCount = namespace->symCount;

    /* TODO: More elegant resizing */
    namespace->symbols = realloc(namespace->symbols, (symCount + 1) * sizeof(Symbol*));      
    namespace->symbols[symCount] = sym;

    namespace->symCount++;
}

Symbol* LookupNamespaceCurrentScope(Namespace* namespace, char* name)
{
    size_t symCount = namespace->symCount;

    for (size_t j = 0; j < symCount; j++) {
        Symbol* sym = namespace->symbols[j];
        if (!sym) continue;
        if (0 == strcmp(name, sym->name))
            return sym;
    }
    return NULL;
}