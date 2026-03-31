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

LLVMValueRef AssembleBinaryNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);
LLVMValueRef AssembleUnaryNode(ASTNode* expr, Namespaces* nss, LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);

/* ---------- LLVM Functions ---------- */

LLVMContextRef GenerateContext();
LLVMBuilderRef GenerateBuilder(LLVMContextRef ctx);
LLVMModuleRef GenerateModule(LLVMContextRef ctx);

void LLVMDestructor(LLVMContextRef ctx, LLVMBuilderRef bldr, LLVMModuleRef mod);

#endif 