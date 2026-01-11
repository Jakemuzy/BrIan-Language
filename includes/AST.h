#ifndef _AST_H__
#define _AST_H__

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "Token.h"

typedef struct ASTNode
{
    struct ASTNode** children; 
    size_t childCount;
    Token token;
} ASTNode;

typedef struct AST
{
    ASTNode* root;
} AST;

AST* ASTInit();
int ASTFree();

ASTNode* InitASTNode();
int ASTPushTokNode(ASTNode* node, Token t);
int ASTPushChildNode(ASTNode* node, ASTNode* child);
int ASTFreeNode(ASTNode* node);

#endif 
