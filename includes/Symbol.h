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

/* ---------- Forward Declaration Type Info ---------- */

typedef struct Symbol Symbol;

typedef struct TYPE TYPE;
typedef struct TYPE_LIST TYPE_LIST;
typedef struct TYPE_FIELD TYPE_FIELD;
typedef struct TYPE_FIELD_LIST TYPE_FIELD_LIST;

/* Constructors */
TYPE* TY_ERROR(void);
TYPE* TY_VOID(void);
TYPE* TY_INT(void);
TYPE* TY_FLOAT(void);
TYPE* TY_DOUBLE(void);
TYPE* TY_BOOL(void);
TYPE* TY_STRING(void);
TYPE* TY_NULL(void);

TYPE* TY_I8();
TYPE* TY_I16();
TYPE* TY_I32();
TYPE* TY_I64();

TYPE* TY_U8();
TYPE* TY_U16();
TYPE* TY_U32();
TYPE* TY_U64();

TYPE* TY_ARR(TYPE* element, int size);
TYPE* TY_NAME(Symbol* sym, TYPE* type);

/* ---------- Symbols ---------- */

typedef struct Namespace Namespace;

typedef enum SymbolType {   
    S_VAR, S_FUNC, S_INDEX, S_CALL, S_FIELD, S_TYPEDEF, S_STRUCT, S_ENUM, S_CTRL, S_ERROR
} SymbolType;

typedef struct Symbol {
    char* name; 
    ASTNode* decl;
    struct Symbol* prev;    

    /* Data Type and actual Symbol Type */
    TYPE* type;
    SymbolType stype;

    /* Lightweight namespace for paramaters / struct fields / etc */
    Namespace* fields;  
} Symbol;

Symbol* InitSymbol(ASTNode* decl, Symbol* prev, TYPE* type);
void    FreeSymbol(Symbol* sym);

/* ---------- Symbol Table ---------- */

#define BUCKET_COUNT_INIT 109

typedef struct SymbolTable {
    Symbol** buckets;

    size_t maxSize;
    size_t currSize;
} SymbolTable;

SymbolTable* STInit();
Symbol* STPop(SymbolTable* env, char* name);
Symbol* STLookup(SymbolTable* env, char* key);
Symbol* STPush(SymbolTable* env, ASTNode* key, TYPE* type);
void STResize(SymbolTable* env, unsigned int newSize);

#endif