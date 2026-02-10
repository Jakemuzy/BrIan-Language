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
*/

/* ---------- Error Handling ---------- */

#define ERRN false
#define VALDN true

bool NERROR_NO_IDENT(ASTNode* curr);
bool NERROR_ALREADY_DEFINED(char* name, ASTNode* curr, ASTNode* first);
bool NERROR_DOESNT_EXIST(char* name, ASTNode* curr);

/* ---------- Helpers ---------- */

static NodeType CTRL_STMTS[] = { IF_NODE, ELIF_NODE, ELSE_NODE, SWITCH_STMT_NODE,
                                 CASE_NODE, DEFAULT_NODE, WHILE_STMT_NODE, 
                                 DO_WHILE_STMT_NODE, FOR_STMT_NODE };
static int CTRL_STMTS_SIZE = sizeof(CTRL_STMTS) / sizeof(CTRL_STMTS[0]);

ASTNode* FindIdentChild(ASTNode* node);
bool IdentIsDecl(ASTNode* decl, ASTNode* parent);
bool IsCtrlStmt(NodeType type);
NodeType GetScopeType(ASTNode* node) ;

/* ---------- Resolving ---------- */

SymbolTable* ResolveNames(AST* ast);
bool ResolveNamesInNode(SymbolTable* venv, ASTNode* node, ASTNode* parent);

/* Alpha Renaming? */
/* Name Spaces */
/* Overloading */


#endif 