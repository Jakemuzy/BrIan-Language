#include "NameResolver.h"



/* ----------- Helper ----------- */

bool IsValidDecl(ASTNode* decl)
{
    NodeType type = decl->type;
    if (type == DECL_STMT_NODE || type == FUNC_NODE)
        return true;
    return false;
}

char* FindIdent(ASTNode* decl)
{
    int i;
    for (i = 0; i < decl->childCount; i++) {
        ASTNode* child = decl->children[i];
        if (child->type == IDENT)
            return child->token.lex.word;
    }
    return NULL;
}

/* ----------- Name Resolution ---------- */

void ResolveNames(AST* ast) 
{
    printf("Resolving Names in Prog\n");
    ASTNode* root = ast->root;
    BeginScope();   /* Global Scope */
    ResolveNamesInNode(root);
}

void ResolveNamesInNode(ASTNode* current) {
    /* TODO: Have separate namespaces (ie typedef, etc) */
    if (current->token.lex.word)
        printf("Resolving names in %s\n", current->token.lex.word);

    int i;
    for (i = 0; i < current->childCount; i++) {
        ASTNode* child = (current->children)[i];
        NodeType type = child->type;
        if (child->token.lex.word)
            printf("\t %s\n", child->token.lex.word);

        if (type == DECL_STMT_NODE) 
            STPush(child);
        else if (type == FUNC_NODE) {
            /* Get Body Node from Here */
            STPush(child);
            BeginScope();
        }

        ResolveNamesInNode(child);
        if (current->type == FUNC_NODE)
            ExitScope();
    }
    
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