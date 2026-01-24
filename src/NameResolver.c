#include "NameResolver.h"



/* ----------- Helper ----------- */

bool IdentIsDecl(ASTNode* ident, ASTNode* parent)
{
    if (!parent) return false;

    return parent->type == FUNC_NODE || parent->type == DECL_STMT_NODE || parent->type == PARAM_NODE;
}

/* ----------- Name Resolution ---------- */

void ResolveNames(AST* ast) 
{
    printf("Resolving Names in Prog\n");
    ASTNode* root = ast->root;
    ResolveNamesInNode(root, NULL);
}

void ResolveNamesInNode(ASTNode* current, ASTNode* parent) {
    /* TODO: Have separate namespaces (ie typedef, etc) */

    if (current->type == PROG_NODE || current->type == BODY_NODE)
        BeginScope();

    if (current->type == IDENT_NODE && IdentIsDecl(current, parent)) {
        char* name = current->token.lex.word;
        if (LookupCurrentScope(name))       /* TODO: make decl_stmt and func have a token of their ident instead of having ident as a child */
            NERROR(name);

        printf("IDENT %s\n", name);
        STPush(current);
    }

    int i;
    for (i = 0; i < current->childCount; i++) 
        ResolveNamesInNode(current->children[i], current);
    
    if (current->type == PROG_NODE || current->type == BODY_NODE)
        ExitScope();
        
}

/* ----------- Symbol Table ---------- */

/*
Symbol* ResolveBinding(SymbolTable* st, Symbol* sym)
{
    /*
        Starts with innermost table, expanding search outwards.
        If duplicated names in same scope -> Name Resolution error
        If no decl stmt or function for Name -> Name Resolution error

    
}
*/

/*
    Have two structures: 
        One map, where the key is the actual ident (as a Symbol instead of a string).
        The symbol is used as the hash function and the value is that particular ident
        at that point in time.
            - For Example: int x in global scope would be the first member of that key
              Upon seeing a local X in a function (shadowing) the stack for that key is
              pushed to. The ne wvalue now contains the first latest declaration.
        
        The second being an "auxiliary stack" which shows in what order the symbols
        were "pushed" into the symbol table (map), this helps with scpoe. Pushing special
        markers onto the stack for begin scope and end scope 

    What is a binding (What should the table be filled with)?
        
*/