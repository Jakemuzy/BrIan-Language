#ifndef _SYMBOL_H__
#define _SYMBOL_H__

/* #include "Dict.h" */
#include "Parser.h"
#include "Dictionary.h"

#define SIZE 109

/* One symbol table per namespace */

/* ---------- Symbols ---------- */

typedef enum SymbolType {   
    S_VAR, S_FUNC
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

/* 
    Essentially a mapping, wherein each bucket is a linked list ( stack ). 
    Resolve an IDENT to a bucket via a hash function and depending on the scope 
    either push to the stack or error

    Typedef ST
    Var Decl ST
        -> Each unique IDENT 
            -> Temporal history of ident
            -> Linked List acting like a stack
        -> Global Scope
            -> Stores all viewable vars in current scopeS
*/

static Symbol* SymbolTable[SIZE];

Symbol* STPop(char* name);
Symbol* STLookup(char* key);
Symbol* STPush(ASTNode* key);

/* ---------- Scope Logic ---------- */

typedef enum ScopeType { PROG_SCOPE, FUNC_SCOPE, CTRL_SCOPE, INVALID_SCOPE } ScopeType;
typedef struct Scope {
    Symbol** symbols;  
    size_t symCount;  
    struct Scope* prev; /* Maybe have ScopeType? For control statements that shouldn't be allowed to have shadowed variables */

    ScopeType stype;
} Scope;

void BeginScope(Scope** currentScope, ScopeType type);
void ExitScope(Scope** currentScope);
void PushScope(Scope** currentScope, Symbol* sym);
bool LookupCurrentScope(Scope** currentScope, char* name);


#endif