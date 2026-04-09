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
      bool failure, panicMode;

      Arena* arena;
      AST* ast;
} ParserContext;

ParserContext* InitalizeParserContext(TokenizerContext* tokenizer);
void DestroyParserContext(ParserContext* ctx);

/* ----- Operator Precedence Order ----- */

typedef enum PRECEDENCE {
      PREC_NONE, PREC_TERNARY, PREC_ASGN,
      PREC_ORL, PREC_ANDL, PREC_OR, PREC_XOR,
      PREC_AND, PREC_EQQ, PREC_COMP, PREC_SHIFT,
      PREC_ADD, PREC_MULT, PREC_POW, PREC_PRE,
      PREC_POST
} PRECEDENCE;

/* ----- Recursive Descent ----- */

void Program(ParserContext* ctx);

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

ASTNode* Expr(ParserContext* ctx, PRECEDENCE prec);
ASTNode* AsgnExpr(ParserContext* ctx, PRECEDENCE prec, ASTNode* left);
ASTNode* UnaryExpr(ParserContext* ctx, PRECEDENCE prec);
ASTNode* BinaryExpr(ParserContext* ctx, PRECEDENCE prec, ASTNode* left);
ASTNode* TernaryExpr(ParserContext* ctx, PRECEDENCE prec, ASTNode* left);

ASTNode* Channel(ParserContext* ctx);
ASTNode* Matrix(ParserContext* ctx);
ASTNode* Vector(ParserContext* ctx);
ASTNode* DeclPrefix(ParserContext* ctx);
ASTNode* GenericList(ParserContext* ctx);
      ASTNode* Generic(ParserContext* ctx);
ASTNode* TypeQualifierList(ParserContext* ctx);
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

/* ----- Pratt Parsing ----- */

typedef struct ParseRule {
      ASTNode* (*prefix)(ParserContext* ctx, PRECEDENCE prec);
      ASTNode* (*infix)(ParserContext* ctx, PRECEDENCE prec, ASTNode* left);
      PRECEDENCE prec;
      
      bool rightAssoc;
} ParseRule;

static ParseRule PRECEDENCE_TABLE[] = {
      { NULL, NULL, PREC_NONE, false },
      [EQ] = { NULL, AsgnExpr, PREC_ASGN, true }, [PEQ] = { NULL, AsgnExpr, PREC_ASGN, true }, 
      [SEQ] = { NULL, AsgnExpr, PREC_ASGN, true }, [MEQ] = { NULL, AsgnExpr, PREC_ASGN, true }, 
      [DEQ] = { NULL, AsgnExpr, PREC_ASGN, true }, [MODEQ] = { NULL, AsgnExpr, PREC_ASGN, true }, 
      [LEFTEQ] = { NULL, AsgnExpr, PREC_ASGN, true }, [RIGHTEQ] = { NULL, AsgnExpr, PREC_ASGN, true }, 
      [ANDEQ] = { NULL, AsgnExpr, PREC_ASGN, true }, [XOREQ] = { NULL, AsgnExpr, PREC_ASGN, true }, 
      [OREQ] = { NULL, AsgnExpr, PREC_ASGN, true }, [ANDLEQ] = { NULL, AsgnExpr, PREC_ASGN, true }, 
      [ORLEQ] = { NULL, AsgnExpr, PREC_ASGN, true }, [SEND] = { UnaryExpr, AsgnExpr, PREC_ASGN, true }, 

      [ORL] = { NULL, BinaryExpr, PREC_ORL, false },
      [ANDL] = { NULL, BinaryExpr, PREC_ANDL, false },
      [OR] = { NULL, BinaryExpr, PREC_OR, false },
      [XOR] = { NULL, BinaryExpr, PREC_XOR, false },
      [AND] = { UnaryExpr, BinaryExpr, PREC_AND, false },

      [EQQ] = { NULL, BinaryExpr, PREC_EQQ, false }, [NEQQ] = { NULL, BinaryExpr, PREC_EQQ, false },

      [LESS] = { NULL, BinaryExpr, PREC_COMP, false }, [GREAT] = { NULL, BinaryExpr, PREC_COMP, false },
      [LEQQ] = { NULL, BinaryExpr, PREC_COMP, false }, [GEQQ] = { NULL, BinaryExpr, PREC_COMP, false },

      [LSHIFT] = { NULL, BinaryExpr, PREC_SHIFT, false }, [RSHIFT] = { NULL, BinaryExpr, PREC_SHIFT, false },

      [PLUS] = { UnaryExpr, BinaryExpr, PREC_ADD, false }, [MINUS] = { UnaryExpr, BinaryExpr, PREC_ADD, false }, 

      [MULT] = { UnaryExpr, BinaryExpr, PREC_MULT, false }, [DIV] = { NULL, BinaryExpr, PREC_MULT, false }, 
      [MOD] = { NULL, BinaryExpr, PREC_MULT, false }, [DOTPROD] = { NULL, BinaryExpr, PREC_MULT, false }, 

      [POW] = { NULL, BinaryExpr, PREC_POW, true }, 

      [INC] = { UnaryExpr, NULL, PREC_PRE, false }, [DEC] = { UnaryExpr, NULL, PREC_PRE, false }, 
      [NEG] = { UnaryExpr, NULL, PREC_PRE, false }, [NOT] = { UnaryExpr, NULL, PREC_PRE, false }, 
      [SPAWN] = { UnaryExpr, NULL, PREC_PRE, false }, [AWAIT] = { UnaryExpr, NULL, PREC_PRE, false }, 
};

#endif
