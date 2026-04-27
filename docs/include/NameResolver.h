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

void ResolveFuncDecl(NameResolverContext* ctx, ASTNode* current);
void ResolveFuncDef(NameResolverContext* ctx, ASTNode* current);
void ResolveGenFuncDecl(NameResolverContext* ctx, ASTNode* current);
void ResolveGenFuncDef(NameResolverContext* ctx, ASTNode* current);

void ResolveType(NameResolverContext* ctx, ASTNode* current);

#endif 
