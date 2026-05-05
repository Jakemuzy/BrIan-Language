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

    //InitalizeStart(ctx, bldr, mod);
    AssembleASTNode(ast->root, nss, ctx, bldr, mod);

    char outMsg[512];
    LLVMVerifierFailureAction failureAct = LLVMAbortProcessAction;
    //LLVMVerifyModule(mod, failureAct, &outMsg);

    LLVMDumpModule(mod);
    OptimizeModule(mod);
    printf("------------------------------------------------\n");
    LLVMDumpModule(mod);

    CodegenModule(mod);

    LinkModule(mod);

    LLVMDestructor(ctx, bldr, mod);
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
                    return ref;
                case (SLITERAL):
                    /* Multiple chars */
                    break;
            }
            break;


        case FUNC_NODE: return AssembleFuncNode(expr, nss, ctx, bldr, mod);
        case BINARY_EXPR_NODE: return AssembleBinaryNode(expr, nss, ctx, bldr, mod);
        case UNARY_EXPR_NODE: return AssembleUnaryNode(expr, nss, ctx, bldr, mod);

        //case ASGN_EXPR_NODE: 
        case VAR_DECL_NODE:  return AssembleVarNode(expr, nss, ctx, bldr, mod);
        /*
        case CALL_FUNC_NODE: 

        case DO_WHILE_STMT_NODE: 
        case WHILE_STMT_NODE:    
        case FOR_STMT_NODE:      
        case IF_STMT_NODE:       
        case SWITCH_STMT_NODE:   
        */
        case RETURN_STMT_NODE: return AssembleReturnStmt(expr, nss, ctx, bldr, mod);
        /*

        case ARR_DECL_NODE:     
        case ARR_INIT_NODE:     
        case ARR_INDEX_NODE:   

        case MEMBER_ACCESS_NODE:
        case TYPEDEF_DECL_NODE:

        case ENUM_BODY_NODE:    

        case STRUCT_DECL_NODE: 
        */
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

    /* Cast as LLVM requires same type */
    if (LLVMTypeOf(lhs) != LLVMTypeOf(rhs))
        rhs = LLVMBuildSIToFP(bldr, rhs, LLVMTypeOf(lhs), "casttmp");

    bool isFloat = LLVMGetTypeKind(LLVMTypeOf(lhs)) == LLVMFloatTypeKind || LLVMGetTypeKind(LLVMTypeOf(lhs)) == LLVMDoubleTypeKind;

    // LLVMBuildBinOp ??
    TokenType type = expr->token.type;
    switch (type) {
        case (PLUS): 
            return isFloat ? LLVMBuildFAdd(bldr, lhs, rhs, "addtmp")
                        : LLVMBuildAdd(bldr, lhs, rhs, "addtmp");
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
    // LLVMBuildAlloca for stack
    // LLVMBuildStore for heap or stack (predefined area of mem)
    LLVMTypeRef type = AssembleType(expr->children[0], nss, ctx, bldr, mod);

    ASTNode* varListNode = expr->children[1];
    for (size_t i = 0; i < varListNode->childCount; i++) 
        AssembleVar(varListNode->children[i], nss, ctx, bldr, mod, type);

    return type;
}

LLVMValueRef AssembleVar(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod, LLVMTypeRef type)
{
    // TOOD: Need to check = with another rhs check 
    ASTNode* identNode = expr->children[0];
    LLVMValueRef val = LLVMBuildAlloca(bldr, type, identNode->token.lex.word);

    if (expr->childCount > 1 ) {
        ASTNode* rhs = expr->children[1];
        LLVMValueRef init = AssembleASTNode(rhs, nss, ctx, bldr, mod);
        if (!init) printf("NOT INIT\n");

        if (type != LLVMTypeOf(init)) 
            init = LLVMBuildFPTrunc(bldr, init, type, "fptrunctmp");

        LLVMBuildStore(bldr, init, val);    
    }

    return val;
}


LLVMValueRef AssembleFuncNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{
    /* Check predefined types, if not there, check user defined types */
    LLVMTypeRef* returnRef = AssembleType(expr->children[0], nss, ctx, bldr, mod);

    ASTNode* identNode = expr->children[1];
    LLVMTypeRef* paramsRef = AssembleParamsNode(expr->children[2], nss, ctx, bldr, mod);

    /* False since variadic arguments aren't implemented yet */
    size_t paramCount = (paramsRef == NULL ) ? 0 : expr->children[2]->childCount;


    /* TODO: This should be in AssembleType?? */
    LLVMTypeRef ref = LLVMFunctionType(returnRef, paramsRef, paramCount, false);
    LLVMValueRef val = LLVMAddFunction(mod, identNode->token.lex.word, ref);

    LLVMBasicBlockRef funcBlock = LLVMAppendBasicBlockInContext(ctx, val, identNode->token.lex.word);
    LLVMPositionBuilderAtEnd(bldr, funcBlock);

    /* Body after block, since everything in body is inside the block */
    LLVMValueRef* bodyRef = AssembleASTNode(expr->children[3], nss, ctx, bldr, mod);
    //LLVMValueRef tmp = LLVMBuildAdd(bldr,, strcat(identNode, "_tmp"));
    //LLVMBuildRet(bldr, tmp);

    return ref;
}

LLVMTypeRef* AssembleParamsNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{
    if (expr->children[0]->type == EMPTY_NODE) return NULL;

    LLVMTypeRef* params = malloc(sizeof(LLVMTypeRef) * expr->childCount);
    for (size_t i = 0; i < expr->childCount; i++)  
        /* Lookup symbol table and get type from there, and value ref */ 
        params[i] = AssembleParamNode(expr->children[i], nss, ctx, bldr, mod);
    return params;
}

LLVMTypeRef AssembleParamNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{
    ASTNode* typeNode = expr->children[0];
    return AssembleType(typeNode, nss, ctx, bldr, mod);
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
    LLVMValueRef ret;
    ASTNode* retValueNode = expr->children[0];

    if (expr->token.type == VOID) ret = LLVMBuildRetVoid(bldr);
    else ret = LLVMBuildRet(bldr, AssembleASTNode(retValueNode, nss, ctx, bldr, mod));

    return ret;
}

LLVMTypeRef AssembleType(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{
    char* typeLex = expr->token.lex.word;

    /* No error checking, since should be GUARANTEED to exist based on type checking phase */
    TYPE* type = StrToType(typeLex);
    if (!type) {
        Symbol* sym = STLookupNamespace(nss, typeLex, N_TYPE);
        type = sym->type;
    }

    switch (type->kind) {
        case (TYPE_BOOL): return LLVMInt8TypeInContext(ctx);
        case (TYPE_INT): return LLVMInt32TypeInContext(ctx);
        /* No distinction between signed and unsigned in LLVM */
        case (TYPE_U8): case (TYPE_I8): return LLVMInt8TypeInContext(ctx);
        case (TYPE_U16): case (TYPE_I16): return LLVMInt16TypeInContext(ctx);
        case (TYPE_U32): case (TYPE_I32): return LLVMInt32TypeInContext(ctx);
        case (TYPE_U64): case (TYPE_I64): return LLVMInt64TypeInContext(ctx);
        case (TYPE_FLOAT): return LLVMFloatTypeInContext(ctx);
        case (TYPE_DOUBLE): return LLVMDoubleTypeInContext(ctx);

        /* User defined types are a bit interesting */
        case (TYPE_ENUM): return LLVMInt32TypeInContext(ctx);
        case (TYPE_VOID): return LLVMVoidTypeInContext(ctx);
        case (TYPE_STRUCT): /* Sequential type? */
        case (TYPE_STRING): /* Sequential type */

        /* 
        
        */

        /*  
        STRING: 
            char* str;
            size_t len;
            size_t max; 
        */
    }

}

LLVMValueRef AssembleTypedef(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{

}

LLVMValueRef AssembleStructDecl(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{

}

/* ---------- LLVM Functions ---------- */

void OptimizeModule(LLVMModuleRef mod)
{
    LLVMPassBuilderOptionsRef options = LLVMCreatePassBuilderOptions();
    LLVMErrorRef err = LLVMRunPasses(mod, "default<O2>", NULL, options);
    if (err) {
        char* msg = LLVMGetErrorMessage(err);
        printf("Pass error: %s\n", msg);
        LLVMDisposeErrorMessage(msg);
    }
    LLVMDisposePassBuilderOptions(options);
}

void CodegenModule(LLVMModuleRef mod)
{
    /* When targetting embedded eventually pick the specific target */
    //LLVMInitializeNativeTargetInfo();
    LLVMInitializeNativeTarget();
    //LLVMInitalizeNativeTargetAsmPrinter();
    //LLVMInitializeNativeTargetMC();
    LLVMInitializeNativeAsmPrinter();

    char* triple = LLVMGetDefaultTargetTriple();
    LLVMTargetRef target;
    char* err;
    LLVMGetTargetFromTriple(triple, &target, &err);

    LLVMTargetMachineRef tm = LLVMCreateTargetMachine(
        target, triple, "generic", "",
        LLVMCodeGenLevelAggressive, // Aggressive since targetting embedded
        LLVMRelocDefault,
        LLVMCodeModelDefault
    );

    LLVMTargetMachineEmitToFile(tm, mod, "output.o", LLVMObjectFile, &err);
}

void LinkModule(LLVMModuleRef mod)
{
}

LLVMContextRef GenerateContext()
{
    /* Context should store a symbol table of defined types */
    /* 1.) Go through all predefined types, and generate a static table */
    /* 2.) Everytime a user defined type is found, put it here */
    return LLVMContextCreate();
}

LLVMBuilderRef GenerateBuilder(LLVMContextRef ctx) { return LLVMCreateBuilderInContext(ctx); }

LLVMModuleRef GenerateModule(LLVMContextRef ctx) { return LLVMModuleCreateWithNameInContext("BrIan", ctx); }


void LLVMDestructor(LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod)
{
    LLVMDisposeBuilder(bldr);
    LLVMDisposeModule(mod);
    LLVMContextDispose(ctx);
}