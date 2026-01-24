#ifndef _NAME_RESOLVER_H__
#define _NAME_RESOLVER_H__

#include "Token.h"
#include "Symbol.h"

/* TODO:
    Make this system more robust
    Allow for function overloading
    Allow for custom type decl / other name spaces

*/

/* ---------- Helper ---------- */

bool IdentIsDecl(ASTNode* decl, ASTNode* parent);

/* Symbol*      ResolveBinding(SymbolTable* st, Symbol* sym); */

void ResolveNames(AST* ast);
void ResolveNamesInNode(ASTNode* node, ASTNode* parent);

/* Alpha Renaming? */
/* Name Spaces */
/* Overloading */


#endif 