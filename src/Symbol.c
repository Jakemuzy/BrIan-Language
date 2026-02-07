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

SymbolTable* STInit() 
{
    SymbolTable* ST = malloc(sizeof(SymbolTable));
    ST->buckets = malloc(sizeof(Symbol*) * INIT_SIZE);
    ST->maxSize = INIT_SIZE; ST->currSize = INIT_SIZE;
    ST->currentScope = NULL;

    return ST;
}

Symbol* STLookup(SymbolTable* ST, char* name)
{
    int index = Hash(name, HASH_STR, ST->maxSize);
    Symbol* sym, *syms = ST->buckets[index];
    for (sym=syms; sym; sym=sym->prev)
        if (0 == strcmp(sym->name, name)) return sym;

    return sym;
}

Symbol* STPush(SymbolTable* ST, ASTNode* key)
{
    char* name = key->token.lex.word;
    int index = Hash(name, HASH_STR, ST->maxSize);

    Symbol* sym, *syms = ST->buckets[index];
    sym = InitSymbol(key, syms);

    ST->buckets[index] = sym;
    return sym;
}

Symbol* STPop(SymbolTable* ST, char* name)
{
    int index = Hash(name, HASH_STR, ST->maxSize);
    Symbol* top = ST->buckets[index];
    if (!top)
        return NULL;
    
    ST->buckets[index] = top->prev;
    return top;
}

/* ---------- Scope ---------- */

void BeginScope(SymbolTable* ST, ScopeType type)
{
    Scope* newScope = malloc(sizeof(Scope));
    newScope->prev = ST->currentScope;
    newScope->symCount = 0; 
    newScope->symbols = NULL;
    newScope->stype = type;

    ST->currentScope = newScope;
}

void ExitScope(SymbolTable* ST)
{
    Scope* scope = ST->currentScope;

    size_t i;
    for (i = 0; i < scope->symCount; i++) {
        Symbol* sym = scope->symbols[i];
        STPop(ST, sym->name);
    } 

    ST->currentScope = scope->prev;
}

void PushScope(SymbolTable* ST, Symbol* sym) 
{
    Scope* scope = ST->currentScope;
    size_t symCount = scope->symCount;

    scope->symbols = realloc(scope->symbols, (symCount + 1) * sizeof(Symbol*));     /* Incrementing by one is inefficient, we can fix this later by making a vector DS */
    scope->symbols[symCount] = sym;

    scope->symCount++;
}

bool LookupCurrentScope(SymbolTable* ST, char* name)
{
    Scope* scope = ST->currentScope;

    int i = 0;
    for (i = 0; i < scope->symCount; i++) {
        Symbol* sym = scope->symbols[i];
        if(0 == strcmp(name, sym->name)) 
            return true;
    }
    return false;
}