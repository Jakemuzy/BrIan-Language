#ifndef _SYMBOL_H__
#define _SYMBOL_H__

/* #include "Dict.h" */
#include "Parser.h"

/* ---------- Symbols ---------- */

typedef enum SymbolType {   
    S_VAR, S_FUNC
} SymbolType;

typedef struct Symbol {
    SymbolType stype;
    char* name; 
    ASTNode* decl;

    struct Symbol* next;
} Symbol;

Symbol* InitSymbol(ASTNode* decl, Symbol* next);
bool    SymbolHasOwnScope(Symbol* sym);

/* ---------- Scope Logic ---------- */

#define SIZE 109

#define SYM_MARK "<mark>"
static Symbol Scope[SIZE];

/* ---------- Symbol Table ---------- */

/* 
    Essentially a mapping, wherein each bucket is a linked list ( stack ). 
    Resolve an IDENT to a bucket via a hash function and depending on the scope 
    either push to the stack or error
*/

static Symbol* SymbolTable[SIZE];

Symbol* STPop(char* name);
void    STPush(ASTNode* key);
Symbol* STLookup(ASTNode* key);

void STBeginScope(Symbol* ST[]);
void STEndScope(Symbol* ST[]);
#endif