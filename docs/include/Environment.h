#ifndef _BRIAN_SYMBOL_TABLE_H_
#define _BRIAN_SYMBOL_TABLE_H_

#include "ErrorHandler.h"
#include "ArenaAllocator.h"
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
struct Type;

/* ----- Symbol ----- */

typedef enum SymbolType {
    S_VAR, S_FUNC, S_INDEX, S_CALL, S_FIELD, S_TYPEDEF, S_STRUCT, S_ENUM, S_CTRL, S_ERROR
} SymbolType;

typedef struct Symbol {
    char* name;
    ASTNode* node;

    SymbolType stype;
    struct Type* type;
} Symbol;

int SymbolHash(char* key);

static Symbol* POISON_SYM = (Symbol*){"<poison>", NULL, S_ERROR, NULL};

/* ----- Symbol Table ----- */

#define ENV_INIT_SIZE 109
#define NS_COUNT 2  // 2 for now, only VAR and TYPE implemented

typedef enum NamespaceKind {
    N_VAR, N_TYPE, N_NAMESPACE, N_MACRO, N_LIFETIME, N_LABEL 
} NamespaceKind;

typedef struct Environment {
    Symbol** buckets;
    
    size_t maxSize;
    size_t currSize;

    // Scoping 
    struct Environment* prev;
    NamespaceKind nskind;
} Environment;

#define SYM_ALREADY_EXISTS NULL
#define SYM_DOESNT_EXIST NULL

Environment* InitalizeEnvironment(Arena* arena, NamespaceKind nskind);
Symbol* LookupEnvironment(Environment* env, char* key);
Symbol* LookupEnvironmentCurrentScope(Environment* env, char* key) ;
Symbol* PushEnvironment(Arena* arena, Environment* env, ASTNode* key, SymbolType stype);
void DestroyEnvironment(Environment* env);

void ResizeEnvironment(Environment* env, unsigned int newSize);

#endif 
