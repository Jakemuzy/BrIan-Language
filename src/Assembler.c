#include "Assembler.h"

/* ---------- Error Handling ---------- */

void* LLVM_ERR(char* msg, ASTNode* node)
{
    printf("%s, on line %d \n", msg, node->token.line);
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
                    char *endptr;
                    long long val = strtol(expr->token.lex.word, &endptr, 10);

                    /* TODO: Use LLVMConstIntOf String, it auto parses */
                    ty = LLVMIntTypeInContext(ctx, 32);
                    ref = LLVMConstInt(ty, val, true);
                    return ref;
                case (DECIMAL):

                    ty = LLVMIntTypeInContext(ctx, 64);
                    ref = LLVMConstRealOfString(ty, expr->token.lex.word);
                    return ref;
                case (CLITERAL):
                    break;
                case (SLITERAL):
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

    if (!lhs | !rhs)
        return LLVM_ERR("Couldn't evalualte binary node", expr);

    char op = expr->token.lex.word[0];
    switch (op) {
        case ('+'):
            printf("PLUH\n");
            break;
        //return LLVMBuilder()
    }

    return NULL;
}

LLVMValueRef AssembleUnaryNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{
    return NULL;
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