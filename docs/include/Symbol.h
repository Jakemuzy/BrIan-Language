#ifndef _BRIAN_SYMBOL_TABLE_H_
#define _BRIAN_SYMBOL_TABLE_H_

#include "AST.h"

/*       BrIan Symbol Table
    ----------------------------
     Refer to docs/pipeline.md 
           for details.  

     Linked list of symbols for
        resolving multiple 
     declarations and detecting
        undefined varianbles.

*/

// Forward Declarations
typedef struct Type Type;


/* ----- Symbol ----- */

typedef enum SymbolType {
    S_VAR, S_FUNC, S_INDEX, S_CALL, S_FIELD, S_TYPEDEF, S_STRUCT, S_ENUM, S_CTRL, S_ERROR
} SymbolType;

typedef struct Symbol {
    char* name;
    ASTNode* current, prev;

    SymbolType stype;
    Type* type;
} Symbol;

/* ----- Symbol Table ----- */

#define SYMBOL_BUCKET_COUNT_INIT 109

typedef struct SymbolTable {
    Symbol** buckets;
    
    size_t maxSize;
    size_t currSize;
} SymboLTable;

SymbolTable* STInit(void);
Symbol* STPop(SymbolTable* env, char* name);
Symbol* STLookup(SymbolTable* env, char* key);
Symbol* STPush(SymbolTable* env, ASTNode* key, SymbolType stype);
void STResize(SymbolTable* env, unsigned int newSize);

#endif 