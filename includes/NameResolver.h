#ifndef _NAME_RESOLVER_H__
#define _NAME_RESOLVER_H__

#include "Token.h"
#include "Symbol.h"

/* TODO:
    Make this system more robust
    Allow for function overloading
    Allow for custom type decl / other name spaces

*/

typedef struct TYPE {
    enum {
        TYPE_NULL, TYPE_INT, TYPE_STRING, TYPE_ARR,
        TYPE_VOID, TYPE_DOUBLE, TYPE_FLOAT
    } kind;
    union {
        struct TYPE* array;
        struct {Symbol* sym; struct TYPE* type;} name;
    } u;
} TYPE;

TYPE* TY_NULL(void);
TYPE* TY_INT(void);
TYPE* TY_STRING(void);
TYPE* TY_VOID(void);

TYPE* TY_ARR(TYPE* type);
TYPE* TY_NAME(Symbol* sym, TYPE* type);

/* ---------- Helper ---------- */

bool IdentIsDecl(ASTNode* decl, ASTNode* parent);
bool CanEnterOrExitScope(ASTNode* node) ;

/* ---------- Resolving ---------- */

void ResolveNames(AST* ast);
void ResolveNamesInNode(ASTNode* node, ASTNode* parent);

/* Alpha Renaming? */
/* Name Spaces */
/* Overloading */


#endif 