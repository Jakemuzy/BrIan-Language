#include "Symbol.h"

/* ---------- Symbols ---------- */

Symbol* InitSymbol(ASTNode* decl, Symbol* prev, TYPE* type) 
{
    Symbol* sym = malloc(sizeof(Symbol));
    sym->decl = decl;
    sym->name = decl->token.lex.word;
    sym->prev = prev;
    sym->type = type;
    switch(decl->type) {
        case(VAR_DECL_NODE): sym->stype = S_VAR; break;
        /* Check parent for type? */
        case(FUNC_NODE) : sym->stype = S_FUNC; break;
        case(ARR_INDEX_NODE) : sym->stype = S_INDEX; break;
        case(CALL_FUNC_NODE) : sym->stype = S_CALL; break;
        case(TYPEDEF_DECL_NODE) : sym->stype = S_TYPEDEF; break;
        case(STRUCT_DECL_NODE) : sym->stype = S_STRUCT; break;
        case(ENUM_DECL_NODE) : sym->stype = S_ENUM; break;

        /* Fall thorugh for speicifc Ctrl Scopes */
        case(IF_NODE): case(ELIF_NODE): case(ELSE_NODE):
        case(DO_WHILE_STMT_NODE): case(WHILE_STMT_NODE):
        case(SWITCH_STMT_NODE): case(FOR_STMT_NODE): sym->stype = S_CTRL; break;  

        default: sym->stype = S_ERROR; break;
    }

    return sym;
}

/* ---------- Symbol Table ---------- */

SymbolTable* STInit() 
{
    SymbolTable* env = malloc(sizeof(SymbolTable));
    env->buckets = calloc(INIT_SIZE, sizeof(Symbol*));
    env->maxSize = INIT_SIZE; env->currSize = INIT_SIZE;

    return env;
}

Symbol* STLookup(SymbolTable* env, char* name)
{
    int index = Hash(name, HASH_STR, env->maxSize);
    Symbol* sym, *syms = env->buckets[index];
    for (sym=syms; sym; sym=sym->prev)
        if (0 == strcmp(sym->name, name)) return sym;

    return sym;
}

Symbol* STPush(SymbolTable* env, ASTNode* key, TYPE* type)
{
    char* name = key->token.lex.word;
    int index = Hash(name, HASH_STR, env->maxSize);

    Symbol* sym, *syms = env->buckets[index];
    sym = InitSymbol(key, syms, type);

    env->buckets[index] = sym;
    return sym;
}

Symbol* STPop(SymbolTable* env, char* name)
{
    int index = Hash(name, HASH_STR, env->maxSize);
    Symbol* top = env->buckets[index];
    if (!top)
        return NULL;
    
    env->buckets[index] = top->prev;
    return top;
}
