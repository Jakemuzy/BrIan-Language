#include "AST.h"

AST* ASTInit(FILE* fptr)
{
    AST* ast = malloc(sizeof(AST));
    if (!ast) return NULL;

    ast->root = InitASTNode();
    if (!ast->root) {
        free(ast);
        return NULL;
    }

    return ast;
}

int ASTFree(AST* ast)
{
    if (!ast) return -1;
    ASTFreeNode(ast->root);
    free(ast);
    return 0;
}


/* ---------- Nodes ---------- */

ASTNode* InitASTNode()
{
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    node->children = NULL;
    node->childCount = 0;
    node->nodeType = TOK_NODE;
    return node;
}


int ASTPushTokNode(ASTNode* node, Token t)
{
    int index = node->childCount;

    ASTNode** tmp = realloc(node->children, (index + 1) * sizeof(ASTNode*));
    if (!tmp) return -1;
    node->children = tmp;

    ASTNode* child = InitASTNode();
    if (!child) return -1;

    child->token = t;
    node->nodeType = TOK_NODE;

    node->children[index] = child;
    node->childCount++;

    return 0;
}

int ASTPushChildNode(ASTNode* node, ASTNode* child, NodeType type)
{
    int index = node->childCount;

    ASTNode** tmp = realloc(node->children, (index + 1) * sizeof(ASTNode*));
    if (!tmp) return -1;

    node->children = tmp;
    node->children[index] = child;
    node->childCount++;
    node->type = type;

    return 0;
}

int ASTFreeNode(ASTNode* node)
{
    if (!node) return -1;

    int i = 0;
    for (i = 0; i < node->childCount; i++) {
        ASTFreeNode(node->children[i]);
    }

    free(node->children);
    free(node);

    return 0;
}
