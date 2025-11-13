#include "Tokenizer.h"

typedef struct ASTNode
{
    struct ASTNode* children; 
    int count;
    Token t;
} ASTNode;

typedef struct AST
{
    ASTNode* root;
} AST;

AST* ConstructAST();

