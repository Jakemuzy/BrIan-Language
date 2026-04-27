#ifndef _BRIAN_NAMESPACE_H
#define _BRIAN_NAMESPACE_H

#include "ArenaAllocator.h"
#include "Environment.h"

/*        BrIan Namespaces
    ----------------------------
     Refer to docs/pipeline.md 
           for details.  

    Groupings of symbol tables.
        Can easily convert
      into a linked list if 
      user defined namespaces
        become desireable.

*/

/* ----- Namespaces ----- */

typedef struct Namespaces {
    Environment* ns[NS_COUNT];
} Namespaces;

Namespaces* InitalizeNamespaces(Arena* arena);
Environment* GetNamespace(Namespaces* nss, NamespaceKind nskind);

/* ----- Scopes ----- */

void EnterScope(Arena* arena, Namespaces* nss);
void ExitScope(Namespaces* nss);

#endif
