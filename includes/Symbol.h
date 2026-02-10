#ifndef _SYMBOL_H__
#define _SYMBOL_H__

#include "Dict.h" 
#include "Parser.h"

#define INIT_SIZE 109

/* 
    TODO:
        - One symbol table per namespace (create name spaces / environment )
        - Have SymbolType assign a type to each identifier, that way
          type checker can easily decipher the type
        - Make SymbolTable ues Dictionary for dynamic sizing
*/

/* ---------- Symbols ---------- */

typedef enum SymbolType {   
    S_VAR, S_FUNC, S_INDEX, S_CALL, S_FIELD, S_TYPEDEF, S_STRUCT, S_ENUM
} SymbolType;

typedef struct Symbol {
    SymbolType stype;
    char* name; 
    ASTNode* decl;

    struct Symbol* prev;    /* TODO: Implement a Stack Separately */
} Symbol;

Symbol* InitSymbol(ASTNode* decl, Symbol* prev);
void    FreeSymbol(Symbol* sym);

/* ---------- Scope Logic ---------- */

typedef enum ScopeType { PROG_SCOPE, FUNC_SCOPE, CTRL_SCOPE, INVALID_SCOPE } ScopeType;
typedef struct Scope {
    Symbol** symbols;  
    Symbol** typedefs;

    size_t symCount;  
    size_t typCount;

    struct Scope* prev; 
    ScopeType stype;
} Scope;

/* ---------- Symbol Table ---------- */

typedef struct SymbolTable {
    Symbol** buckets;

    size_t maxSize;
    size_t currSize;

    Scope* currentScope;
} SymbolTable;

SymbolTable* STInit();
Symbol* STPop(SymbolTable* env, char* name);
Symbol* STLookup(SymbolTable* env, char* key);
Symbol* STPush(SymbolTable* env, ASTNode* key);
void STResize(SymbolTable* env, unsigned int newSize);

/* ---------- Scope Functions ---------- */

/* Scope functions depend on ST */
void BeginScope(SymbolTable* env, ScopeType type);
void ExitScope(SymbolTable* env);
void PushScope(SymbolTable* env, Symbol* sym);
bool LookupCurrentScope(SymbolTable* env, char* name);

/* ---------- Environments ---------- */

/* TODO: For later if I want to have many environments */
/*
typedef enum NamespaceKind {
    E_VAR, E_TYPE, E_NAMESPACE, E_MACRO
} NamespaceKind;

typedef struct Namespace {   
    SymbolTable** env;
    size_t count;
} Namespace;

typedef struct Scope {
    Namespace** namespaces;  
    size_t namespaceCount;  

    struct Scope* prev; 
    ScopeType stype;
} Scope;
 */

#endif