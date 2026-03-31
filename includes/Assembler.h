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

void AssembleLLVM(AST* ast, Namespaces* nss);
LLVMValueRef* AssembleASTNode(ASTNode* ast, Namespaces* nss, LLVMContextRef* context, LLVMBuilderRef* builder, LLVMModuleRef* modeule);

LLVMContextRef* GenerateContext();
LLVMBuilderRef* GenerateBuilder(LLVMContextRef* ctx);
LLVMModuleRef* GenerateModule(LLVMContextRef* ctx);

void LLVMDestructor(LLVMContextRef* ctx, LLVMBuilderRef* bldr, LLVMModuleRef* mod);

#endif 