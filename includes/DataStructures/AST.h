#ifndef _AST_H__
#define _AST_H__

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#include "Token.h"

typedef enum NodeType {
    PROG_NODE, FUNC_NODE, PARAM_LIST_NODE, PARAM_NODE,
    BODY_NODE, STMT_LIST_NODE, STMT_NODE,

    EXPR_STMT_NODE, DECL_STMT_NODE, CTRL_STMT_NODE, RETURN_STMT_NODE,

    IF_STMT_NODE, SWITCH_STMT_NODE, WHILE_STMT_NODE, DO_WHILE_STMT_NODE, FOR_STMT_NODE,
    IF_NODE, ELIF_NODE, ELSE_NODE, CASE_NODE, DEFAULT_NODE,

    EXPR_LIST_NODE, EXPR_NODE,

    TYPE_NODE, ARG_LIST_NODE, VAR_LIST_NODE, VAR_NODE,
    IDENT_NODE, LITERAL_NODE, EMPTY_NODE, 
    ASGN_EXPR_NODE, BINARY_EXPR_NODE, UNARY_EXPR_NODE, CALL_FUNC_NODE, ARR_INDEX_NODE, ARR_INIT_NODE
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
int ASTPushChildNode(ASTNode* node, ASTNode* child);
int ASTFreeNodes(int count, ...);

#endif 
