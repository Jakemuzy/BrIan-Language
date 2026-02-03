#include "Symbol.h"

/* ---------- Symbols ---------- */

Symbol* InitSymbol(ASTNode* decl, Symbol* prev) 
{
    Symbol* sym = malloc(sizeof(Symbol));
    sym->decl = decl;
    sym->name = decl->token.lex.word;
    sym->stype = (decl->type == DECL_STMT_NODE) ? S_VAR : S_FUNC;   /* Determine Type Here */
    sym->prev = prev;

    return sym;
}

/* ---------- Symbol Table ---------- */

Symbol* STLookup(char* name)
{
    int index = HashStr(name) % SIZE;
    Symbol* sym, *syms = ST[index];
    for (sym=syms; sym; sym=sym->prev)
        if (0 == strcmp(sym->name, name)) return sym;

    return sym;
}

Symbol* STPush(ASTNode* key)
{
    char* name = key->token.lex.word;
    int index = HashStr(name) % SIZE;

    Symbol* sym, *syms = ST[index];
    sym = InitSymbol(key, syms);

    ST[index] = sym;
    return sym;
}

Symbol* STPop(char* name)
{
    int index = HashStr(name) % SIZE;
    Symbol* top = ST[index];
    if (!top)
        return NULL;
    
    ST[index] = top->prev;
    return top;
}

/* ---------- Scope ---------- */

void BeginScope(Scope** currentScope, ScopeType type)
{
    Scope* newScope = malloc(sizeof(Scope));
    newScope->prev = *currentScope;
    newScope->symCount = 0; 
    newScope->symbols = NULL;
    newScope->stype = type;

    *currentScope = newScope;
}

void ExitScope(Scope** currentScope)
{
    Scope* scope = *currentScope;

    size_t i;
    for (i = 0; i < scope->symCount; i++) {
        Symbol* sym = scope->symbols[i];
        STPop(sym->name);
    } 

    *currentScope = scope->prev;
}

void PushScope(Scope** currentScope, Symbol* sym) 
{
    Scope* scope = *currentScope;
    size_t symCount = scope->symCount;

    scope->symbols = realloc(scope->symbols, (symCount + 1) * sizeof(Symbol*));     /* Incrementing by one is inefficient, we can fix this later by making a vector DS */
    scope->symbols[symCount] = sym;

    scope->symCount++;
}

bool LookupCurrentScope(Scope** currentScope, char* name)
{
    Scope* scope = *currentScope;

    int i = 0;
    for (i = 0; i < scope->symCount; i++) {
        Symbol* sym = scope->symbols[i];
        if(0 == strcmp(name, sym->name)) 
            return true;
    }
    return false;
}