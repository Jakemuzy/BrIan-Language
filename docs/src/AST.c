#include "AST.h"

AST* InitalizeAST(Arena* arena)
{
    AST* ast = AllocateArena(arena, sizeof(AST));
    ast->root = InitalizeASTNode(arena, PROG_NODE, DUMMY_TOKEN);
    return ast;
}

ASTNode* InitalizeASTNode(Arena* arena, NodeType type, Token token)
{
    ASTNode* node = AllocateArena(arena, sizeof(ASTNode));
    node->childCount = 0;
    node->childCapacity = 0;
    node->children = NULL;
    node->type = type;
    node->token = token;

    return node;
}

void AddChildASTNode(Arena* arena, ASTNode* parent, ASTNode* child)
{
    /* This gets means the old memory is completely unused but its the cleanest solution I found */
    /* Only caveat being: statements with lots of children will be less efficient (ie. long if else chains) */
    if (parent->childCount >= parent->childCapacity) {
        size_t newCapacity = parent->childCapacity == 0 ? 4 : parent->childCapacity * 2;
        ASTNode** newChildren = AllocateArena(arena, sizeof(ASTNode*) * newCapacity);
        if (parent->children) {
            memcpy(newChildren, parent->children, sizeof(ASTNode*) * parent->childCount);
        }
        parent->children = newChildren;
        parent->childCapacity = newCapacity;
    }
    parent->children[parent->childCount++] = child;
}

void PrependChildASTNode(Arena* arena, ASTNode* parent, ASTNode* child)
{
    if (parent->childCount >= parent->childCapacity) {
        size_t newCapacity = parent->childCapacity == 0 ? 4 : parent->childCapacity * 2;
        ASTNode** newChildren = AllocateArena(arena, sizeof(ASTNode*) * newCapacity);
        if (parent->children)
            memcpy(newChildren + 1, parent->children, sizeof(ASTNode*) * parent->childCount);
        parent->children = newChildren;
        parent->childCapacity = newCapacity;
    } else {
        memmove(parent->children + 1, parent->children, sizeof(ASTNode*) * parent->childCount);
    }
    parent->children[0] = child;
    parent->childCount++;
}