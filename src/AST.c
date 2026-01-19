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
    ASTFreeNodes(1, ast->root);
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
    node->type = EMPTY_NODE;
    return node;
}


int ASTPushChildNode(ASTNode* node, ASTNode* child)
{
    int index = node->childCount;

    ASTNode** tmp = realloc(node->children, (index + 1) * sizeof(ASTNode*));
    if (!tmp) return -1;

    node->children = tmp;
    node->children[index] = child;
    node->childCount++;
    return 0;
}

int ASTFreeNodes(int count, ...)
{
    va_list args; 
    va_start(args, count);

    int i;
    for(i = 0; i < count; i++)
    {
        ASTNode* node = va_arg(args, ASTNode*); 
        if (!node) continue;

        int j = 0;
        for (j = 0; j < node->childCount; j++) {
            ASTFreeNodes(1, node->children[j]);
        }

        free(node->children);
        free(node);
    }

    va_end(args);
    return 0;
}
