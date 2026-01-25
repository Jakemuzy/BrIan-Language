#include "Symbol.h"

/* ---------- Symbols ---------- */

Symbol* InitSymbol(ASTNode* decl, Symbol* prev) 
{
    Symbol* sym = malloc(sizeof(Symbol));
    sym->decl = decl;
    sym->name = decl->token.lex.word;
    sym->stype = (decl->type == DECL_STMT_NODE) ? S_VAR : S_FUNC;
    sym->prev = prev;

    return sym;
}

/* ---------- Symbol Table ---------- */

Symbol* STLookup(char* name)
{
    int index = HashStr(name) % SIZE;
    Symbol* sym, *syms = SymbolTable[index];
    for (sym=syms; sym; sym=sym->prev)
        if (0 == strcmp(sym->name, name)) return sym;

    return sym;
}

void STPush(ASTNode* key)
{
    char* name = key->token.lex.word;
    int index = HashStr(name) % SIZE;

    Symbol* sym, *syms = SymbolTable[index];
    sym = InitSymbol(key, syms);

    SymbolTable[index] = sym;
    PushScope(sym);
}

Symbol* STPop(char* name)
{
    int index = HashStr(name) % SIZE;
    Symbol* top = SymbolTable[index];
    if (top)
        SymbolTable[index] = top->prev;
}

/* ---------- Scope ---------- */

void BeginScope()
{
    printf("Entering Scope \n");
    Scope* newScope = malloc(sizeof(Scope));
    newScope->prev = CurrentScope;
    newScope->symCount = 0; 
    newScope->symbols = NULL;

    CurrentScope = newScope;
}

void ExitScope()
{
    Scope* scope = CurrentScope;

    size_t i;
    for (i = 0; i < scope->symCount; i++) {
        Symbol* sym = scope->symbols[i];
        STPop(sym->name);
    } 

    CurrentScope = CurrentScope->prev;
    free(scope->symbols);       /* TODO: Actual symbols not freed yet */
    free(scope);
}

void PushScope(Symbol* sym) 
{
    Scope* scope = CurrentScope;
    size_t symCount = scope->symCount;

    scope->symbols = realloc(scope->symbols, (symCount + 1) * sizeof(Symbol*));     /* Incrementing by one is inefficient, we can fix this later by making a vector DS */
    scope->symbols[symCount] = sym;

    printf("\t %s\n", sym->name);
    scope->symCount++;
}

bool LookupCurrentScope(char* name) {
    
    int i = 0;
    for (i = 0; i < CurrentScope->symCount; i++) {
        Symbol* sym = CurrentScope->symbols[i];
        if(0 == strcmp(name, sym->name)) 
            return true;
    }
    return false;
}