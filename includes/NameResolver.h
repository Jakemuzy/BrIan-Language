#ifndef _NAME_RESOLVER_H__
#define _NAME_RESOLVER_H__

#include "Token.h"
#include "Symbol.h"

/* TODO:
    Make this system more robust
    Allow for function overloading
    Allow for custom type decl / other name spaces
    Account for break / continue statements
    Account for return statements

    These files need a lot of cleanup, lets start 
    once we have everything fully functional
*/

/* ---------- Error Handling ---------- */

#define ERRN 0
#define VALDN 1
#define NANN 2  /* Not Applicable */

int NERROR_NO_IDENT(ASTNode* curr);
int NERROR_ALREADY_DEFINED(char* name, ASTNode* curr, ASTNode* first);
int NERROR_DOESNT_EXIST(char* name, ASTNode* curr);

/* ---------- Resolving ---------- */

Namespaces* ResolveNames(AST* ast);
int ResolveEverything(Scope* scope, ASTNode* current);


int ResolveTypedefs(Scope* scope, ASTNode* current);

int ResolveVars(Scope* scope, ASTNode* current);
int ResolveVar(Scope* scope, ASTNode* current, TYPE* type);

int ResolveFuncs(Scope** scope, ASTNode* current);
int ResolveParams(Scope* scope, ASTNode* current);
int ResolveParam(Scope* scope, ASTNode* current);

int ResolveExprs(Scope* scope, ASTNode* current);
int ResolveExpr(Scope* scope, ASTNode* current);
int ResolveStmts(Scope** scope, ASTNode* current);
int ResolveStmt(Scope* scope, ASTNode* current);

int ResolveFuncCall(Scope* scope, ASTNode* current);
int ResolveArrIndex(Scope* scope, ASTNode* current);
int ResolveArgList();

int EnterScopeIfNeeded(Scope** scope, ASTNode* current);

/* ---------- Helpers ---------- */

static NodeType CTRL_STMTS[] = { IF_NODE, ELIF_NODE, ELSE_NODE, SWITCH_STMT_NODE,
                                 CASE_NODE, DEFAULT_NODE, WHILE_STMT_NODE, 
                                 DO_WHILE_STMT_NODE, FOR_STMT_NODE };
static int CTRL_STMTS_SIZE = sizeof(CTRL_STMTS) / sizeof(CTRL_STMTS[0]);

/* Alpha Renaming? */
/* Name Spaces */
/* Overloading */


#endif 