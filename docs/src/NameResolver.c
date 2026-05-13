#include "NameResolver.h"

/*
    Please Note: 
        PushEnvironment enforces duplicate symbols, 
        and handles the printing of an error in that case.
    
        I should also make an error helper 
        I should also make an already defined var helper
*/

/* ----- Helpers ----- */

#define DEBUG_MODE true

static inline void Debug(char* msg) {
    if (DEBUG_MODE) printf("%s\n", msg);
}

/* 
TODO: 
static inline void NameresERROR(char* format, ASTNode* current) {
    int buf[1028];
    snprintf(buf, sizeof(buf), "%s, on line %d, col %d.", format);
    ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, buf);

    ctx->failure = true;
}
*/

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
                ResolveFuncDecl(ctx, current);
                break;
            case FUNC_DEF:
                ResolveFuncDef(ctx, current);
                break;
            case GEN_FUNC_DECL:
                ResolveGenFuncDecl(ctx, current);
                break;
            case GEN_FUNC_DEF:
                ResolveGenFuncDef(ctx, current);
                break;
            case VAR_DECL_NODE:
                ResolveVarDecl(ctx, current);
                break;
            case ENUM_DECL_NODE:
                ResolveEnumDecl(ctx, current);
                break;
            case TYPEDEF_DECL_NODE:
                ResolveTypedefDecl(ctx, current);
                break;
            case INTERFACE_DECL_NODE:
                ResolveInterfaceDecl(ctx, current);
                break;
            case STRUCT_DECL_NODE:  
                ResolveStructDecl(ctx, current);
                break;
            case GEN_STRUCT_DECL_NODE:
                ResolveGenStructDecl(ctx, current);
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
    Environment* env = GetNamespace(ctx->nss, N_VAR);
    PushEnvironment(ctx->arena, env, current, S_FUNC);

    ResolveReturnType(ctx, current->children[0]);

    EnterScope(ctx->arena, ctx->nss);
    ResolveParamList(ctx, current->children[1]);
    ExitScope(ctx->nss);
}

void ResolveFuncDef(NameResolverContext* ctx, ASTNode* current)
{
    Debug("FuncDef");
    Environment* env = GetNamespace(ctx->nss, N_VAR);
    PushEnvironment(ctx->arena, env, current, S_FUNC);

    ResolveReturnType(ctx, current->children[0]);

    EnterScope(ctx->arena, ctx->nss);
    ResolveParamList(ctx, current->children[1]);
    ResolveBody(ctx, current->children[2]);
    ExitScope(ctx->nss);
}

void ResolveGenFuncDecl(NameResolverContext* ctx, ASTNode* current)
{
    Debug("GenFuncDecl");
    Environment* env = GetNamespace(ctx->nss, N_VAR);
    PushEnvironment(ctx->arena, env, current, S_FUNC);

    // Enter scope earlier, since should resolve from gen param list
    EnterScope(ctx->arena, ctx->nss);

    size_t i = 1;
    // Optional generic list 
    if (current->children[i]->ntype == GENERIC_LIST_NODE) 
        ResolveGenericList(ctx, current->children[i++]);

    ResolveReturnType(ctx, current->children[0]);
    ResolveParamList(ctx, current->children[i++]);
    ExitScope(ctx->nss);
}

void ResolveGenFuncDef(NameResolverContext* ctx, ASTNode* current)
{
    Debug("GenFuncDef");
    Environment* env = GetNamespace(ctx->nss, N_VAR);
    PushEnvironment(ctx->arena, env, current, S_FUNC);

    // Enter scope earlier, since should resolve from gen param list
    EnterScope(ctx->arena, ctx->nss);

    size_t i = 1;
    // Optional generic list 
    if (current->children[i]->ntype == GENERIC_LIST_NODE) 
        ResolveGenericList(ctx, current->children[i++]);

    ResolveReturnType(ctx, current->children[0]);
    ResolveParamList(ctx, current->children[i++]);
    ResolveBody(ctx, current->children[i++]);
    ExitScope(ctx->nss);
}

void ResolveReturnType(NameResolverContext* ctx, ASTNode* current)
{
    Debug("ReturnType");
    ASTNode* returnTypeNode = current->children[0];
    if (returnTypeNode->ntype == GENERIC_NODE)   ResolveGenericRef(ctx, returnTypeNode);
    else if (returnTypeNode->ntype == TYPE_NODE) ResolveType(ctx, returnTypeNode);
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
            case LOCK_STMT_NODE:
                ResolveLockStmt(ctx, stmt);
                break;
            case CRITICAL_STMT_NODE:
                ResolveCriticalStmt(ctx, stmt);
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
            case GEN_STRUCT_DECL_NODE:
                ResolveGenStructDecl(ctx, stmt);
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
            default: 
                ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
                    "Invalid statement type within body '%s' on line %d, col %d.\n", 
                    stmt->token.lexeme, current->token.row, current->token.col
                );            
        }
    }
}

void ResolveStructBody(NameResolverContext* ctx, ASTNode* current)
{
    Debug("StructBody");
    for (size_t i = 0; i < current->childCount; i++) {
        ASTNode* bodyElement = current->children[i];
        if (bodyElement->ntype == VAR_DECL_NODE) ResolveVarDecl(ctx, bodyElement);
        else if (bodyElement->ntype == ENUM_DECL_NODE) ResolveEnumDecl(ctx, bodyElement);
        else if (bodyElement->ntype == FUNC_DEF) ResolveFuncDef(ctx, bodyElement);
        else if (bodyElement->ntype == FUNC_DECL) ResolveFuncDecl(ctx, bodyElement);
        else if (bodyElement->ntype == OPERATOR_OVERLOAD_NODE) ResolveOperatorOverload(ctx, bodyElement);
        else 
            ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
                "Invalid statement '%s' within struct scope on line %d, col %d.\n",
                bodyElement->token.lexeme, bodyElement->token.row, bodyElement->token.col
            );
    }
}

void ResolveGenStructBody(NameResolverContext* ctx, ASTNode* current)
{
    Debug("GenStructBody");
    for (size_t i = 0; i < current->childCount; i++) {
        ASTNode* bodyElement = current->children[i];
        if (bodyElement->ntype == GEN_DECL_NODE) ResolveVarDecl(ctx, bodyElement);
        else if (bodyElement->ntype == FUNC_DECL) ResolveFuncDecl(ctx, bodyElement);
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
                EnterScope(ctx->arena, ctx->nss);
                ResolveExpr(ctx, ifElifElse->children[0]);
                ResolveBody(ctx, ifElifElse->children[1]);
                ExitScope(ctx->nss);
                break;
            case ELSE_NODE:
                EnterScope(ctx->arena, ctx->nss);
                ResolveBody(ctx, ifElifElse->children[0]);
                ExitScope(ctx->nss);
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
    Debug("ReturnStmt");
    if (current->childCount == 0) return;
    ResolveExpr(ctx, current->children[0]);
}

void ResolveLockStmt(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Lock");
    ResolveExpr(ctx, current->children[0]);
    ResolveBody(ctx, current->children[1]);
}

void ResolveCriticalStmt(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Critical");
    ResolveBody(ctx, current->children[0]);
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
    Debug("Enum");
    char* typeName = current->token.lexeme;
    Environment* typeEnv = GetNamespace(ctx->nss, N_TYPE);
    PushEnvironment(ctx->arena, typeEnv, current, S_TYPEDEF);

    // Treated as an identifier in the scope where the enum was defined
    ASTNode* enumBody = current->children[0];
    Environment* symEnv = GetNamespace(ctx->nss, N_VAR);
    for (size_t i = 0; i < enumBody->childCount; i++) 
        PushEnvironment(ctx->arena, symEnv, enumBody->children[i], S_VAR) ;
}

void ResolveTypedefDecl(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Typedef");
    char* typeName = current->token.lexeme;
    Environment* typeEnv = GetNamespace(ctx->nss, N_TYPE);
    PushEnvironment(ctx->arena, typeEnv, current, S_TYPEDEF);
}

void ResolveStructDecl(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Struct");

    char* typeName = current->token.lexeme;
    Environment* typeEnv = GetNamespace(ctx->nss, N_TYPE);
    PushEnvironment(ctx->arena, typeEnv, current, S_STRUCT);

    // Interface implementation
    int i = 0;
    ASTNode* implementsNode = current->children[0];
    if (implementsNode->ntype == IMPLEMENTS_NODE) {
        ResolveImplements(ctx, implementsNode);
        i++;
    }

    ASTNode* structBody = current->children[i];

    EnterScope(ctx->arena, ctx->nss);
    ResolveStructBody(ctx, structBody);
    ExitScope(ctx->nss);
}

void ResolveGenStructDecl(NameResolverContext* ctx, ASTNode* current)
{
    Debug("GenStruct");

    EnterScope(ctx->arena, ctx->nss);
    ResolveGenericList(ctx, current->children[0]);
    ResolveGenStructBody(ctx, current->children[1]);
    ExitScope(ctx->nss);
}

void ResolveImplements(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Implements");

    for (size_t j = 0; j < current->childCount; j++) {
        ASTNode* interfaceNode = current->children[j];

        char* intName = interfaceNode->token.lexeme;
        Environment* intEnv = GetNamespace(ctx->nss, N_TYPE);
        Symbol* sym = LookupEnvironment(intEnv, intName); 

        if (sym == SYM_DOESNT_EXIST) 
            ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
                "Interface '%s' doesn't exist within current scope on line %d, col %d.\n",
                intName, interfaceNode->token.row, interfaceNode->token.col
            );
    }
}

void ResolveInterfaceDecl(NameResolverContext* ctx, ASTNode* current)
{
    char* intName = current->token.lexeme;
    Environment* typeEnv = GetNamespace(ctx->nss, N_TYPE);
    PushEnvironment(ctx->arena, typeEnv, current, S_TYPEDEF);

    EnterScope(ctx->arena, ctx->nss);

    ASTNode* interfaceBody = current->children[0];
    for (size_t i = 0; i < interfaceBody->childCount; i++) {
        ASTNode* memberNode = interfaceBody->children[i];

        if (memberNode->ntype == VAR_DECL_NODE) ResolveVarDecl(ctx, memberNode);
        else if (memberNode->ntype == FUNC_DECL) ResolveFuncDecl(ctx, memberNode);
    }

    ExitScope(ctx->nss);
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
            ResolveSizeof(ctx, current);
            break;
        case CAST_NODE:
            ResolveCast(ctx, current);
            break;
        case INDEX_NODE:
            ResolveIndex(ctx, current);
            break;
        case LAMBDA_NODE:
            ResolveLambda(ctx, current);
            break;
        default: break; // ERROR
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
    Debug("Cast");
    ResolveType(ctx, current->children[0]);
    char* varName = current->children[1]->token.lexeme;
    Environment* env = GetNamespace(ctx->nss, N_VAR);
    Symbol* sym = LookupEnvironment(env, varName);

    if (sym == SYM_DOESNT_EXIST)
        ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
            "Variable '%s' doesn't exist within current scope on line %d, col %d.\n",
            varName, current->token.row, current->token.col
        );
}

void ResolveIndex(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Index");
    ResolveExpr(ctx, current->children[0]);

    // Handles Nested
    ASTNode* nestedOrVar = current->children[1];
    if (nestedOrVar->ntype == INDEX_NODE) ResolveIndex(ctx, nestedOrVar);
    else if (nestedOrVar->ntype == IDENT_NODE) {
        char* arrName = nestedOrVar->token.lexeme;
        Environment* env = GetNamespace(ctx->nss, N_VAR);
        Symbol* arrSym = LookupEnvironment(env, arrName);

        if (arrSym == SYM_DOESNT_EXIST) 
            ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
                "Cannot index array. Array '%s' doesn't exist within current scope on line %d, col %d.\n",
                arrName, current->token.row, current->token.col
            );
    }
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
    // Struct member technically a part of the type. Type Checkers responsibility
    Debug("Member");
    char* memName = current->token.lexeme;
    Environment* env = GetNamespace(ctx->nss, N_VAR);
    Symbol* sym = LookupEnvironment(env, memName);

    if (sym == SYM_DOESNT_EXIST) 
        ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
            "No struct '%s' exists within current scope on line %d, col %d.\n",
            memName, current->token.row, current->token.col
        );
}

void ResolveReference(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Reference");
    char* refName = current->token.lexeme;
    Environment* env = GetNamespace(ctx->nss, N_VAR);
    Symbol* sym = LookupEnvironment(env, refName);

    if (sym == SYM_DOESNT_EXIST) 
        ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
            "No struct '%s' exists within current scope on line %d, col %d.\n",
            refName, current->token.row, current->token.col
        );
}

void ResolveSizeof(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Sizeof");
    ASTNode* typeNode = current->children[0];
    if (typeNode->ntype == TYPE_NODE) return;   // unambiguous

    char* typeName = typeNode->token.lexeme;
    Environment* typeEnv = GetNamespace(ctx->nss, N_TYPE);
    Symbol* typeSym = LookupEnvironment(typeEnv, typeName);

    if (typeSym == SYM_DOESNT_EXIST) {
        ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
            "Attempting to get the size of an undefined type '%s' within current scope on line %d, col %d.\n",
            typeName, current->token.row, current->token.col
        );
    }
}

/* Others */

void ResolveVar(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Var");
    Environment* env = GetNamespace(ctx->nss, N_VAR);
    PushEnvironment(ctx->arena, env, current, S_VAR);

    // Declaration not definition
    if (current->childCount == 0) return;

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

void ResolveOperatorOverload(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Operator");
    // TODO: Lazy namespace binding in the future
    // TODO: Allow multiple of the same operator, as long as types are diff

    // Operator
    char* operatorNode = current->token.lexeme;
    Environment* opEnv = GetNamespace(ctx->nss, N_OPERATOR);
    PushEnvironment(ctx->arena, opEnv, current, S_OPERATOR);

    EnterScope(ctx->arena, ctx->nss);

    // Paramaters and body
    Environment* env = GetNamespace(ctx->nss, N_VAR);

    for (size_t i = 0; i < current->childCount; i++) {
        ASTNode* paramOrBodyNode = current->children[i];

        if (paramOrBodyNode->ntype == PARAM_NODE) {
            ASTNode* param = paramOrBodyNode;

            ResolveType(ctx, param->children[0]);
            PushEnvironment(ctx->arena, env, param, S_FIELD);
        } else if (paramOrBodyNode->ntype == BODY_NODE) 
            ResolveBody(ctx, paramOrBodyNode);
    } 

    ExitScope(ctx->nss);
}

/* Lists */

void ResolveParamList(NameResolverContext* ctx, ASTNode* current)
{
    Debug("ParamList");
    Environment* env = GetNamespace(ctx->nss, N_VAR);

    for (size_t i = 0; i < current->childCount; i++) {
        ASTNode* param = current->children[i];
        ResolveType(ctx, param->children[0]);
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

void ResolveGenericList(NameResolverContext* ctx, ASTNode* current)
{
    Debug("GenericList");
    for (size_t i = 0; i < current->childCount; i++) 
        ResolveGeneric(ctx, current->children[i]);
}

void ResolveGeneric(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Generic");
    char* typeName = current->token.lexeme;
    Environment* typeEnv = GetNamespace(ctx->nss, N_TYPE);
    PushEnvironment(ctx->arena, typeEnv, current, S_GEN);
}

void ResolveGenericRef(NameResolverContext* ctx, ASTNode* current)
{
    Debug("GenericRef");
    // For generic returns, need to lookup if defined already
    char* typeName = current->token.lexeme;
    Environment* typeEnv = GetNamespace(ctx->nss, N_TYPE);
    
    Symbol* found = LookupEnvironment(typeEnv, typeName);
    if (found == SYM_DOESNT_EXIST || found->stype != S_GEN) {
        ERROR(ERR_FLAG_CONTINUE, NAME_RESOLVER_ERR, 
            "Undefined generic parameter '%s' on line %d, col %d.\n", 
            typeName, current->token.row, current->token.col
        );
    }
}

void ResolveType(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Type");

    switch (current->ntype) {
        case TYPE_NODE: return; // Predefined types
        case CHANNEL_NODE: ResolveChannel(ctx, current); return;
        case CLOSURE_NODE: ResolveClosure(ctx, current); return;
        case FUNC_POINTER_NODE: ResolveFuncPointer(ctx, current); return;
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

void ResolveChannel(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Channel");
    ResolveType(ctx, current->children[0]);
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

void ResolveClosure(NameResolverContext* ctx, ASTNode* current)
{
    Debug("Closure");
    for (size_t i = 0; i < current->childCount; i++) 
        ResolveType(ctx, current->children[i]);
}

void ResolveFuncPointer(NameResolverContext* ctx, ASTNode* current)
{
    Debug("FuncPtr");

    for (size_t i = 0; i < current->childCount; i++) 
        ResolveType(ctx, current->children[i]);
}
