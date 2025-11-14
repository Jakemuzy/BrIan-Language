#include "Tokenizer.h"

#define ERRP -1
#define NAP
#define VALID 1

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

AST* ConstructAST(FILE* fptr);

/* ---------- Recursive Descent ---------- */

void Prog(FILE* fptr, AST* ast);


