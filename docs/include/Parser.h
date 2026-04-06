#ifndef _BRIAN_PARSER_H_
#define _BRIAN_PARSER_H_

#include "ErrorHandler.h"
#include "ArenaAllocator.h"

#include "Tokenizer.h"

/*          BrIan Parser
    ----------------------------
     Refer to docs/pipeline.md 
           for details.  
   
     Recursive descent parser, 
      ambiguity resolved via
   multi token lookahead during
          AST generation.

*/

/* ----- AST Logic ----- */

typedef enum NodeType {
    PROG_NODE, FUNC_NODE, PARAM_LIST_NODE, PARAM_NODE,
    BODY_NODE, STMT_LIST_NODE, STMT_NODE, 
    
    EXPR_STMT_NODE, CTRL_STMT_NODE, RETURN_STMT_NODE,
    VAR_DECL_NODE, STRUCT_DECL_NODE, ENUM_DECL_NODE, TYPEDEF_DECL_NODE,
    STRUCT_BODY_NODE, ENUM_BODY_NODE, TYPEDEF_POSTFIX_PTR, TYPEDEF_POSTFIX_ARR,

    IF_STMT_NODE, SWITCH_STMT_NODE, WHILE_STMT_NODE, DO_WHILE_STMT_NODE, FOR_STMT_NODE,
    IF_NODE, ELIF_NODE, ELSE_NODE, CASE_NODE, DEFAULT_NODE,

    VAR_EXPR_LIST_NODE, EXPR_LIST_NODE, EXPR_NODE,

    TYPE_NODE, ARG_LIST_NODE, VAR_LIST_NODE, VAR_NODE,
    IDENT_NODE, LITERAL_NODE, EMPTY_NODE, 
    ASGN_EXPR_NODE, BINARY_EXPR_NODE, UNARY_EXPR_NODE, CALL_FUNC_NODE, ARR_DECL_NODE, ARR_INDEX_NODE, ARR_INIT_NODE, STRUCT_INIT_NODE,
    MEMBER_ACCESS_NODE, SAFE_MEMBER_ACCESS_NODE
} NodeType;

typedef struct {
      struct ASTNode** children;
      size_t childCount;

      NodeType type;
      Token tok;
} ASTNode;

typedef struct {
      ASTNode* root;
} AST;

/* ----- Parser Logic ----- */

typedef struct {
      Arena* arena;
      AST* ast;
} ParserContext;

/* ----- Recursive Descent ----- */

void Program(ParserContext* ctx);

ASTNode* Function(ParserContext* ctx);
ASTNode* FuncDecl(ParserContext* ctx);
ASTNode* FuncDef(ParserContext* ctx);

ASTNode* FuncSignature(ParserContext* ctx);
ASTNode* GenericFunc(ParserContext* ctx);
ASTNode* GenericList(ParserContext* ctx);
ASTNode* RegularFunc(ParserContext* ctx);

ASTNode* ParamList(ParserContext* ctx);
ASTNode* Param(ParserContext* ctx);
ASTNode* GenParam(ParserContext* ctx);

ASTNode* Lamba(ParserContext* ctx);
ASTNode* Body(ParserContext* ctx);
ASTNode* StmtList(ParserContext* ctx);
ASTNode* Stmt(ParserContext* ctx);

ASTNode* ExprStmt(ParserContext* ctx);
ASTNode* DeclStmt(ParserContext* ctx);

ASTNode* VarDecl(ParserContext* ctx);
ASTNode* GenDecl(ParserContext* ctx);
ASTNode* StructDecl(ParserContext* ctx);
ASTNode* GenericStruct(ParserContext* ctx);
      ASTNode* GenStructBody(ParserContext* ctx);
ASTNode* RegularStruct(ParserContext* ctx);
      ASTNode* StructBody(ParserContext* ctx);
      ASTNode* OperatorOverload(ParserContext* ctx);
      // Overloadable op 
ASTNode* InterfaceDecl(ParserContext* ctx);
      ASTNode* InterfaceBody(ParserContext* ctx);

ASTNode* EnumDecl(ParserContext* ctx);
      ASTNode* EnumBody(ParserContext* ctx);
ASTNode* TypedefDecl(ParserContext* ctx);
      ASTNode* TypeSpec(ParserContext* ctx);
      ASTNode* TypedefPostfix(ParserContext* ctx);

ASTNode* ConcurrencyStmt(ParserContext* ctx);
      ASTNode* LockStmt(ParserContext* ctx);
      ASTNode* CriticalStmt(ParserContext* ctx);

ASTNode* IfStmt(ParserContext* ctx);
ASTNode* SwitchStmt(ParserContext* ctx);
      ASTNode* Case(ParserContext* ctx);
      ASTNode* Default(ParserContext* ctx);
ASTNode* WhileStmt(ParserContext* ctx);
ASTNode* DoWhlieStmt(ParserContext* ctx);
ASTNode* ForStmt(ParserContext* ctx);
ASTNode* ExprList(ParserContext* ctx);

ASTNode* Expr(ParserContext* ctx);
ASTNode* TernaryExpr(ParserContext* ctx);
ASTNode* AsgnExpr(ParserContext* ctx);
ASTNode* BinaryExpr(ParserContext* ctx);
ASTNode* UnaryExpr(ParserContext* ctx); // Prefix / postfix
ASTNode* Primary(ParserContext* ctx);

ASTNode* Type(ParserContext* ctx);
ASTNode* Channel(ParserContext* ctx);
ASTNode* Matrix(ParserContext* ctx);
ASTNode* Vector(ParserContext* ctx);
ASTNode* DeclPrefix(ParserContext* ctx);
ASTNode* GenericList(ParserContext* ctx);
      ASTNode* Generic(ParserContext* ctx);
ASTNode* TypeQualifier(ParserContext* ctx);
ASTNode* LinkageSpecifier(ParserContext* ctx);

// Idk if this here 
ASTNode* Sizeof(ParserContext* ctx);

ASTNode* Reg(ParserContext* ctx);
ASTNode* Hex(ParserContext* ctx);
ASTNode* PredefVars(ParserContext* ctx);

ASTNode* ArgList(ParserContext* ctx);

ASTNode* VarList(ParserContext* ctx);
ASTNode* Var(ParserContext* ctx);

ASTNode* ArrDecl(ParserContext* ctx);
ASTNode* ArrInitList(ParserContext* ctx);
ASTNode* Literal(ParserContext* ctx);

#endif