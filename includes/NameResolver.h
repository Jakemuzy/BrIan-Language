#ifndef _NAME_RESOLVER_H__
#define _NAME_RESOLVER_H__

#include "Token.h"
#include "Parser.h"

/* ---------- Helper ---------- */

bool  IsValidDecl(ASTNode* decl);
char* FindIdent(ASTNode* decl);

/* ---------- Symbols ---------- */

/* 
    Need a helper function for resolving symbol type.
    This will aid when I want to add different symbol types dynamically.
*/

typedef enum SymbolType {   
    S_VAR, S_FUNC
} SymbolType;

typedef struct Symbol {
    SymbolType stype;
    char* name; 
    ASTNode* decl;

} Symbol;

Symbol* InitSymbol(ASTNode* decl);
bool    SymbolHasOwnScope(Symbol* sym);

/* Each symbol table is its own "scope" represented by the tree structure */
typedef struct SymbolTable {
    Symbol* symbols;
   
    struct SymbolTable* parent;
    struct SymbolTable* chlidren;
} SymbolTable;

SymbolTable* InitST();
void STPushChild (SymbolTable* st, SymbolTable* st2);
void STPushSymbol(SymbolTable* st, Symbol* sym);

SymbolTable* GenerateSymbolTable(AST* ast);
Symbol*      ResolveBinding(SymbolTable* st, Symbol* sym);

/* Static Name Resolution */
/* Symbol Table */
/* Alpha Renaming? */

/* Scope */
/* Name Spaces */
/* Overloading */
/* Shadowing */


/* Program Scope 
    -> Function Scope
    -> Statement Scope

    Resolve names by checking the lexical level of the syntax level
    If its not there, continue to broader scopes until found. If not
    found it is a name resolver error.

    Goal is to make sure scope is valid and names are valid. Also will 
    convert symbols to a more readible type for the Type Checker.

    The Type Checker will check the output of the symbol table to 
    determine valid typing. And ONLY then, where no Syntax Errors, and 
    no Name Resolution / Type Erros occur, will the AST be passed to the 
    Desugarizer step.
*/

#endif 