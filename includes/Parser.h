#ifndef _PARSER_H__
#define _PARSER_H__

#include "Preprocessor.h"
#include "Dictionary.h"
#include "AST.h"

#define ERRP -1
#define NAP   0
#define VALID 1

/* ---------- Recursive Descent ---------- */

AST* Program(FILE* fptr, AST* ast);
int Function(FILE* fptr, ASTNode* parent);
int ParamList(FILE* fptr, ASTNode* parent);
int Param(FILE* fptr, ASTNode* parent);

int Body(FILE* fptr, ASTNode* parent);
int StmtList(FILE* fptr, ASTNode* parent);
int Stmt(FILE* fptr, ASTNode* parent);

int ExprStmt(FILE* fptr, ASTNode* parent);
int DeclStmt(FILE* fptr, ASTNode* parent);
int CtrlStmt(FILE* fptr, ASTNode* parent);
int ReturnStmt(FILE* fptr, ASTNode* parent);

int IfStmt(FILE* fptr, ASTNode* parent);
int SwitchStmt(FILE* fptr, ASTNode* parent);
int WhileStmt(FILE* fptr, ASTNode* parent);
int DoWhileStmt(FILE* fptr, ASTNode* parent);
int ForStmt(FILE* fptr, ASTNode* parent);

int ExprList(FILE* fptr, ASTNode* parent);
int Expr(FILE* fptr, ASTNode* parent);
int AsgnExpr(FILE* fptr, ASTNode* parent);
int OrlExpr(FILE* fptr, ASTNode* parent);
int AndlExpr(FILE* fptr, ASTNode* parent);
int OrExpr(FILE* fptr, ASTNode* parent);
int XorExpr(FILE* fptr, ASTNode* parent);
int AndExpr(FILE* fptr, ASTNode* parent);
int EqqExpr(FILE* fptr, ASTNode* parent);
int RelationExpr(FILE* fptr, ASTNode* parent);
int AddExpr(FILE* fptr, ASTNode* parent);
int MultExpr(FILE* fptr, ASTNode* parent);
int PowExpr(FILE* fptr, ASTNode* parent);
int Prefix(FILE* fptr, ASTNode* parent);
int Postfix(FILE* fptr, ASTNode* parent);
int Primary(FILE* fptr, ASTNode* parent);

int Type(FILE* fptr, ASTNode* parent);
int ArgList(FILE* fptr, ASTNode* parent);
int VarList(FILE* fptr, ASTNode* parent);
int Var(FILE* fptr, ASTNode* parent);

#endif
