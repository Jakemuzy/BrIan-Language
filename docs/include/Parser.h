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

      /* Add tokenizer lookahead */
      Error error;
      bool failure;

      Arena* arena;
      AST* ast;
} ParserContext;

ParserContext* InitalizeParserContext(TokenizerContext* tokenizer, size_t fileSize);
void DestroyParserContext(ParserContext* ctx);

typedef enum ParseFlag {
    PARSE_VALID,
    PARSE_NOT_APPLICABLE,
    PARSE_ERROR
} ParseFlag;

typedef struct ParseResult {
    ParseFlag flag;
    ASTNode*  node;
} ParseResult;

void SyncRecovery(ParserContext* ctx, TokenType tt);

/* ----- Recursive Descent ----- */

void Program(ParserContext* ctx);

ParseResult Function(ParserContext* ctx);
ParseResult FuncDecl(ParserContext* ctx);
ParseResult FuncDef(ParserContext* ctx);

ParseResult FuncSignature(ParserContext* ctx);
ParseResult GenericFunc(ParserContext* ctx);
ParseResult RegularFunc(ParserContext* ctx);

ParseResult ParamList(ParserContext* ctx);
ParseResult Param(ParserContext* ctx);
ParseResult GenParam(ParserContext* ctx);

ParseResult Lamba(ParserContext* ctx);
ParseResult Body(ParserContext* ctx);
ParseResult StmtList(ParserContext* ctx);
ParseResult Stmt(ParserContext* ctx);

ParseResult ExprStmt(ParserContext* ctx);
ParseResult DeclStmt(ParserContext* ctx);

ParseResult VarDecl(ParserContext* ctx);
ParseResult GenDecl(ParserContext* ctx);
ParseResult StructDecl(ParserContext* ctx);
ParseResult GenericStruct(ParserContext* ctx);
      ParseResult GenStructBody(ParserContext* ctx);
ParseResult RegularStruct(ParserContext* ctx);
      ParseResult StructBody(ParserContext* ctx);
      ParseResult OperatorOverload(ParserContext* ctx);
      // Overloadable op 
ParseResult InterfaceDecl(ParserContext* ctx);
      ParseResult InterfaceBody(ParserContext* ctx);

ParseResult EnumDecl(ParserContext* ctx);
      ParseResult EnumBody(ParserContext* ctx);
ParseResult TypedefDecl(ParserContext* ctx);
      ParseResult TypeSpec(ParserContext* ctx);
      ParseResult TypedefPostfix(ParserContext* ctx);

ParseResult ConcurrencyStmt(ParserContext* ctx);
      ParseResult LockStmt(ParserContext* ctx);
      ParseResult CriticalStmt(ParserContext* ctx);

ParseResult IfStmt(ParserContext* ctx);
ParseResult SwitchStmt(ParserContext* ctx);
      ParseResult Case(ParserContext* ctx);
      ParseResult Default(ParserContext* ctx);
ParseResult WhileStmt(ParserContext* ctx);
ParseResult DoWhlieStmt(ParserContext* ctx);
ParseResult ForStmt(ParserContext* ctx);
ParseResult ExprList(ParserContext* ctx);

ParseResult Expr(ParserContext* ctx);
ParseResult TernaryExpr(ParserContext* ctx);
ParseResult AsgnExpr(ParserContext* ctx);
ParseResult BinaryExpr(ParserContext* ctx);
ParseResult UnaryExpr(ParserContext* ctx); // Prefix / postfix
ParseResult Primary(ParserContext* ctx);

ParseResult Type(ParserContext* ctx);
ParseResult Channel(ParserContext* ctx);
ParseResult Matrix(ParserContext* ctx);
ParseResult Vector(ParserContext* ctx);
ParseResult DeclPrefix(ParserContext* ctx);
ParseResult GenericList(ParserContext* ctx);
      ParseResult Generic(ParserContext* ctx);
ParseResult TypeQualifierList(ParserContext* ctx);
ParseResult TypeQualifier(ParserContext* ctx);
ParseResult LinkageSpecifier(ParserContext* ctx);

// Idk if this here 
ParseResult Sizeof(ParserContext* ctx);

ParseResult Reg(ParserContext* ctx);
ParseResult Hex(ParserContext* ctx);
ParseResult PredefVars(ParserContext* ctx);

ParseResult ArgList(ParserContext* ctx);

ParseResult VarList(ParserContext* ctx);
ParseResult Var(ParserContext* ctx);

ParseResult ArrDecl(ParserContext* ctx);
ParseResult ArrInitList(ParserContext* ctx);
ParseResult Literal(ParserContext* ctx);

#endif
