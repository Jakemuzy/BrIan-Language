#ifndef _SYMBOL_H__
#define _SYMBOL_H__

#include <stdarg.h>

#include "Dict.h" 
#include "Parser.h"

/* 
    TODO:
        - One symbol table per namespace (create name spaces / environment )
        - Have SymbolType assign a type to each identifier, that way
          type checker can easily decipher the type
        - Make SymbolTable ues Dictionary for dynamic sizing
*/

/* ---------- Forward Declarations ---------- */

struct TYPE;
struct Namespaces;

/* ---------- Symbols ---------- */

typedef enum SymbolType {   
    S_VAR, S_FUNC, S_INDEX, S_CALL, S_FIELD, S_TYPEDEF, S_STRUCT, S_ENUM, S_CTRL, S_ERROR
} SymbolType;

typedef struct Symbol {
    char* name; 
    ASTNode* decl;
    struct Symbol* prev;    

    /* Data Type and actual Symbol Type */
    struct TYPE* type;
    SymbolType stype;

    /* Lightweight namespace for paramaters / struct fields / etc */
    struct Namespaces* fields;  
    size_t fieldCount;
} Symbol;

Symbol* InitSymbol(ASTNode* decl, Symbol* prev, SymbolType stype);
void    FreeSymbol(Symbol* sym);

/* ---------- Symbol Table ---------- */

#define BUCKET_COUNT_INIT 109

typedef struct SymbolTable {
    Symbol** buckets;

    size_t maxSize;
    size_t currSize;
} SymbolTable;

SymbolTable* STInit(void);
Symbol* STPop(SymbolTable* env, char* name);
Symbol* STLookup(SymbolTable* env, char* key);
Symbol* STPush(SymbolTable* env, ASTNode* key, SymbolType stype);
void STResize(SymbolTable* env, unsigned int newSize);

#endif
