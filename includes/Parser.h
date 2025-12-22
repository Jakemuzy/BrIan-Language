#ifndef _PARSER_H__
#define _PARSER_H__

#include "Preprocessor.h"
#include "Dictionary.h"

#define ERRP -1
#define NAP   0
#define VALID 1

typedef struct ASTNode
{
    struct ASTNode* children; 
    int count;
    Token t;
} ASTNode;

typedef struct AST
{
    ASTNode* root;
} AST;

AST* ConstructAST(FILE* fptr);



/* ---------- Recursive Descent ---------- */

void Program(FILE* fptr, AST* ast);

int Body(FILE* fptr, AST* ast);
int StmtList(FILE* fptr, AST* ast);
int Stmt(FILE* fptr, AST* ast);

int ExprStmt(FILE* fptr, AST* ast);
int DeclStmt(FILE* fptr, AST* ast);
int CtrlStmt(FILE* fptr, AST* ast);
int ReturnStmt(FILE* fptr, AST* ast);

int IfStmt(FILE* fptr, AST* ast);
int SwitchStmt(FILE* fptr, AST* ast);
int WhileStmt(FILE* fptr, AST* ast);
int DoWhileStmt(FILE* fptr, AST* ast);
int ForStmt(FILE* fptr, AST* ast);

int Expr(FILE* fptr, AST* ast);
int AsgnExpr(FILE* fptr, AST* ast);
int OrlExpr(FILE* fptr, AST* ast);
int AndlExpr(FILE* fptr, AST* ast);
int OrExpr(FILE* fptr, AST* ast);
int XorExpr(FILE* fptr, AST* ast);
int AndExpr(FILE* fptr, AST* ast);
int EqqExpr(FILE* fptr, AST* ast);
int RelationExpr(FILE* fptr, AST* ast);
int AddExpr(FILE* fptr, AST* ast);
int MultExpr(FILE* fptr, AST* ast);
int PowExpr(FILE* fptr, AST* ast);
int Prefix(FILE* fptr, AST* ast);
int Postfix(FILE* fptr, AST* ast);
int Primary(FILE* fptr, AST* ast);

int VarList(FILE* fptr, AST* ast);
int Var(FILE* fptr, AST* ast);

#endif
