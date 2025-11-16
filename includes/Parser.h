#include "Tokenizer.h"
#include "Dictionary.h"

#define ERRP -1
#define NAP
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

static Dict TypeMap;

/* ---------- Recursive Descent ---------- */

void Program(FILE* fptr, AST* ast);
void ImportList(FILE* fptr, AST* ast);

void Thread(FILE* fptr, AST* ast);

void Body(FILE* fptr, AST* ast);
void StmtList(FILE* fptr, AST* ast);
void Stmt(FILE* fptr, AST* ast);
void LineStmt(FILE* fptr, AST* ast);

void ExprStmt(FILE* fptr, AST* ast);
void DeclStmt(FILE* fptr, AST* ast);
void CtrlStmt(FILE* fptr, AST* ast);
void ReturnStmt(FILE* fptr, AST* ast);

void IfStmt(FILE* fptr, AST* ast);
void SwitchStmt(FILE* fptr, AST* ast);
void WhileStmt(FILE* fptr, AST* ast);
void DoWhileStmt(FILE* fptr, AST* ast);
void ForStmt(FILE* fptr, AST* ast);

void Expr(FILE* fptr, AST* ast);
void LasgnExpr(FILE* fptr, AST* ast);
void BasgnExpr(FILE* fptr, AST* ast);
void SasgnExpr(FILE* fptr, AST* ast);
void AsgnExpr(FILE* fptr, AST* ast);
void TernExpr(FILE* fptr, AST* ast);
void LorExpr(FILE* fptr, AST* ast);
void LandExpr(FILE* fptr, AST* ast);
void BorExpr(FILE* fptr, AST* ast);
void XorExpr(FILE* fptr, AST* ast);
void BandExpr(FILE* fptr, AST* ast);
void EqqExpr(FILE* fptr, AST* ast);
void CompExpr(FILE* fptr, AST* ast);
void ShiftExpr(FILE* fptr, AST* ast);
void AddExpr(FILE* fptr, AST* ast);
void MultExpr(FILE* fptr, AST* ast);
void PowExpr(FILE* fptr, AST* ast);
void Prefix(FILE* fptr, AST* ast);
void Postfix(FILE* fptr, AST* ast);
void Primary(FILE* fptr, AST* ast);

void VarList(FILE* fptr, AST* ast);
void Var(FILE* fptr, AST* ast);



