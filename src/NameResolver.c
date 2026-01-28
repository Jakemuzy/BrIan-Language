#include "NameResolver.h"

Scope* CurrentScope = NULL;

/* ----------- Error Handling ---------- */

bool NERROR_NO_IDENT(ASTNode* curr)
{
    printf("NAME ERROR: No Identifier found on line '%d'\n", curr->token.line); 
    return ERRN;
}

bool NERROR_ALREADY_DEFINED(char* name, ASTNode* curr, ASTNode* first)
{
    printf("NAME ERROR: Identifier '%s' on line %d already defined, first definition on line %d\n", name, curr->token.line, first->token.line); \
    return ERRN; 
}

bool NERROR_DOESNT_EXIST(char* name, ASTNode* curr) 
{
    printf("NAME ERROR: Identifier '%s' on line %d, hasn't been previously defined\n", name, curr->token.line);
    return ERRN;
}


/* ----------- Helper ----------- */

ASTNode* FindIdentChild(ASTNode* node) {
    size_t i;
    for (i = 0; i < node->childCount; i++) {
        if ((node->children[i])->type == IDENT_NODE) 
            return node->children[i];
    }
    return NULL;
}

bool IdentIsDecl(ASTNode* ident, ASTNode* parent)
{
    NodeType type = parent->type;
    return type == VAR_NODE || type == PARAM_NODE;
}


bool CanEnterOrExitScope(ASTNode* node) 
{
    /* TODO: Have these as a static array to iterate through */
    /* TODO: Avoid Variable shadowing in these types of nodes */
    NodeType type = node->type;
    if (type == FUNC_NODE || type == IF_NODE ||
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

Symbol** ResolveNames(AST* ast) 
{
    printf("Resolving Names in Prog\n");
    ASTNode* root = ast->root;
    BeginScope(&CurrentScope);
    if(!ResolveNamesInNode(root, NULL))
        return NULL;
    ExitScope(&CurrentScope);

    return SymbolTable;
}

bool ResolveNamesInNode(ASTNode* current, ASTNode* parent) 
{
    if (current->type == FUNC_NODE) {
        ASTNode* funcIdent = FindIdentChild(current);
        if (!funcIdent)
            return NERROR_NO_IDENT(funcIdent);

        Symbol* sym = STPush(funcIdent);
        PushScope(&CurrentScope, sym);
    }

    if (CanEnterOrExitScope(current))
        BeginScope(&CurrentScope);

    if (current->type == IDENT_NODE && IdentIsDecl(current, parent)) {
        char* name = current->token.lex.word;
        if (LookupCurrentScope(&CurrentScope, name)) 
            return NERROR_ALREADY_DEFINED(name, current, STLookup(name)->decl);

        Symbol* sym = STPush(current);
        PushScope(&CurrentScope, sym);
        PrintScope();
    }
    else if (current->type == BINARY_EXPR_NODE || current->type == UNARY_EXPR_NODE || current->type == ASGN_EXPR_NODE) {
        char* name = FindIdentChild(current)->token.lex.word;
        if (!STLookup(name)) 
            return NERROR_DOESNT_EXIST(name, current);
        /* No pushing, just checking */    
    }


    /* Recursively check children */
    int i;
    for (i = 0; i < current->childCount; i++) {
        if(ResolveNamesInNode(current->children[i], current) == ERRN)
            return ERRN;
    }

    if (CanEnterOrExitScope(current))
        ExitScope(&CurrentScope);

    return VALDN;
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