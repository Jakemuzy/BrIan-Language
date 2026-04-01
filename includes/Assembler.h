#ifndef _ASSEMBLER_H__
#define _ASSEMBLER_H__

#include <stdlib.h>

#include "TypeChecker.h"
#include "Desugar.h"

#include "llvm-c/Core.h"

//#define LLVMValueRef Value

/* Symbol Table for llvm code

std::map<std::string, Value*> NamedValues;
*/

/* ---------- Error Handling ---------- */

void* LLVM_ERR(char* msg, ASTNode* node);

/* ---------- Codegen --------- */

void AssembleLLVM(AST* ast, Namespaces* nss);
LLVMValueRef AssembleASTNode(ASTNode* ast, Namespaces* nss, LLVMContextRef context, LLVMBuilderRef builder, LLVMModuleRef module);

LLVMValueRef AssembleVarNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);
    LLVMValueRef AssembleVar(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod, LLVMTypeRef type);
LLVMValueRef AssembleFuncNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);
    LLVMTypeRef* AssembleParamsNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);
    LLVMTypeRef AssembleParamNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);

LLVMValueRef AssembleBinaryNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);
LLVMValueRef AssembleUnaryNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);

LLVMValueRef AssembleWhileLoop(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);
LLVMValueRef AssembleDoWhileLoop(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);
LLVMValueRef AssembleForLoop(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);
LLVMValueRef AssembleIfStmt(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);
LLVMValueRef AssembleSwitchStmt(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);
LLVMValueRef AssembleReturnStmt(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);

LLVMTypeRef AssembleType(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);
LLVMValueRef AssembleTypedef(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);
LLVMValueRef AssembleStructDecl(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);

/* ---------- LLVM Functions ---------- */

LLVMContextRef GenerateContext();
LLVMBuilderRef GenerateBuilder(LLVMContextRef ctx);
LLVMModuleRef GenerateModule(LLVMContextRef ctx);

void LLVMDestructor(LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);

#endif 