#ifndef _BRIAN_TYPE_CHECKER_H_
#define _BRIAN_TYPE_CHECKER_H_

#include <stdbool.h>

#include "OperatorRules.h"

/*       BrIan Type Checker
    ----------------------------
     Refer to docs/pipeline.md 
           for details.  

      Handles all the implicit 
    semantic correctness and type
    checking regarding operators
         of separate types.

      This includes but is not 
    limited to: implicit casting,
    explicit casting, divide by 0
     T/U MinMax, and other checks
*/

typedef struct TypeCheckerContext {
     AST* ast;
     bool failure;
} TypeCheckerContext;

TypeCheckerContext* InitalizeTypeCheckerContext(AST* ast);

/* ----- Actual Type Checking ----- */

TYPE* TypeCheckProgram(TypeCheckerContext* ctx);

/* Functions */
TYPE* TypeCheckFuncDecl(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckFuncDef(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckGenFuncDecl(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckGenFuncDef(TypeCheckerContext* ctx, ASTNode* current);
/* ... Return Type ... */
TYPE* TypeCheckBody(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckStructBody(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckGenStructBody(TypeCheckerContext* ctx, ASTNode* current);

/* Ctrl Stmts */
TYPE* TypeCheckIfStmt(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckSwitchStmt(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckWhileStmt(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckDoWhileStmt(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckForStmt(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckReturnStmt(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckLockStmt(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckCriticalStmt(TypeCheckerContext* ctx, ASTNode* current);

/* Decls */
TYPE* TypeCheckVarDecl(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckEnumDecl(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckTypedefDecl(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckStructDecl(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckGenStructDecl(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckImplements(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckInterfaceDecl(TypeCheckerContext* ctx, ASTNode* current);

/* Exprs */
TYPE* TypeCheckExpr(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckBinaryExpr(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckAsgnExpr(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckPrefixExpr(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckPostfixExpr(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckTernaryExpr(TypeCheckerContext* ctx, ASTNode* current);

/* Operations */
TYPE* TypeCheckCast(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckIndex(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckFuncCall(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckMember(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckReference(TypeCheckerContext* ctx, ASTNode* current); // Bad name, refers to ->
TYPE* TypeCheckSizeof(TypeCheckerContext* ctx, ASTNode* current);

/* Others */
TYPE* TypeCheckVar(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckArrDecl(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckArrInit(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckCaptures(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckOperatorOverload(TypeCheckerContext* ctx, ASTNode* current);

/* Lists */
TYPE* TypeCheckParamList(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckArgList(TypeCheckerContext* ctx, ASTNode* current);

/* Types */
TYPE* TypeCheckGenericList(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckGeneric(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckGenericRef(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckType(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckMatrix(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckVector(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckChannel(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckLambda(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckClosure(TypeCheckerContext* ctx, ASTNode* current);
TYPE* TypeCheckFuncPointer(TypeCheckerContext* ctx, ASTNode* current);

#endif
