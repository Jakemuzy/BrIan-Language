#ifndef _SCOPE_CONTEXT_H__
#define _SCOPE_CONTEXT_H__

#include "Namespace.h"

/* ---------- Scope Logic ---------- */

typedef enum ScopeType { PROG_SCOPE, FUNC_SCOPE, CTRL_SCOPE, STRUCT_SCOPE, ENUM_SCOPE, INVALID_SCOPE } ScopeType;

typedef struct ScopeStack {
    size_t size;
    size_t maxSize;
    ScopeType* stypes;
} ScopeStack;

ScopeStack InitScopeStack();
void PushScopeStack(ScopeStack* stack, ScopeType stype);
ScopeType PopScopeStack(ScopeStack* stack);
ScopeType PeekScopeStack(ScopeStack* stack);

typedef struct ScopeContext { 
    Namespaces* namespaces;
    struct ScopeContext* prev;

    ScopeType stype;
    size_t size;
    size_t maxSize;
} ScopeContext;

ScopeContext* ScopeInit(size_t count, ...);   
void BeginScope(ScopeContext* scope, ScopeType type);
void ExitScope(ScopeContext* scope);
void PushScope(ScopeContext* scope, Symbol* sym, NamespaceKind nsKind);

/* ---------- Namespace Scope ----------- */

Symbol* LookupCurrentScope(ScopeContext* scope, char* name, NamespaceKind nsKind);
Symbol* LookupAllScopes(ScopeContext* scope, char* name, NamespaceKind kind);
Symbol* STPushNamespace(ScopeContext* scope, ASTNode* key, NamespaceKind kind, TYPE* type, char* typeLex);

/* For type checker */
Symbol* STLookupNamespace(Namespaces* nss, char* name, NamespaceKind kind);

#endif