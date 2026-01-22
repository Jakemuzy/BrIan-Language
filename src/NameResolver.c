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

/* ----------- Symbols  ---------- */


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