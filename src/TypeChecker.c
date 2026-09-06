#include "TypeChecker.h"

/*
    TODO: visitor pattern probably works better for type checking, instead
    of using many switch statements. This only works in other phases (like name resolution)
    is because the parser needs to.

    Should really be table driven to be honest.
*/

TypeCheckerContext* InitalizeTypeCheckerContext(AST* ast)
{
    TypeCheckerContext* ctx = malloc(sizeof(TypeCheckerContext));
    ctx->ast = ast;
    return ctx; 
}

/* ----- Actual Type Checking ----- */

TYPE* TypeCheckProgram(TypeCheckerContext* ctx)
{
    printf("Type Cheking...\n");
    ASTNode* root = ctx->ast->root;

    for (size_t i = 0; i < root->childCount; i++) {
        ASTNode* current = root->children[i];

        switch (current->ntype) {
            case LITERAL_NODE:
            case FUNC_DECL: return TypeCheckFuncDecl(ctx, current);
            case FUNC_DEF: return TypeCheckFuncDef(ctx, current);
            case GEN_FUNC_DECL: return TypeCheckGenFuncDecl(ctx, current);
            case GEN_FUNC_DEF: return TypeCheckGenFuncDef(ctx, current);
            /* ... Return Type ... */
            case BODY_NODE: return TypeCheckBody(ctx, current);
            case STRUCT_BODY_NODE: return TypeCheckStructBody(ctx, current);
            case IF_STMT_NODE: return TypeCheckIfStmt(ctx, current);
            case SWITCH_STMT_NODE: return TypeCheckSwitchStmt(ctx, current);
            case WHILE_STMT_NODE: return TypeCheckWhileStmt(ctx, current);
            case DO_WHILE_STMT_NODE: return TypeCheckDoWhileStmt(ctx, current);
            case FOR_STMT_NODE: return TypeCheckForStmt(ctx, current);
            case RETURN_STMT_NODE: return TypeCheckReturnStmt(ctx, current);
            case LOCK_STMT_NODE: return TypeCheckLockStmt(ctx, current);
            case CRITICAL_STMT_NODE: return TypeCheckCriticalStmt(ctx, current);
            case VAR_DECL_NODE: return TypeCheckVarDecl(ctx, current);
            case ENUM_DECL_NODE: return TypeCheckEnumDecl(ctx, current);
            case TYPEDEF_DECL_NODE: return TypeCheckTypedefDecl(ctx, current);
            case STRUCT_DECL_NODE: return TypeCheckStructDecl(ctx, current);
            case GEN_STRUCT_DECL_NODE: return TypeCheckGenStructDecl(ctx, current);
            case IMPLEMENTS_NODE: return TypeCheckImplements(ctx, current);
            case INTERFACE_DECL_NODE: return TypeCheckInterfaceDecl(ctx, current);

            default:
        }
    }

}

/* Functions */
TYPE* TypeCheckFuncDecl(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckFuncDef(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckGenFuncDecl(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckGenFuncDef(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckReturnType(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckBody(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckStructBody(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckGenStructBody(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}


/* Ctrl Stmts */
TYPE* TypeCheckIfStmt(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckSwitchStmt(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckWhileStmt(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckDoWhileStmt(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckForStmt(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckReturnStmt(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckLockStmt(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckCriticalStmt(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}


/* Decls */
TYPE* TypeCheckVarDecl(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckEnumDecl(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckTypedefDecl(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckStructDecl(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckGenStructDecl(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckImplements(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckInterfaceDecl(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}


/* Exprs */
TYPE* TypeCheckExpr(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckBinaryExpr(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckAsgnExpr(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckPrefixExpr(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckPostfixExpr(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckTernaryExpr(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}


/* Operations */
TYPE* TypeCheckCast(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckIndex(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckFuncCall(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckMember(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckReference(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}
 // Bad name, refers to ->
TYPE* TypeCheckSizeof(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}


/* Others */
TYPE* TypeCheckVar(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckArrDecl(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckArrInit(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckCaptures(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckOperatorOverload(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}


/* Lists */
TYPE* TypeCheckParamList(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckArgList(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}


/* Types */
TYPE* TypeCheckGenericList(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckGeneric(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckGenericRef(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckType(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckMatrix(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckVector(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckChannel(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckLambda(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckClosure(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

TYPE* TypeCheckFuncPointer(TypeCheckerContext* ctx, ASTNode* current)
{
return NULL;
}

