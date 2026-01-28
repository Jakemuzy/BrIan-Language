#ifndef _NAME_RESOLVER_H__
#define _NAME_RESOLVER_H__

#include "Token.h"
#include "Symbol.h"

/* TODO:
    Make this system more robust
    Allow for function overloading
    Allow for custom type decl / other name spaces

*/

/* ---------- Error Handling ---------- */

#define ERRN false
#define VALDN true

bool NERROR_NO_IDENT(ASTNode* curr);
bool NERROR_ALREADY_DEFINED(char* name, ASTNode* curr, ASTNode* first);
bool NERROR_DOESNT_EXIST(char* name, ASTNode* curr);

/* ---------- Helper ---------- */

ASTNode* FindIdentChild(ASTNode* node);
bool IdentIsDecl(ASTNode* decl, ASTNode* parent);
bool CanEnterOrExitScope(ASTNode* node) ;

/* ---------- Resolving ---------- */

Symbol** ResolveNames(AST* ast);
bool ResolveNamesInNode(ASTNode* node, ASTNode* parent);

/* Alpha Renaming? */
/* Name Spaces */
/* Overloading */


#endif 