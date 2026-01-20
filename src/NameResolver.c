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

Symbol* InitSymbol(ASTNode* decl)
{
    /* TODO: Clean this up */

    Symbol* sym = malloc(sizeof(Symbol)); 
    sym->decl = decl;
    sym->name = FindIdent(decl);
    if (!sym->name) {
        printf("ERROR: No ident associated with ast node\n");
    }
    if (decl->type == DECL_STMT_NODE)
        sym->stype = S_VAR;
    else 
        sym->stype = S_FUNC;

    return sym;
}

bool SymbolHasOwnScope(Symbol* sym)
{
    if (sym->stype == S_FUNC)   /* Again a helper function/macro would be nice */
        return true;
    return false;
}

/* ----------- Symbol Table ---------- */

SymbolTable* InitST() 
{

}

void STPushChild (SymbolTable* st, SymbolTable* st2)
{

}

void STPushSymbol(SymbolTable* st, Symbol* sym)
{

}

SymbolTable* GenerateSymbolTable(ASTNode* node) 
{
    SymbolTable* st = InitST();

    int i;
    for (i = 0; i < node->childCount; i++)
    {
        if (IsValidSymbol(node->children[i])) {
            Symbol* sym = InitSymbol(node->children[i]); 
            if (SymbolHasOwnScope(sym))
                STPushChild(st, GenerateSymbolTable(sym->decl));
            else
                STPushSymbol(st, sym); 
        }

    }

}

Symbol* ResolveBinding(SymbolTable* st, Symbol* sym)
{
    /*
        Starts with innermost table, expanding search outwards.
        If duplicated names in same scope -> Name Resolution error
        If no decl stmt or function for Name -> Name Resolution error

    */
}

/*
    int scopeExample = 3;
    func () {
        // Not an error
        int scopeExmple = 5;  

        while(scopeExmaple)
    }


*/