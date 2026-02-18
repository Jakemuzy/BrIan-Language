#ifndef _SCOPE_CONTEXT_H__
#define _SCOPE_CONTEXT_H__

#include "Namespaces.h"

/* ---------- Scope Logic ---------- */

typedef enum ScopeType { PROG_SCOPE, FUNC_SCOPE, CTRL_SCOPE, INVALID_SCOPE } ScopeType;

typedef struct ScopeStack {
    size_t size;
    ScopeType stypes[];
} ScopeStack;

typedef struct Scope { /* Scope -> ScopeContext */
    Namespaces* namespaces;

    /* TODO: URGENT could simply have this as a stack of symbol types
       don't need to introduce the complexity of having a linked list 
       of namespaces inside of a linked list of scopes 
    */
    struct Scope* prev; 
    ScopeType stype;

    ScopeStack scopeTypes;
} Scope;

/* Adding Namespaces to Scope */
Scope* ScopeInit(size_t count, ...);    /* NamespaceKind */

/* Scope logic for global Scope struct */
Scope* BeginScope(Scope* scope, ScopeType type);
Scope* ExitScope(Scope* scope);
void PushScope(Scope* scope, Symbol* sym, NamespaceKind nsKind);

/* Namespace operations for namespace scope */
Symbol* LookupCurrentScope(Scope* scope, char* name, NamespaceKind nsKind);
Symbol* LookupAllScopes(Scope* scope, char* name, NamespaceKind kind);
Symbol* STPushNamespace(Scope* scope, ASTNode* key, NamespaceKind kind, TYPE* type);

/* For type checker */
Symbol* STLookupNamespace(Namespaces* nss, char* name, NamespaceKind kind);

#endif