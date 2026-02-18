#ifndef _NAMESPACE_H__
#define _NAMESPACE_H__

#include "Symbol.h"

/* ---------- Namespaces ---------- */

#define NAMESPACE_COUNT 2

typedef enum NamespaceKind {
    N_VAR, N_TYPE, N_NAMESPACE, N_MACRO, N_LIFETIME, N_LABEL    // Based on Rust
} NamespaceKind;

typedef struct NamespaceScope {
    Symbol** symbols;
    size_t symCount;    /* TODO: This can benefit from being a map as well */

    struct NamespaceScope* prev;
} NamespaceScope;

typedef struct Namespace {   
    SymbolTable* env;
    NamespaceKind kind;

    /* Also a Linked List */
    NamespaceScope* nsScope;
} Namespace;

typedef struct Namespaces {
    Namespace** nss;
    size_t count;   /* Set size array, can't imagine more than maybe 6 */
} Namespaces;

Namespace* NamspaceInit(NamespaceKind kind);
SymbolTable* NamespaceGetST(Namespace* ns);

void BeginNamespaceScope(Namespace* namespace);
void ExitNamespaceScope(Namespace* namespace);
void PushNamespaceScope(Namespace* namespace, Symbol* sym);
Symbol* LookupNamespaceCurrentScope(Namespace* namespace, char* name);

#endif