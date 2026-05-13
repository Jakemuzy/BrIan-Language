#ifndef _BRIAN_NAME_RESOLVER_H_
#define _BRIAN_NAME_RESOLVER_H_

#include <stdbool.h>

#include "ArenaAllocator.h"
#include "Environment.h"
#include "Namespace.h"

/*      BrIan Name Resolution
    ----------------------------
     Refer to docs/pipeline.md 
           for details.  
           
*/

/* ----- Context ----- */

typedef struct NameResolverContext {
    AST* ast;
    Arena* arena;

    Namespaces* nss;

    // Failure will be recovered by assuming a reasonable in the mean time
    bool failure;   
} NameResolverContext;

NameResolverContext* InitalizeNameResolverContext(AST* ast, size_t arenaSize);
void DestroyNameResolverContext(NameResolverContext* ctx);

/* ----- Actual Resolution ----- */

void NameResolve(NameResolverContext* ctx);

/* Functions */
void ResolveFuncDecl(NameResolverContext* ctx, ASTNode* current);
void ResolveFuncDef(NameResolverContext* ctx, ASTNode* current);
void ResolveGenFuncDecl(NameResolverContext* ctx, ASTNode* current);
void ResolveGenFuncDef(NameResolverContext* ctx, ASTNode* current);
void ResolveReturnType(NameResolverContext* ctx, ASTNode* current);
void ResolveBody(NameResolverContext* ctx, ASTNode* current);
void ResolveStructBody(NameResolverContext* ctx, ASTNode* current);
void ResolveGenStructBody(NameResolverContext* ctx, ASTNode* current);

/* Ctrl Stmts */
void ResolveIfStmt(NameResolverContext* ctx, ASTNode* current);
void ResolveSwitchStmt(NameResolverContext* ctx, ASTNode* current);
void ResolveWhileStmt(NameResolverContext* ctx, ASTNode* current);
void ResolveDoWhileStmt(NameResolverContext* ctx, ASTNode* current);
void ResolveForStmt(NameResolverContext* ctx, ASTNode* current);
void ResolveReturnStmt(NameResolverContext* ctx, ASTNode* current);
void ResolveLockStmt(NameResolverContext* ctx, ASTNode* current);
void ResolveCriticalStmt(NameResolverContext* ctx, ASTNode* current);

/* Decls */
void ResolveVarDecl(NameResolverContext* ctx, ASTNode* current);
void ResolveEnumDecl(NameResolverContext* ctx, ASTNode* current);
void ResolveTypedefDecl(NameResolverContext* ctx, ASTNode* current);
void ResolveStructDecl(NameResolverContext* ctx, ASTNode* current);
void ResolveGenStructDecl(NameResolverContext* ctx, ASTNode* current);
void ResolveImplements(NameResolverContext* ctx, ASTNode* current);
void ResolveInterfaceDecl(NameResolverContext* ctx, ASTNode* current);

/* Exprs */
void ResolveExpr(NameResolverContext* ctx, ASTNode* current);
void ResolveBinaryExpr(NameResolverContext* ctx, ASTNode* current);
void ResolveAsgnExpr(NameResolverContext* ctx, ASTNode* current);
void ResolvePrefixExpr(NameResolverContext* ctx, ASTNode* current);
void ResolvePostfixExpr(NameResolverContext* ctx, ASTNode* current);
void ResolveTernaryExpr(NameResolverContext* ctx, ASTNode* current);

/* Operations */
void ResolveCast(NameResolverContext* ctx, ASTNode* current);
void ResolveIndex(NameResolverContext* ctx, ASTNode* current);
void ResolveFuncCall(NameResolverContext* ctx, ASTNode* current);
void ResolveMember(NameResolverContext* ctx, ASTNode* current);
void ResolveReference(NameResolverContext* ctx, ASTNode* current); // Bad name, refers to ->
void ResolveSizeof(NameResolverContext* ctx, ASTNode* current);

/* Others */
void ResolveVar(NameResolverContext* ctx, ASTNode* current);
void ResolveArrDecl(NameResolverContext* ctx, ASTNode* current);
void ResolveArrInit(NameResolverContext* ctx, ASTNode* current);
void ResolveCaptures(NameResolverContext* ctx, ASTNode* current);
void ResolveOperatorOverload(NameResolverContext* ctx, ASTNode* current);

/* Lists */
void ResolveParamList(NameResolverContext* ctx, ASTNode* current);
void ResolveArgList(NameResolverContext* ctx, ASTNode* current);

/* Types */
void ResolveGenericList(NameResolverContext* ctx, ASTNode* current);
void ResolveGeneric(NameResolverContext* ctx, ASTNode* current);
void ResolveGenericRef(NameResolverContext* ctx, ASTNode* current);
void ResolveType(NameResolverContext* ctx, ASTNode* current);
void ResolveChannel(NameResolverContext* ctx, ASTNode* current);
void ResolveMatrix(NameResolverContext* ctx, ASTNode* current);
void ResolveVector(NameResolverContext* ctx, ASTNode* current);
void ResolveLambda(NameResolverContext* ctx, ASTNode* current);
void ResolveClosure(NameResolverContext* ctx, ASTNode* current);
void ResolveFuncPointer(NameResolverContext* ctx, ASTNode* current);

#endif 
