#ifndef _BRIAN_TYPE_H_
#define _BRIAN_TYPE_H_

#include "Environment.h"

/*          BrIan Types
    ----------------------------
       Refer to docs/types.md 
           for details.  

*/

typedef enum TypeKind {
    TYPE_BOOL, TYPE_DOUBLE, TYPE_FLOAT, TYPE_PTR,
    TYPE_FUNC, TYPE_STRING, TYPE_ARR,  TYPE_VOID, 
    TYPE_ERROR, TYPE_NULL, TYPE_NAME,

    TYPE_I8, TYPE_I16, TYPE_I32, TYPE_I64,
    TYPE_U8, TYPE_U16, TYPE_U32, TYPE_U64,

    TYPE_MUTEX, TYPE_SEMAPHORE, TYPE_TASK,
    TYPE_MATRIX, TYPE_VECTOR,

    TYPE_STRUCT, TYPE_ENUM, TYPE_FUNC_PTR, 

    TYPE_NAT
} TypeKind;

typedef struct TYPE {
    TypeKind kind;

    union {
        struct { struct TYPE* element; size_t size; } array; 
        struct { Symbol* sym; struct TYPE* type; } name;
        struct { Symbol* sym; struct TYPE_FIELD_LIST* fields; } struc;
    } Specific;
} TYPE;

static TYPE TY_ERR = { .kind = TYPE_NAT };
static TYPE* TY_ERROR = &TY_ERR;

/* ----- Helpers ----- */

TYPE* ExistsType(ASTNode* node);
TYPE* CreateNewType(ASTNode* node);

TYPE* LiteralToType(TokenType);


#endif
