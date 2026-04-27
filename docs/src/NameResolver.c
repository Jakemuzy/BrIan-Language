#include "NameResolver.h"

/* ----- Context ----- */

NameResolverContext* InitalizeNameResolverContext(AST* ast, size_t arenaSize)
{
    NameResolverContext* ctx = malloc(sizeof(NameResolverContext));

    ctx->ast = ast;
    ctx->arena = CreateArena(arenaSize); // Estimate done in compiler.c
    ctx->nss = InitalizeNamespaces(ctx->arena);
    ctx->failure = false;

    return ctx;
}

void DestroyNameResolverContext(NameResolverContext* ctx)
{
    // Dont free ast, owned by parser context (freed with its arena)
    DestroyArena(ctx->arena);   // Arena frees namespace symbols
    free(ctx->nss);
    free(ctx);
}

/* ----- Actual Resolution ----- */

void NameResolve(NameResolverContext* ctx)
{

}