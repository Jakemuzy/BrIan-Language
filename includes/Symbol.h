#ifndef _SYMBOL_H__
#define _SYMBOL_H__

#include <stdarg.h>

#include "Dict.h" 
#include "Parser.h"

/* 
    TODO:
        - One symbol table per namespace (create name spaces / environment )
        - Have SymbolType assign a type to each identifier, that way
          type checker can easily decipher the type
        - Make SymbolTable ues Dictionary for dynamic sizing
*/

/* ---------- Forward Declaration Type Info ---------- */
typedef struct TYPE TYPE;
typedef struct TYPE_LIST TYPE_LIST;
typedef struct TYPE_FIELD TYPE_FIELD;
typedef struct TYPE_FIELD_LIST TYPE_FIELD_LIST;

/* Constructors */
TYPE* TY_ERROR(void);
TYPE* TY_VOID(void);
TYPE* TY_INT(void);
TYPE* TY_FLOAT(void);
TYPE* TY_DOUBLE(void);
TYPE* TY_BOOL(void);
TYPE* TY_STRING(void);

TYPE* TY_ARR(TYPE* element, int size);
TYPE* TY_NAME(Symbol* sym, TYPE* type);

/* ---------- Symbols ---------- */

typedef enum SymbolType {   
    S_VAR, S_FUNC, S_INDEX, S_CALL, S_FIELD, S_TYPEDEF, S_STRUCT, S_ENUM, S_CTRL, S_ERROR
} SymbolType;

typedef struct Symbol {
    char* name; 
    ASTNode* decl;
    struct Symbol* prev;    

    /* Data Type and actual Symbol Type */
    TYPE* type;
    SymbolType stype;
} Symbol;

Symbol* InitSymbol(ASTNode* decl, Symbol* prev, TYPE* type);
void    FreeSymbol(Symbol* sym);

/* ---------- Symbol Table ---------- */

#define BUCKET_COUNT_INIT 109

typedef struct SymbolTable {
    Symbol** buckets;

    size_t maxSize;
    size_t currSize;
} SymbolTable;

SymbolTable* STInit();
Symbol* STPop(SymbolTable* env, char* name);
Symbol* STLookup(SymbolTable* env, char* key);
Symbol* STPush(SymbolTable* env, ASTNode* key, TYPE* type);
void STResize(SymbolTable* env, unsigned int newSize);

/* ---------- Namespaces ---------- */

#define NAMESPACE_COUNT 2

typedef struct Scope Scope;  // Forward Declaration to prevent circular dependency

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

/* ---------- Scope Logic ---------- */

typedef enum ScopeType { PROG_SCOPE, FUNC_SCOPE, CTRL_SCOPE, INVALID_SCOPE } ScopeType;
typedef struct Scope {
    Namespaces* namespaces;

    struct Scope* prev; 
    ScopeType stype;
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