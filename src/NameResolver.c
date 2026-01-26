#include "NameResolver.h"

Scope* CurrentScope = NULL;

/* ----------- Helper ----------- */

bool IdentIsDecl(ASTNode* ident, ASTNode* parent)
{
    if (!parent) return false;


    NodeType type = parent->type;
    printf("DECL %d\n", type);
    return type == VAR_NODE || type == PARAM_NODE;
}

ASTNode* FuncIdent(ASTNode* funcNode) {
    size_t i;
    for (i = 0; i < funcNode->childCount; i++) {
        if ((funcNode->children[i])->type == IDENT_NODE) 
            return funcNode->children[i];

        printf("%s type: %d\n", funcNode->children[i]->token.lex.word, funcNode->children[i]->type);
    }
    return NULL;
}

bool CanEnterOrExitScope(ASTNode* node) 
{
    /* TODO: Have these as a static array to iterate through */
    NodeType type = node->type;
    if (type == FUNC_NODE || type == IF_STMT_NODE || type == IF_NODE ||
        type == ELIF_NODE || type == ELSE_NODE  || type == SWITCH_STMT_NODE ||
        type == CASE_NODE || type == DEFAULT_NODE || type == WHILE_STMT_NODE ||
        type == DO_WHILE_STMT_NODE || type == FOR_STMT_NODE) {
            return true;
    }

    return false;
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
    BeginScope(&CurrentScope);
    ResolveNamesInNode(root, NULL);
    ExitScope(&CurrentScope);
}

void ResolveNamesInNode(ASTNode* current, ASTNode* parent) 
{
    if (current->type == FUNC_NODE) {
        ASTNode* funcIdent = FuncIdent(current);
        if (!funcIdent)
            NERROR("Function has no Identifier");

        Symbol* sym = STPush(funcIdent);
        PushScope(&CurrentScope, sym);
    }

    if (current->type == IDENT_NODE && IdentIsDecl(current, parent)) {
        char* name = current->token.lex.word;
        if (LookupCurrentScope(&CurrentScope, name)) {
            char buff[256];
            snprintf(buff, sizeof(buff), "Identifier already declared in scope: %s", name);
            NERROR(buff);
        }

        printf("Ident is DECL: %s\n", name);
        Symbol* sym = STPush(current);
        PushScope(&CurrentScope, sym);
        PrintScope();
    }

    if (CanEnterOrExitScope(current))
        BeginScope(&CurrentScope);

    /* Recursively check children */
    int i;
    for (i = 0; i < current->childCount; i++) {
        ResolveNamesInNode(current->children[i], current);
    }

    if (CanEnterOrExitScope(current))
        ExitScope(&CurrentScope);
}


/*
CASES:
    Decl Node 
        1.) Resolve Children
        2.) For each child, if Ident, check if parent is Decl,
            if so, push to push symbol
    Body Nodes
        1.) Any body, IfStmt, ElifStmt, Else, Switch, Case, Default, 
            Whilestmt, DoWhileStmt, ForStmt, all have own scope
        2.) Get ident from child, 
        3.) Enter scope
        4.) Resolve Children
    Function Node
        1.) Push function symbol (get ident)
        2.) Enter Scope
        3.) Resolve Children

*/