#include "NameResolver.h"

/* ----------- Helper ----------- */

bool IdentIsDecl(ASTNode* ident, ASTNode* parent)
{
    if (!parent) return false;

    return parent->type == FUNC_NODE || parent->type == DECL_STMT_NODE || parent->type == PARAM_NODE;
}

/* ----------- Name Resolution ---------- */

void PrintScope(void)
{
    Scope* scope = CurrentScope;
    int depth = 0;

    printf("=== Scope Stack ===\n");

    while (scope) {
        printf("Scope %d%s:\n", depth,
               depth == 0 ? " (current)" : "");

        size_t i;
        for (i = 0; i < scope->symCount; i++) {
            Symbol* sym = scope->symbols[i];
            printf("  %s\n", sym->name);
        }

        scope = scope->prev;
        depth++;
    }

    printf("===================\n");
}

void ResolveNames(AST* ast) 
{
    printf("Resolving Names in Prog\n");
    ASTNode* root = ast->root;
    ResolveNamesInNode(root, NULL);
}

void ResolveNamesInNode(ASTNode* current, ASTNode* parent) {
    /* TODO: Have separate namespaces (ie typedef, etc) */

    if (current->type == PROG_NODE || current->type == FUNC_NODE || current->type == BODY_NODE) 
        BeginScope();

    if (current->type == IDENT_NODE && IdentIsDecl(current, parent)) {
        char* name = current->token.lex.word;
        if (LookupCurrentScope(name))       /* TODO: make decl_stmt and func have a token of their ident instead of having ident as a child */
            NERROR(name);

        STPush(current);
        PrintScope();
    }

    int i;
    for (i = 0; i < current->childCount; i++) {
        ResolveNamesInNode(current->children[i], current);
    }
    
    if (current->type == PROG_NODE || current->type == BODY_NODE) 
        ExitScope();
    
}
