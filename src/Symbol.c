#include "Symbol.h"

/* ---------- Symbols ---------- */

Symbol* InitSymbol(ASTNode* decl, Symbol* next) 
{
    Symbol* sym = malloc(sizeof(Symbol));
    sym->decl = decl;
    sym->name = decl->token.lex.word;
    sym->stype = (decl->type == DECL_STMT_NODE) ? S_VAR : S_FUNC;
    sym->next = next;
}


/* ---------- Symbol Table ---------- */

Symbol* STLookup(ASTNode* decl)
{
    char* name = decl->token.lex.word;

    int index = HashStr(name) % SIZE;
    Symbol* sym, *syms = SymbolTable[index];
    for (sym=syms; sym; sym=sym->next)
        if (0 == strcmp(sym->name, name)) return sym;

    return sym;
}

void STPush(ASTNode* key)
{
    char name = key->token.lex.word;

    int index = HashStr(name) % SIZE;
    Symbol* sym, *syms = SymbolTable[index];

    sym = InitSymbol(key, syms);
    SymbolTable[index] = sym;

    if (sym->stype == FUNC_NODE)
        STScopeEnter(SymbolTable, key);
}

/* ---------- Scope Logic ---------- */


void STScopeEnter(Symbol* ST[], ASTNode* key)
{
    /* Vry inefficient to inc by 1, but good enough for testing */
    SCOPE.symbols = realloc(SCOPE.symbols, ++SCOPE.symCount * sizeof(Symbol*));
    SCOPE.symbols[SCOPE.symCount] = STLookup(key);
}

void STScopeExit(Symbol* ST[])
{

}