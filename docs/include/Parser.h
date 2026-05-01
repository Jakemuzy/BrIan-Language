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

ASTNode* Lambda(ParserContext* ctx);
ASTNode* Captures(ParserContext* ctx);
ASTNode* Body(ParserContext* ctx);


ASTNode* VarDecl(ParserContext* ctx);
ASTNode* StructDecl(ParserContext* ctx);
ASTNode* GenStructBody(ParserContext* ctx);
      ASTNode* StructBody(ParserContext* ctx);
      ASTNode* OperatorOverload(ParserContext* ctx);
ASTNode* InterfaceDecl(ParserContext* ctx);
      ASTNode* InterfaceBody(ParserContext* ctx);
      ASTNode* Implements(ParserContext* ctx);
ASTNode* EnumDecl(ParserContext* ctx);
ASTNode* TypedefDecl(ParserContext* ctx);

ASTNode* LockStmt(ParserContext* ctx);
ASTNode* CriticalStmt(ParserContext* ctx);

ASTNode* IfStmt(ParserContext* ctx);
ASTNode* SwitchStmt(ParserContext* ctx);
      ASTNode* Case(ParserContext* ctx);
      ASTNode* Default(ParserContext* ctx);
ASTNode* WhileStmt(ParserContext* ctx);
ASTNode* DoWhileStmt(ParserContext* ctx);
ASTNode* ForStmt(ParserContext* ctx);
      ASTNode* ExprList(ParserContext* ctx);
ASTNode* ReturnStmt(ParserContext* ctx);

ASTNode* Expr(ParserContext* ctx, PRECEDENCE prec);
ASTNode* AsgnExpr(ParserContext* ctx, PRECEDENCE prec, ASTNode* left);
ASTNode* PrefixExpr(ParserContext* ctx, PRECEDENCE prec);
ASTNode* PostfixExpr(ParserContext* ctx, PRECEDENCE prec, ASTNode* left);
ASTNode* BinaryExpr(ParserContext* ctx, PRECEDENCE prec, ASTNode* left);
ASTNode* TernaryExpr(ParserContext* ctx, PRECEDENCE prec, ASTNode* left);

ASTNode* Type(ParserContext* ctx);
ASTNode* Channel(ParserContext* ctx);
ASTNode* Matrix(ParserContext* ctx);
ASTNode* Vector(ParserContext* ctx);
ASTNode* FuncPointerType(ParserContext* ctx);
ASTNode* GenericList(ParserContext* ctx);
      ASTNode* Generic(ParserContext* ctx);
ASTNode* TypeQualifierList(ParserContext* ctx);
ASTNode* LinkageSpecifier(ParserContext* ctx);

ASTNode* Sizeof(ParserContext* ctx);
// TODO: Need to provide left to these, so they ident can be in the correct order
ASTNode* Cast(ParserContext* ctx);
ASTNode* Index(ParserContext* ctx);
ASTNode* CallFunc(ParserContext* ctx, ASTNode* left);

ASTNode* ArgList(ParserContext* ctx);

ASTNode* VarList(ParserContext* ctx);
ASTNode* Var(ParserContext* ctx);

ASTNode* ArrInitList(ParserContext* ctx);

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
      [ORLEQ] = { NULL, AsgnExpr, PREC_ASGN, true }, [SEND] = { PrefixExpr, AsgnExpr, PREC_ASGN, true }, 
      [NEGEQ] = { NULL, AsgnExpr, PREC_ASGN, true },

      [ORL] = { NULL, BinaryExpr, PREC_ORL, false },
      [ANDL] = { NULL, BinaryExpr, PREC_ANDL, false },
      [OR] = { NULL, BinaryExpr, PREC_OR, false },
      [XOR] = { NULL, BinaryExpr, PREC_XOR, false },
      [AND] = { PrefixExpr, BinaryExpr, PREC_AND, false },

      [EQQ] = { NULL, BinaryExpr, PREC_EQQ, false }, [NEQQ] = { NULL, BinaryExpr, PREC_EQQ, false },

      [LESS] = { NULL, BinaryExpr, PREC_COMP, false }, [GREAT] = { NULL, BinaryExpr, PREC_COMP, false },
      [LEQQ] = { NULL, BinaryExpr, PREC_COMP, false }, [GEQQ] = { NULL, BinaryExpr, PREC_COMP, false },

      [LSHIFT] = { NULL, BinaryExpr, PREC_SHIFT, false }, [RSHIFT] = { NULL, BinaryExpr, PREC_SHIFT, false },

      [PLUS] = { PrefixExpr, BinaryExpr, PREC_ADD, false }, [MINUS] = { PrefixExpr, BinaryExpr, PREC_ADD, false }, 

      [MULT] = { PrefixExpr, BinaryExpr, PREC_MULT, false }, [DIV] = { NULL, BinaryExpr, PREC_MULT, false }, 
      [MOD] = { NULL, BinaryExpr, PREC_MULT, false }, [DOTPROD] = { NULL, BinaryExpr, PREC_MULT, false }, 

      [POW] = { NULL, BinaryExpr, PREC_POW, true }, 

      [MEM] = { NULL, BinaryExpr, PREC_POST, false }, [SMEM] = { NULL, BinaryExpr, PREC_POST, false }, 
      [AS] = { NULL, BinaryExpr, PREC_POST, false }, [REF] = { NULL, BinaryExpr, PREC_POST, false }, 
      [SREF] = { NULL, BinaryExpr, PREC_POST, false }, [LBRACK] = { NULL, BinaryExpr, PREC_POST, false }, 
      [LPAREN] = { NULL, BinaryExpr, PREC_POST, false },

      [INC] = { PrefixExpr, PostfixExpr, PREC_POST, false }, [DEC] = { PrefixExpr, PostfixExpr, PREC_POST, false }, 
      [NEG] = { PrefixExpr, NULL, PREC_PRE, false }, [NOT] = { PrefixExpr, NULL, PREC_PRE, false }, 
      [SPAWN] = { PrefixExpr, NULL, PREC_PRE, false }, [AWAIT] = { PrefixExpr, NULL, PREC_PRE, false }, 
};

#endif
