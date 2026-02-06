#ifndef _SYMBOL_H__
#define _SYMBOL_H__

#include "Dict.h" 
#include "Parser.h"

#define SIZE 109

/* 
    TODO:
        - One symbol table per namespace (create name spaces)
        - Have SymbolType assign a type to each identifier, that way
          type checker can easily decipher the type
*/

/* ---------- Symbols ---------- */

typedef enum SymbolType {   
    S_VAR, S_FUNC, S_INDEX, S_CALL
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

static Symbol* ST[SIZE];   /* TODO: Change to extern */

Symbol* STPop(char* name);
Symbol* STLookup(char* key);
Symbol* STPush(ASTNode* key);

/* ---------- Scope Logic ---------- */

typedef enum ScopeType { PROG_SCOPE, FUNC_SCOPE, CTRL_SCOPE, INVALID_SCOPE } ScopeType;
typedef struct Scope {
    Symbol** symbols;  
    size_t symCount;  
    struct Scope* prev; 

    ScopeType stype;
} Scope;

void BeginScope(Scope** currentScope, ScopeType type);
void ExitScope(Scope** currentScope);
void PushScope(Scope** currentScope, Symbol* sym);
bool LookupCurrentScope(Scope** currentScope, char* name);

#endif