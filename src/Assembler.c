#include "Assembler.h"

void AssembleLLVM(AST* ast, Namespaces* nss)
{
    LLVMContextRef* ctx = GenerateContext();
    LLVMBuilderRef* bldr = GenerateBuilder(ctx);
    LLVMModuleRef* mod = GenerateModule(ctx);

    AssembleASTNode(ast->root, nss, ctx, bldr, mod);
}

LLVMValueRef* AssembleASTNode(ASTNode* expr, Namespaces* nss, LLVMContextRef* ctx, LLVMBuilderRef* bldr, LLVMModuleRef* mod)
{   
    switch (expr->type) {
        case (LITERAL_NODE): 
            switch (expr->token.type) {
                case (INTEGRAL):
                    // TODO: Last param is signed extend, need to determine signedness first
                    // But since its a temp value its unknown, the refactor would have to fix this 
                    // design flaw
                    char *endptr;
                    long long val = strtol(expr->token.lex, &endptr, 10);

                    LLVMTypeRef boolType = LLVMInt1TypeInContext(*ctx);
                    LLVMValueRef trueVal = LLVMConstInt(boolType, 1, 0);

                    LLVMTypeRef ty = LLVMIntTypeInContext(*ctx, 32);
                    LLVMValueRef ref = LLVMConstInt(ty, val, trueVal);
                    LLVMBuildStore(bldr, )
                    printf("int\n");
                    break;
                case (DECIMAL):
                    // LLVMTypeRef
                    // LLVMValueRef
                    break;
                case (CLITERAL):
                    break;
                case (SLITERAL):
                    break;
            }
            break;
    }

    /* Recursively assemble node at a time */
    for (size_t i = 0; i < expr->childCount; i++) 
        AssembleASTNode(expr->children[i], nss, ctx, bldr, mod);
}

LLVMContextRef* GenerateContext()
{
    /* Context should store a symbol table of defined types */
    /* 1.) Go through all predefined types, and generate a static table */
    /* 2.) Everytime a user defined type is found, put it here */
    LLVMContextRef context = LLVMContextCreate();
    return &context;
}

LLVMBuilderRef* GenerateBuilder(LLVMContextRef* ctx)
{
    LLVMBuilderRef bldr = LLVMCreateBuilderInContext(*ctx);
    return bldr;
}

LLVMModuleRef* GenerateModule(LLVMContextRef* ctx)
{
    LLVMModuleRef mod = LLVMModuleCreateWithNameInContext("context", *ctx);
    return &mod;
}


void LLVMDestructor(LLVMContextRef* ctx, LLVMBuilderRef* bldr, LLVMModuleRef* mod)
{
    LLVMDisposeBuilder(*bldr);
    LLVMDisposeModule(*mod);
    LLVMContextDispose(*ctx);
}