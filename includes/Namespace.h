#ifndef _NAMESPACE_H__
#define _NAMESPACE_H__

#include "Symbol.h"

/* ---------- Namespaces ---------- */

#define NAMESPACE_COUNT 2

typedef enum NamespaceKind {
    N_VAR, N_TYPE, N_NAMESPACE, N_MACRO, N_LIFETIME, N_LABEL    // Based on Rust
} NamespaceKind;

typedef struct Namespace {   
    SymbolTable* env;
    NamespaceKind kind;

    Symbol** symbols;
    size_t symCount;    /* ATODO: add Dynamic resizing */
} Namespace;

typedef struct Namespaces {
    Namespace** nss;
    size_t count;   /* Set size array, can't imagine more than maybe 6 */
} Namespaces;

Namespace* NamespaceInit(NamespaceKind kind);
SymbolTable* NamespaceGetST(Namespace* ns);

Namespace* BeginNamespaceScope(Namespace* namespace);
void ExitNamespaceScope(Namespace* namespace);
void PushNamespaceScope(Namespace* namespace, Symbol* sym);
Symbol* LookupNamespaceCurrentScope(Namespace* namespace, char* name);

#endif