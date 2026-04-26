#ifndef _BRIAN_NAME_RESOLVER_H_
#define _BRIAN_NAME_RESOLVER_H_

#include <stdbool.h>

#include "Symbol.h"

/*      BrIan Name Resolution
    ----------------------------
     Refer to docs/pipeline.md 
           for details.  
           
*/

/* ----- Context ----- */

typedef struct NameResolverContext {
    AST* ast;
    Arena* arena;
    
    // Failure will be recovered by assuming a reasonable in the mean time
    bool failure;   
} NameResolverContext;

NameResolverContext* InitalizeNameResolverContext(AST* ast);
void DestroyNameResolverContext(NameResolverContext* ctx);

#endif 