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

/* ---------- Symbols ---------- */

typedef enum SymbolType {   
    S_VAR, S_FUNC, S_INDEX, S_CALL, S_FIELD, S_TYPEDEF, S_STRUCT, S_ENUM, S_CTRL, S_ERROR
} SymbolType;

typedef struct Symbol {
    SymbolType stype;
    char* name; 
    ASTNode* decl;

    struct Symbol* prev;    /* TODO: Implement a Stack Separately */
} Symbol;

Symbol* InitSymbol(ASTNode* decl, Symbol* prev);
void    FreeSymbol(Symbol* sym);

/* ---------- Symbol Table ---------- */

#define BUCKET_COUNT_INIT 109

typedef struct SymbolTable {
    Symbol** buckets;

    size_t maxSize;
    size_t currSize;

    //Scope* currentScope;    /* Circular Dependency. Mayhaps namespace holds it */
} SymbolTable;

SymbolTable* STInit();
Symbol* STPop(SymbolTable* env, char* name);
Symbol* STLookup(SymbolTable* env, char* key);
Symbol* STPush(SymbolTable* env, ASTNode* key);
void STResize(SymbolTable* env, unsigned int newSize);

/* ---------- Namespaces ---------- */

#define NAMESPACE_COUNT 2

typedef struct Scope Scope;  // Forward Declaration to prevent circular dependency

typedef enum NamespaceKind {
    N_VAR, N_TYPE, N_NAMESPACE, N_MACRO, N_LIFETIME, N_LABEL    // Based on Rust
} NamespaceKind;

typedef struct NamespaceScope {
    Symbol** symbols;
    size_t symCount;

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
bool LookupNamespaceCurrentScope(Namespace* namespace, char* name);

/* ---------- Scope Logic ---------- */

typedef enum ScopeType { PROG_SCOPE, FUNC_SCOPE, CTRL_SCOPE, INVALID_SCOPE } ScopeType;
typedef struct Scope {
    Namespaces* namespaces;

    struct Scope* prev; 
    ScopeType stype;
} Scope;

/* Adding Namespaces to Scope */
Scope* ScopeInit(size_t count, ...);    /* NamespaceKind */

/* Make this Namespace Instead */
Scope* BeginScope(Scope* scope, ScopeType type);
Scope* ExitScope(Scope* scope);
void PushScope(Scope* scope, Symbol* sym, NamespaceKind nsKind);
bool LookupCurrentScope(Scope* scope, char* name, NamespaceKind nsKind);

SymbolTable* GetSTfromNS(Scope* scope, NamespaceKind kind);

#endif