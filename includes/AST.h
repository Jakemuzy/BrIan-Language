#ifndef _AST_H__
#define _AST_H__

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "Token.h"

typedef enum NodeType {
    PROG_NODE, FUNC_NODE, PARAM_LIST_NODE, PARAM_NODE,
    BODY_NODE, STMT_LIST_NODE, STMT_NODE,
    EXPR_STMT_NODE, DECL_STMT_NODE, CTRL_STMT_NODE, RETURN_STMT_NODE,
    IF_STMT_NODE, SWITCH_STMT_NODE, WHILE_STMT_NODE, DO_WHILE_STMT_NODE, FOR_STMT_NODE,
    EXPR_LIST_NODE, EXPR_NODE,
    /* The following nodes will be known by their token type or the operator they apply on their children */
    TOK_NODE,	/* ie.) Instead of ADD_NDOE, SUB_NODE ... they will simply use their token type */
    TYPE_NODE, ARG_LIST_NODE, VAR_LIST_NODE, VAR_NODE
} NodeType;

typedef struct ASTNode
{
    struct ASTNode** children; 
    size_t childCount;
    NodeType type;
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
int ASTPushChildNode(ASTNode* node, ASTNode* child, NodeType type);
int ASTFreeNode(ASTNode* node);

#endif 
