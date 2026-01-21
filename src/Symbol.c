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
    /*
    char* name = decl->token.lex.word;

    int index = HashStr(name) % SIZE;
    Symbol* sym, *syms = SymbolTable[index];
    for (sym=syms; sym; sym=sym->next)
        if (0 == strcmp(sym->name, name)) return sym;

    sym = InitSymbol(decl, syms);
    SymbolTable[index] = sym;
    return sym;
    */
}