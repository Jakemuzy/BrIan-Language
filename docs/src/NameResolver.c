#include "NameResolver.h"

static inline void NameresERROR(NameResolverContext* ctx, Symbol* original, ASTNode* current) 
{
}

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
    printf("Name resolving...\n");

    ASTNode* root = ctx->ast->root;
    for (int i = 0; i < root->childCount; i++) {
        ASTNode* current = root->children[i];

        switch (current->ntype) {
            case FUNC_DECL:
                printf("Func\n");
                break;
            case FUNC_DEF:
                ResolveFuncDef(ctx, current);
                break;
            case GEN_FUNC_DECL:
                printf("Gen func decl\n");
                break;
            case GEN_FUNC_DEF:
                printf("Gen func def\n");
                break;
            case VAR_DECL_NODE:
                printf("Decl\n");
                break;
            case INTERFACE_DECL_NODE:
                printf("Interface\n");
                break;
            default:
                printf("Unexpected node type %d\n", current->ntype);
                return;
        }
    }
}

void ResolveFuncDecl(NameResolverContext* ctx, ASTNode* current)
{

}

void ResolveFuncDef(NameResolverContext* ctx, ASTNode* current)
{

    Environment* env = GetNamespace(ctx->nss, N_VAR);
    Symbol* sym = PushEnvironment(ctx->arena, env, current, S_FUNC);

    if (sym == SYM_ALREADY_EXISTS) 
        sym = POISON_SYM;

    ResolveType(ctx, current->children[0]);

}

void ResolveGenFuncDecl(NameResolverContext* ctx, ASTNode* current)
{

}

void ResolveGenFuncDef(NameResolverContext* ctx, ASTNode* current)
{

}


void ResolveType(NameResolverContext* ctx, ASTNode* current)
{
    // Predefined types 
    if (current->ntype == TYPE_NODE) return;

    // Ident means user defined
    char* typeName = current->token.lexeme;
    Environment* typeEnv = GetNamespace(ctx->nss, N_TYPE);
    Symbol* typeSym = LookupEnvironment(typeEnv, typeName);

    if (typeSym == SYM_DOESNT_EXIST) 
        ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
            "User defined type '%s' doesn't exist within current scope on line %d, col %d.\n",
            typeName, current->token.row, current->token.col
        );
}