#include "Tokenizer.h"
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

static Dict TypeMap;

/* ---------- Determing Exprtyp --------- */

static const int ASSIGNS[] = { MEQ, DEQ, MODEQ, PEQ, SEQ, ANDEQ, OREQ, NEGEQ, XOREQ, RIGHTEQ, LEFTEQ, ORLEQ, ANDLEQ, EQ };
static const size_t ASSIGNS_COUNT = sizeof(ASSIGNS) / sizeof(ASSIGNS[0]);

static const int LOGICS[] = { LESS, LEQQ, GREAT, GEQQ, EQQ, NEQQ, ANDL, ORL };
static const size_t LOGICS_COUNT = sizeof(LOGICS) / sizeof(LOGICS[0]);


static const int BITS[] = { AND, XOR, OR, RSHIFT, LSHIFT };
static const size_t BITS_COUNT = sizeof(BITS) / sizeof(BITS[0]);

static const int ADDS[] = { PLUS, MINUS };
static const size_t ADDS_COUNT = sizeof(ADDS) / sizeof(ADDS[0]);

static const int MULTS[] = { MULT, DIV, MOD };
static const size_t MULTS_COUNT = sizeof(MULTS) / sizeof(MULTS[0]);

static const int PREFIXS[] = { POW, PP, SS, NOT, NEG, /* Cast */ MULT, AND };
static const size_t PREFIXS_COUNT = sizeof(PREFIXS) / sizeof(PREFIXS[0]);

static const int POSTFIXS[] = { PP, SS, REG };
static const size_t POSTFIXS_COUNT = sizeof(POSTFIXS) / sizeof(POSTFIXS[0]);

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

