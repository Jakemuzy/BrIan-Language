#ifndef _PARSER_H__
#define _PARSER_H__

#include "Preprocessor.h"
#include "Dictionary.h"
#include "AST.h"

/* ---------- Error Propogation ----------*/

static bool DEBUG = true;
static bool GLOBAL_ERRP = false;

typedef enum ParseStatus {
    VALID, 
    NAP,    /* Not applicable */
    ERRP,   /* Errored */
    EOFP    /* EOF */
} ParseStatus;

typedef struct ParseResult {
    ParseStatus status;
    ASTNode* node;
} ParseResult;


#define ERROR_MESSAGE(message, linenum) printf("SYNTAX ERROR: %s, on line %d\n", message, linenum);
#define DEBUG_MESSAGE(message) do { \
        if (DEBUG == true)  { printf("%s", message); } \
    } while(0);
ParseResult PARSE_VALID(ASTNode* node, NodeType type);
ParseResult PARSE_NAP();
ParseResult PARSE_ERRP(char* message, Token tok);

/* ---------- Helpers ---------- */

int ValidTokType(const int types[], int arrSize, int type);
int FuncNodePossible(FILE* fptr);

ParseResult IdentNode(Token tok);
ParseResult EmptyNode();
ParseResult ProgNode();
ParseResult ArbitraryNode(Token tok, NodeType type);

/* ---------- Recursive Descent ---------- */
/* Indendented Functions Represent Helpers */

AST* Program(FILE* fptr);
ParseResult Function(FILE* fptr);
ParseResult ParamList(FILE* fptr);
ParseResult Param(FILE* fptr);

ParseResult Body(FILE* fptr);
ParseResult StmtList(FILE* fptr);
ParseResult Stmt(FILE* fptr);

ParseResult ExprStmt(FILE* fptr);
ParseResult DeclStmt(FILE* fptr);
ParseResult CtrlStmt(FILE* fptr);
ParseResult ReturnStmt(FILE* fptr);

ParseResult IfStmt(FILE* fptr);
    ParseResult IfElifElse(FILE* fptr, TokenType type);
ParseResult SwitchStmt(FILE* fptr);
    ParseResult Case(FILE* fptr);
    ParseResult Default(FILE* fptr);
ParseResult WhileStmt(FILE* fptr);
ParseResult DoWhileStmt(FILE* fptr);
ParseResult ForStmt(FILE* fptr);
    ParseResult OptionalExpr(FILE* fptr);

ParseResult ExprList(FILE* fptr);
ParseResult Expr(FILE* fptr);
ParseResult AsgnExpr(FILE* fptr);
ParseResult OrlExpr(FILE* fptr);
ParseResult AndlExpr(FILE* fptr);
ParseResult OrExpr(FILE* fptr);
ParseResult XorExpr(FILE* fptr);
ParseResult AndExpr(FILE* fptr);
ParseResult EqqExpr(FILE* fptr);
ParseResult RelationExpr(FILE* fptr);
ParseResult ShiftExpr(FILE* fptr);
ParseResult AddExpr(FILE* fptr);
ParseResult MultExpr(FILE* fptr);
ParseResult PowExpr(FILE* fptr);
ParseResult Prefix(FILE* fptr);
    ParseResult Cast(FILE* fptr);
ParseResult Postfix(FILE* fptr);
    ParseResult Index(FILE* fptr, ASTNode* callee);
    ParseResult CallFunc(FILE* fptr, ASTNode* callee);
ParseResult Primary(FILE* fptr);

ParseResult Type(FILE* fptr);
ParseResult ArgList(FILE* fptr);
ParseResult VarList(FILE* fptr);
ParseResult Var(FILE* fptr);
ParseResult ArrInitList(FILE* fptr);

#endif
