#include "Assembler.h"

/* ---------- Error Handling ---------- */

void* LLVM_ERR(char* msg, ASTNode* node)
{
    printf("ASEEMBLER ERROR: %s, on line %d \n", msg, node->token.line);
    return NULL;
}

/* ---------- Codegen --------- */

void AssembleLLVM(AST* ast, Namespaces* nss)
{
    LLVMContextRef ctx = GenerateContext();
    LLVMBuilderRef bldr = GenerateBuilder(ctx);
    LLVMModuleRef mod = GenerateModule(ctx);

    AssembleASTNode(ast->root, nss, ctx, bldr, mod);
}

LLVMValueRef AssembleASTNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{   
    switch (expr->type) {
        case (LITERAL_NODE): 
            LLVMTypeRef ty;
            LLVMValueRef ref;

            switch (expr->token.type) {
                case (INTEGRAL):
                    // TODO: Last param is signed extend, need to determine signedness first
                    // But since its a temp value its unknown, the refactor would have to fix this 
                    // design flaw
                    ty = LLVMIntTypeInContext(ctx, 32);
                    ref = LLVMConstIntOfString(ty, expr->token.lex.word, 10);
                    return ref;
                case (DECIMAL):
                    ty = LLVMDoubleTypeInContext(ctx);
                    ref = LLVMConstRealOfString(ty, expr->token.lex.word);
                    return ref;
                case (CLITERAL):
                    ty = LLVMIntTypeInContext(ctx, 8);
                    ref = LLVMConstIntOfString(ty, expr->token.lex.word, true);
                    break;
                case (SLITERAL):
                    /* Multiple chars */
                    break;
            }
            break;
        case UNARY_EXPR_NODE:
            return AssembleUnaryNode(expr, nss, ctx, bldr, mod);
        case BINARY_EXPR_NODE: 
            return AssembleBinaryNode(expr, nss, ctx, bldr, mod);
    }

    /* Recursively assemble node at a time */
    for (size_t i = 0; i < expr->childCount; i++) 
        AssembleASTNode(expr->children[i], nss, ctx, bldr, mod);

    return NULL;
}

LLVMValueRef AssembleBinaryNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{
    LLVMValueRef lhs = AssembleASTNode(expr->children[0], nss, ctx, bldr, mod);
    LLVMValueRef rhs = AssembleASTNode(expr->children[1], nss, ctx, bldr, mod);

    if (!lhs || !rhs)
        return LLVM_ERR("Couldn't evalualte binary node", expr);

    TokenType type = expr->token.type;
    switch (type) {
        case (PLUS):
            printf("PLUH\n");
            return LLVMBuildAdd(bldr, lhs, rhs, "addtmp");
        //return LLVMBuilder()
        default: 
            return LLVM_ERR("Invalid binary operator", expr);
    }

    return NULL;
}

LLVMValueRef AssembleUnaryNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{
    return NULL;
}

LLVMValueRef AssembleVarNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{

}

LLVMValueRef AssembleFuncNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{
    /* Check predefined types, if not there, check user defined types */
    LLVMTypeRef* returnRef = AssembleASTNode(expr->children[0], nss, ctx, bldr, mod);
    LLVMTypeRef* paramsRef = AssembleParamsNode(expr->children[1], nss, ctx, bldr, mod);

    /* TODO: Parse Body Node as well here */

    /* False since variadic arguments aren't implemented yet */
    LLVMTypeRef ref = LLVMFunctionType(returnRef, paramsRef, expr.children[1]->childCount, false)
    return ref;
}

LLVMValueRef AssembleParamsNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{

}

LLVMValueRef AssembleWhileLoop(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{

}

LLVMValueRef AssembleDoWhileLoop(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{

}

LLVMValueRef AssembleForLoop(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{

}

LLVMValueRef AssembleIfStmt(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{

}

LLVMValueRef AssembleSwitchStmt(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{

}

LLVMValueRef AssembleReturnStmt(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{

}

LLVMValueRef AssembleTypedef(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{

}

LLVMValueRef AssembleStructDecl(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{

}

/* ---------- LLVM Functions ---------- */


LLVMContextRef GenerateContext()
{
    /* Context should store a symbol table of defined types */
    /* 1.) Go through all predefined types, and generate a static table */
    /* 2.) Everytime a user defined type is found, put it here */
    return LLVMContextCreate();
}

LLVMBuilderRef GenerateBuilder(LLVMContextRef ctx) { return LLVMCreateBuilderInContext(ctx); }

LLVMModuleRef GenerateModule(LLVMContextRef ctx) { return LLVMModuleCreateWithNameInContext("context", ctx); }


void LLVMDestructor(LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{
    LLVMDisposeBuilder(bldr);
    LLVMDisposeModule(mod);
    LLVMContextDispose(ctx);
}