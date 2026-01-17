#ifndef _PARSER_H__
#define _PARSER_H__

#include "Preprocessor.h"
#include "Dictionary.h"
#include "AST.h"

/* ---------- Global Error Propogation ----------*/

typedef enum ParseError {
    VALID, 
    NAP,    /* Not applicable */
    ERRP,   /* Errored */
    EOFP    /* EOF */
} ParseError;

ParseError PARSE_ERROR = VALID;     /* Global Error Token */
ASTNode* PARSE_FAIL(ParseError code);
ASTNode* ERROR_MESSAGE(char* message, int count, ...);

/* ---------- Helpers ---------- */

int ValidTokType(const int types[], int arrSize, int type);
int CompareToken(FILE* fptr, TokenType desired, char* errMessage, int errType);

/* ---------- Recursive Descent ---------- */
/* Indendented Functions Represent Helpers */

AST* Program(FILE* fptr);
ASTNode* Function(FILE* fptr);
ASTNode* ParamList(FILE* fptr);
ASTNode* Param(FILE* fptr);

ASTNode* Body(FILE* fptr);
ASTNode* StmtList(FILE* fptr);
ASTNode* Stmt(FILE* fptr);

ASTNode* ExprStmt(FILE* fptr);
ASTNode* DeclStmt(FILE* fptr);
ASTNode* CtrlStmt(FILE* fptr);
ASTNode* ReturnStmt(FILE* fptr);

ASTNode* IfStmt(FILE* fptr);
    ASTNode* IfElifElse(FILE* fptr, TokenType type);
ASTNode* SwitchStmt(FILE* fptr);
    ASTNode* Case(FILE* fptr);
    ASTNode* Default(FILE* fptr);
ASTNode* WhileStmt(FILE* fptr);
ASTNode* DoWhileStmt(FILE* fptr);
ASTNode* ForStmt(FILE* fptr);

ASTNode* ExprList(FILE* fptr);
ASTNode* Expr(FILE* fptr);
ASTNode* AsgnExpr(FILE* fptr);
ASTNode* OrlExpr(FILE* fptr);
ASTNode* AndlExpr(FILE* fptr);
ASTNode* OrExpr(FILE* fptr);
ASTNode* XorExpr(FILE* fptr);
ASTNode* AndExpr(FILE* fptr);
ASTNode* EqqExpr(FILE* fptr);
ASTNode* RelationExpr(FILE* fptr);
ASTNode* AddExpr(FILE* fptr);
ASTNode* MultExpr(FILE* fptr);
ASTNode* PowExpr(FILE* fptr);
ASTNode* Prefix(FILE* fptr);
    ASTNode* Cast(FILE* fptr);
ASTNode* Postfix(FILE* fptr);
    ASTNode* Index(FILE* fptr);
    ASTNode* CallFunc(FILE* fptr);
ASTNode* Primary(FILE* fptr);

ASTNode* Type(FILE* fptr);
ASTNode* ArgList(FILE* fptr);
ASTNode* VarList(FILE* fptr);
ASTNode* Var(FILE* fptr);

#endif
