#ifndef _NAME_RESOLVER_H__
#define _NAME_RESOLVER_H__

#include "Token.h"
#include "Symbol.h"

/* ---------- Helper ---------- */

bool  IsValidDecl(ASTNode* decl);
char* FindIdent(ASTNode* decl);


/* Symbol*      ResolveBinding(SymbolTable* st, Symbol* sym); */

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