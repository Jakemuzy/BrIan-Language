#ifndef _NAME_RESOLVER_H__
#define _NAME_RESOLVER_H__

#define _POSIX_C_SOURCE 200809L
#include <string.h>

#include "Token.h"
#include "ScopeContext.h"

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

int NERROR_UNDEFINED_TYPE(char* name, ASTNode* curr);
int NERROR(char* msg, char* name, ASTNode* curr);

/* ---------- Resolving ---------- */

Namespaces* ResolveNames(AST* ast);
int ResolveEverything(ScopeContext* scope, ASTNode* current);


int ResolveVars(ScopeContext* scope, ASTNode* current);
int ResolveVar(ScopeContext* scope, ASTNode* current, TYPE* type, char* typeLex);

int ResolveFuncs(ScopeContext* scope, ASTNode* current);
int ResolveParams(ScopeContext* scope, ASTNode* current);
int ResolveParam(ScopeContext* scope, ASTNode* current);

int ResolveExprs(ScopeContext* scope, ASTNode* current);
int ResolveExpr(ScopeContext* scope, ASTNode* current);

int ResolveStmts(ScopeContext* scope, ASTNode* current);
int ResolveIfStmt(ScopeContext* scope, ASTNode* current);
int ResolveDoWhileStmt(ScopeContext* scope, ASTNode* current);
int ResolveWhileStmt(ScopeContext* scope, ASTNode* current);
int ResolveSwitchStmt(ScopeContext* scope, ASTNode* current);
int ResolveForStmt(ScopeContext* scope, ASTNode* current);
int ResolveReturnStmt(ScopeContext* scope, ASTNode* current);

int ResolveStructDecl(ScopeContext* scope, ASTNode* current);
int ResolveEnums(ScopeContext* scope, ASTNode* current);

int ResolveFuncCall(ScopeContext* scope, ASTNode* current);
int ResolveMemberAccess(ScopeContext* scope, ASTNode* current, Symbol** resolvedSym);
int ResolveArrIndex(ScopeContext* scope, ASTNode* current);
int ResolveArgList(ScopeContext* scope, ASTNode* current);
int ResolveTypedefs(ScopeContext* scope, ASTNode* current);

int EnterScopeContextIfNeeded(ScopeContext** scope, ASTNode* current);

/* ---------- Helpers ---------- */

TYPE* StringToType(const char* name);
static NodeType CTRL_STMTS[] = { IF_NODE, ELIF_NODE, ELSE_NODE, SWITCH_STMT_NODE,
                                 CASE_NODE, DEFAULT_NODE, WHILE_STMT_NODE, 
                                 DO_WHILE_STMT_NODE, FOR_STMT_NODE };
static int CTRL_STMTS_SIZE = sizeof(CTRL_STMTS) / sizeof(CTRL_STMTS[0]);

/* Alpha Renaming? */
/* Name Spaces */
/* Overloading */


#endif 