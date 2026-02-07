#ifndef _SYMBOL_H__
#define _SYMBOL_H__

#include "Dict.h" 
#include "Parser.h"

#define INIT_SIZE 109

/* 
    TODO:
        - One symbol table per namespace (create name spaces)
        - Have SymbolType assign a type to each identifier, that way
          type checker can easily decipher the type
        - Make SymbolTable ues Dictionary for dynamic sizing
*/

/* ---------- Symbols ---------- */

typedef enum SymbolType {   
    S_VAR, S_FUNC, S_INDEX, S_CALL, S_FIELD
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
    size_t symCount;  
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
Symbol* STPop(SymbolTable* ST, char* name);
Symbol* STLookup(SymbolTable* ST, char* key);
Symbol* STPush(SymbolTable* ST, ASTNode* key);
void STResize(SymbolTable* ST, unsigned int newSize);

/* Scope functions depend on ST */
void BeginScope(SymbolTable* ST, ScopeType type);
void ExitScope(SymbolTable* ST);
void PushScope(SymbolTable* ST, Symbol* sym);
bool LookupCurrentScope(SymbolTable* ST, char* name);

#endif