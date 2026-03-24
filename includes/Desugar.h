#ifndef _DESUGARIZER_H__
#define _DESUGARIZER_H__

#include <string.h>

#include "AST.h"

/* ---------- Helper ---------- */

Token NewToken(char* lex, TokenType tokType, ASTNode* original);
void ASTInsert(ASTNode* parent, size_t childPos, ASTNode** children, size_t childCount);

/* -------------------- */

AST* Desugar(AST* input);
ASTNode* DesugarNode(ASTNode* input);

ASTNode* DesugarAssignNode(ASTNode* input);
ASTNode* DesugarBinaryNode(ASTNode* input);
ASTNode* DesugarUnaryNode(ASTNode* input);

ASTNode* DesugarFor(ASTNode* input, ASTNode* parent, size_t pos);
ASTNode* DesugarDoWhile(ASTNode* input);
ASTNode* DesugarSwitch(ASTNode* input);
ASTNode* DesugarIf(ASTNode* input);
    ASTNode* DesugarElif(ASTNode* input);

#endif 