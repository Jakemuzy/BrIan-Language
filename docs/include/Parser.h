#ifndef _BRIAN_PARSER_H_
#define _BRIAN_PARSER_H_

#include "ErrorHandler.h"
#include "ArenaAllocator.h"

#include "AST.h"
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

typedef struct ParserContext {
      TokenizerContext* tokenizer;
      Token current;
      Token buffer[3];  // I don't like this buffer system 

      Error error;

      Arena* arena;
      AST* ast;
} ParserContext;

ParserContext* InitalizeParserContext(TokenizerContext* tokenizer, size_t fileSize);
void DestroyParserContext(ParserContext* ctx);

/* ----- Recursive Descent ----- */

void Program(ParserContext* ctx);

ASTNode* DeclQualifiers(ParserContext* ctx);

ASTNode* Function(ParserContext* ctx);
ASTNode* FuncDecl(ParserContext* ctx);
ASTNode* FuncDef(ParserContext* ctx);

ASTNode* FuncSignature(ParserContext* ctx);
ASTNode* GenericFunc(ParserContext* ctx);
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
