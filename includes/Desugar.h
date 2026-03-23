#ifndef _DESUGARIZER_H__
#define _DESUGARIZER_H__

#include "AST.h"

AST* Desugar(AST* input);
ASTNode* DesugarNode(ASTNode* input);

/* -------------------- */

ASTNode* DesugarBinaryNode(ASTNode* input);
ASTNode* DesugarUnaryNode(ASTNode* input);

ASTNode* DesugarFor(ASTNode* input);
ASTNode* DesugarDoWhile(ASTNode* input);
ASTNode* DesugarSwitch(ASTNode* input);
ASTNode* DesugarIf(ASTNode* input);
    ASTNode* DesugarElif(ASTNode* input);

#endif 