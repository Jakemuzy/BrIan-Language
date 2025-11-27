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

/* ---------- Determing Exprtyp --------- */

static const int ASSIGNS = { MEQ, DEQ, MODEQ, PEQ, SEQ, ANDEQ, OREQ, XOREQ, RIGHTEQ, LEFTEQ, LOREQ, LANDEQ, EQ };
static const int LOGICS = { LESS, LEQQ, GREAT, GEQQ, EQQ, NEQQ, ANDL, ORL };
static const int BITS = { AND, XOR, OR, RSHIFT, LSHIFT };
static const int ADDS = { PLUS, MINUS };
static const int MULTS = { MULT, DIV, MOD };
static const int PREFIXS = { POW, INC, DEC, NOT, NEG, /* Cast */ MULT, AND };
static const int POSTFIXS = { INC, DEC, REG };

/* ---------- Recursive Descent ---------- */

void Program(FILE* fptr, AST* ast);
int ImportList(FILE* fptr, AST* ast);

int Thread(FILE* fptr, AST* ast);

int Body(FILE* fptr, AST* ast);
int StmtList(FILE* fptr, AST* ast);
int Stmt(FILE* fptr, AST* ast);
int LineStmt(FILE* fptr, AST* ast);

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
int LogicExpr(FILE* fptr, AST* ast);
int BitExpr(FILE* fptr, AST* ast);
int AddExpr(FILE* fptr, AST* ast);
int MultExpr(FILE* fptr, AST* ast);
int PowExpr(FILE* fptr, AST* ast);
int Prefix(FILE* fptr, AST* ast);
int Postfix(FILE* fptr, AST* ast);
int Primary(FILE* fptr, AST* ast);

int VarList(FILE* fptr, AST* ast);
int Var(FILE* fptr, AST* ast);

