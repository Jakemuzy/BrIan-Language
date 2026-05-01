#include "NameResolver.h"

#define DEBUG_MODE true

static inline void Debug(char* msg) {
    if (DEBUG_MODE) printf("%s\n", msg);
}

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
    Debug("\t START");
    ASTNode* root = ctx->ast->root;
    for (size_t i = 0; i < root->childCount; i++) {
        ASTNode* current = root->children[i];

        switch (current->ntype) {
            case FUNC_DECL:
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
    Debug("FuncDecl");
}

void ResolveFuncDef(NameResolverContext* ctx, ASTNode* current)
{
    Debug("FuncDef");
    Environment* env = GetNamespace(ctx->nss, N_VAR);
    PushEnvironment(ctx->arena, env, current, S_FUNC);

    ResolveType(ctx, current->children[0]);

    EnterScope(ctx->arena, ctx->nss);
    ResolveParamList(ctx, current->children[1]);
    ResolveBody(ctx, current->children[2]);
    ExitScope(ctx->nss);
}

void ResolveGenFuncDecl(NameResolverContext* ctx, ASTNode* current)
{
    Debug("GenFuncDecl");
}

void ResolveGenFuncDef(NameResolverContext* ctx, ASTNode* current)
{
    Debug("GenFuncDef");
}

void ResolveBody(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Body");
    for (size_t i = 0; i < current->childCount; i++) {
        ASTNode* stmt = current->children[i];

        switch (stmt->ntype) {
            case IF_STMT_NODE:
                ResolveIfStmt(ctx, stmt);
                break;
            case SWITCH_STMT_NODE:
                ResolveSwitchStmt(ctx, stmt);
                break;
            case WHILE_STMT_NODE:
                ResolveWhileStmt(ctx, stmt);
                break;
            case DO_WHILE_STMT_NODE:
                ResolveDoWhileStmt(ctx, stmt);
                break;
            case FOR_STMT_NODE:
                ResolveForStmt(ctx, stmt);
                break;
            case RETURN_STMT_NODE:
                ResolveReturnStmt(ctx, stmt);
                break;
            case VAR_DECL_NODE:
                ResolveVarDecl(ctx, stmt);
                break;
            case ENUM_DECL_NODE:
                ResolveEnumDecl(ctx, stmt);
                break;
            case TYPEDEF_DECL_NODE:
                ResolveTypedefDecl(ctx, stmt);
                break;
            case STRUCT_DECL_NODE:
                ResolveStructDecl(ctx, stmt);
                break;
            case BINARY_EXPR_NODE:
                ResolveBinaryExpr(ctx, stmt);
                break;
            case ASGN_EXPR_NODE:
                ResolveAsgnExpr(ctx, stmt);
                break;
            case PREFIX_EXPR_NODE:
                ResolvePrefixExpr(ctx, stmt);
                break;
            case POSTFIX_EXPR_NODE:
                ResolvePostfixExpr(ctx, stmt);
                break;
            case TERNARY_EXPR_NODE:
                ResolveTernaryExpr(ctx, stmt);
                break;
            case CAST_NODE:
                ResolveCast(ctx, stmt);
                break;
            case INDEX_NODE:
                ResolveIndex(ctx, stmt);
                break;
            case CALL_FUNC_NODE:
                ResolveFuncCall(ctx, stmt);
                break;
            case MEMBER_NODE:   // Fallthrough, since same format
            case SMEMBER_NODE:  
                ResolveMember(ctx, stmt);
                break;
            case REF_NODE:      // Falthrough, since same format
            case SREF_NODE:
                ResolveReference(ctx, stmt);
                break;
            default: ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, "Invalid statement type within body %s.\n", stmt->token.lexeme);
        }
    }
}


/* Ctrl Stmts */
void ResolveIfStmt(NameResolverContext* ctx, ASTNode* current)
{
    Debug("IfStmt");
    for (size_t i = 0; i < current->childCount; i++) {
        ASTNode* ifElifElse = current->children[i];
        switch (ifElifElse->ntype) {
            case IF_NODE:   // Fallthrough, same structure
            case ELIF_NODE:
                ResolveExpr(ctx, ifElifElse->children[0]);
                ResolveBody(ctx, ifElifElse->children[1]);
                break;
            case ELSE_NODE:
                ResolveBody(ctx, ifElifElse->children[0]);
                break;    
            default: break;
        }
    }
}

void ResolveSwitchStmt(NameResolverContext* ctx, ASTNode* current)
{
    Debug("SwitchStmt");
    char* identName = current->children[0]->token.lexeme;
    Environment* env = GetNamespace(ctx->nss, N_VAR);
    Symbol* sym = LookupEnvironment(env, identName);

    if (sym == SYM_DOESNT_EXIST) 
        ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
            "Switch statement variable '%s' doesn't exist within current scope on line %d, col %d.\n",
            identName, current->token.row, current->token.col
        );

    for (size_t i = 1; i < current->childCount; i++) {
        ASTNode* caseNode = current->children[i]; 

        // Default also has body on children[0], fall through
        if (caseNode->ntype == CASE_STMT_NODE) 
            ResolveExpr(ctx, caseNode);
        ResolveBody(ctx, caseNode->children[0]); 
    }
}

void ResolveWhileStmt(NameResolverContext* ctx, ASTNode* current)
{
    Debug("WhileStmt");
    ResolveExpr(ctx, current->children[0]);
    ResolveBody(ctx, current->children[1]);
}

void ResolveDoWhileStmt(NameResolverContext* ctx, ASTNode* current)
{
    Debug("DoWhileStmt");
    ResolveExpr(ctx, current->children[0]);
    ResolveBody(ctx, current->children[1]);
}

void ResolveForStmt(NameResolverContext* ctx, ASTNode* current)
{
    Debug("ForStmt");
    ASTNode* initNode = current->children[0];
    if (initNode->ntype == VAR_DECL_NODE) ResolveVarDecl(ctx, initNode);
    else ResolveExpr(ctx, initNode);

    ResolveExpr(ctx, current->children[1]);

    ASTNode* exprListNode = current->children[2];
    for (size_t i = 0; i < exprListNode->childCount; i++) 
        ResolveExpr(ctx, exprListNode->children[i]);

    ResolveBody(ctx, current->children[3]);
}

void ResolveReturnStmt(NameResolverContext* ctx, ASTNode* current)
{

}

/* Decls */
void ResolveVarDecl(NameResolverContext* ctx, ASTNode* current)
{
    Debug("VarDecl");
    ResolveType(ctx, current->children[0]);

    ASTNode* varListNode = current->children[1];
    for (size_t i = 0; i < varListNode->childCount; i++) 
        ResolveVar(ctx, varListNode->children[i]);
}

void ResolveEnumDecl(NameResolverContext* ctx, ASTNode* current)
{

}

void ResolveTypedefDecl(NameResolverContext* ctx, ASTNode* current)
{

}

void ResolveStructDecl(NameResolverContext* ctx, ASTNode* current)
{

}

/* Exprs */
void ResolveExpr(NameResolverContext* ctx, ASTNode* current)
{
    switch (current->ntype) {
        case BINARY_EXPR_NODE:
            ResolveBinaryExpr(ctx, current);
            break;
        case ASGN_EXPR_NODE:
            ResolveAsgnExpr(ctx, current);
            break;
        case PREFIX_EXPR_NODE:
            ResolvePrefixExpr(ctx, current);
            break;
        case POSTFIX_EXPR_NODE:
            ResolvePostfixExpr(ctx, current);
            break;
        case TERNARY_EXPR_NODE:
            ResolveTernaryExpr(ctx, current);
            break;
        case IDENT_NODE:
            Debug("Ident");
            char* identName = current->token.lexeme;
            Environment* env = GetNamespace(ctx->nss, N_VAR);
            Symbol* sym = LookupEnvironment(env, identName);

            if (sym == SYM_DOESNT_EXIST) 
                ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
                    "Variable '%s' doesn't exist within current scope on line %d, col %d.\n",
                    identName, current->token.row, current->token.col
                );
            break;
        case LITERAL_NODE:
            Debug("Literal");
            // Do Nothing
            break;
        case SIZEOF_NODE:
            // Check if type exists        
            break;
        case LAMBDA_NODE:
            ResolveLambda(ctx, current);
            break;
    } 
}

void ResolveBinaryExpr(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Binary");
    ASTNode* first = current->children[0];
    ResolveExpr(ctx, first); 
    ASTNode* second = current->children[1];
    ResolveExpr(ctx, second); 
}

void ResolveAsgnExpr(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Asgn");
    ASTNode* first = current->children[0];
    ResolveExpr(ctx, first); 
    ASTNode* second = current->children[1];
    ResolveExpr(ctx, second); 
}

void ResolvePrefixExpr(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Prefix");
    ASTNode* node = current->children[0];
    ResolveExpr(ctx, node); // Solves Nested
}

void ResolvePostfixExpr(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Postfix");
    ASTNode* node = current->children[0];
    ResolveExpr(ctx, node);
}

void ResolveTernaryExpr(NameResolverContext* ctx, ASTNode* current)
{

}

/* Operations */

void ResolveCast(NameResolverContext* ctx, ASTNode* current)
{

}

void ResolveIndex(NameResolverContext* ctx, ASTNode* current)
{

}

void ResolveFuncCall(NameResolverContext* ctx, ASTNode* current)
{
    char* funcName = current->token.lexeme;
    Environment* env = GetNamespace(ctx->nss, N_VAR);
    Symbol* funcSym = LookupEnvironment(env, funcName);

    if (funcSym == SYM_DOESNT_EXIST) 
        ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
            "User defined function '%s' doesn't exist within current scope on line %d, col %d.\n",
            funcName, current->token.row, current->token.col
        );
    ResolveArgList(ctx, current->children[0]);
}

void ResolveMember(NameResolverContext* ctx, ASTNode* current)
{

}

void ResolveReference(NameResolverContext* ctx, ASTNode* current)
{

}

/* Others */

void ResolveVar(NameResolverContext* ctx, ASTNode* current)
{
    Environment* env = GetNamespace(ctx->nss, N_VAR);
    PushEnvironment(ctx->arena, env, current, S_VAR);

    // Check for k ArrDecls
    size_t i = 0;
    ASTNode* arrDecl = current->children[0];
    while (arrDecl->ntype == ARR_DECL_NODE) {
        ResolveArrDecl(ctx, arrDecl);
        i++;

        arrDecl = current->children[i];
    }

    // Check between Expr or ArrInitList
    if (current->children[i]->ntype == ARR_INIT_LIST_NODE)  // Could resuse arrDecl, but confusing
        ResolveArrInit(ctx, current->children[i]);
    else 
        ResolveExpr(ctx, current->children[i]);
}

void ResolveArrDecl(NameResolverContext* ctx, ASTNode* current)
{
    Debug("ArrDecl");

    // Ignores literals
    if (current->ntype != LITERAL_NODE) ResolveExpr(ctx, current);
}

void ResolveArrInit(NameResolverContext* ctx, ASTNode* current)
{
    Debug("ArrInit");
    for (size_t i = 0; i < current->childCount; i++) {
        ASTNode* listMember = current->children[i];

        // Handles nested arrays 
        if (listMember->ntype == ARR_INIT_LIST_NODE) {
            ResolveArrInit(ctx, listMember);
            continue;
        }
        else if (listMember->ntype != LITERAL_NODE) {
            ResolveExpr(ctx, listMember);
            continue;
        }

        // Ignores literals 
    }
}

void ResolveLambda(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Lambda");

    // Must resolve captures first since it checks the current scope
    ResolveType(ctx, current->children[0]);

    EnterScope(ctx->arena, ctx->nss);
    ResolveParamList(ctx, current->children[1]);
    ResolveCaptures(ctx, current->children[2]);
    ResolveBody(ctx, current->children[3]);
    ExitScope(ctx->nss);
}

void ResolveCaptures(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Captures");
    // Captures from parent scope so its a bit different
    Environment* env = GetNamespace(ctx->nss, N_VAR);
    for (size_t i = 0; i < current->childCount; i++) {
        ASTNode* capture = current->children[i];
        char* captureName = capture->token.lexeme;
        Symbol* captureSym = LookupEnvironment(env->prev, captureName);

        if (captureSym == SYM_DOESNT_EXIST) {
            ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
                "Captured variable '%s' doesn't exist within current scope on line %d, col %d.\n",
                captureName, current->token.row, current->token.col
            );
            capture->sym = POISON_SYM;
            continue;
        }
            
        // Push them to current
        PushEnvironment(ctx->arena, env, captureSym->node, S_VAR);
    }
}

/* Lists */

void ResolveParamList(NameResolverContext* ctx, ASTNode* current)
{
    Debug("ParamList");
    Environment* env = GetNamespace(ctx->nss, N_VAR);

    for (size_t i = 0; i < current->childCount; i++) {
        ASTNode* param = current->children[i];
        PushEnvironment(ctx->arena, env, param, S_FIELD);
    }
}

void ResolveArgList(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Arglist");
    for (size_t i = 0; i < current->childCount; i++) 
        ResolveExpr(ctx, current->children[i]);
}

/* Types */

void ResolveType(NameResolverContext* ctx, ASTNode* current)
{
    switch (current->ntype) {
        case TYPE_NODE: return; // Predefined types
        case CLOSURE_NODE: ResolveClosure(ctx, current); return;
        default: break;
    }

    // Ident -> user defined
    char* typeName = current->token.lexeme;
    Environment* typeEnv = GetNamespace(ctx->nss, N_TYPE);
    Symbol* typeSym = LookupEnvironment(typeEnv, typeName);

    if (typeSym == SYM_DOESNT_EXIST) 
        ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
            "User defined type '%s' doesn't exist within current scope on line %d, col %d.\n",
            typeName, current->token.row, current->token.col
        );
}

void ResolveClosure(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Closure");
    for (size_t i = 0; i < current->childCount; i++) 
        ResolveType(ctx, current->children[i]);
}
