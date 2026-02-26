#ifndef _SCOPE_CONTEXT_H__
#define _SCOPE_CONTEXT_H__

#include "Namespace.h"

/* ---------- Scope Logic ---------- */

typedef enum ScopeType { PROG_SCOPE, FUNC_SCOPE, CTRL_SCOPE, STRUCT_SCOPE, ENUM_SCOPE, INVALID_SCOPE } ScopeType;

typedef struct ScopeContext { 
    Namespaces* namespaces;
    struct ScopeContext* prev;

    ScopeType stype;
} ScopeContext;

ScopeContext* ScopeInit(ScopeType type, size_t count, ...);   
void BeginScope(ScopeContext** scope, ScopeType type);
void ExitScope(ScopeContext** scope);
void PushScope(ScopeContext* scope, Symbol* sym, NamespaceKind nsKind);

/* For scopes that shouldn't be freed (ie struct members) */
void BeginPersistentScope(ScopeContext** scope, ScopeType type);
void ExitPersistentScope(ScopeContext** scope);

/* ---------- Namespace Scope ----------- */

Symbol* LookupCurrentScope(ScopeContext* scope, char* name, NamespaceKind nsKind);
Symbol* LookupAllScopes(ScopeContext* scope, char* name, NamespaceKind kind);
Symbol* STPushNamespace(ScopeContext* scope, ASTNode* key, NamespaceKind kind);

/* For type checker */
Symbol* STLookupNamespace(Namespaces* nss, char* name, NamespaceKind kind);

#endif