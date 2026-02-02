#ifndef _TYPE_CHECKER_H__
#define _TYPE_CHECKER_H__

#include "NameResolver.h"

/* ---------- Error Handling ----------- */

void TERROR_INCOMPATIBLE(Symbol* sym);

/* ---------- Type Structures  ----------- */

typedef enum {
    TYPE_INT, TYPE_CHAR, TYPE_DOUBLE, TYPE_FLOAT,
    TYPE_PTR, TYPE_FUNC, TYPE_STRUCT, TYPE_STRING,
    TYPE_ARR, TYPE_VOID
} TypeKind;


typedef struct TYPE {
    TypeKind kind;
    union {
        struct TYPE* array;
        struct {Symbol* sym; struct TYPE* type;} name;
    } u;
} TYPE;

typedef struct TYPE_LIST {
    TYPE* head;
    TYPE* tail;
} TYPE_LIST;

TYPE* TY_NULL(void);
TYPE* TY_INT(void);
TYPE* TY_STRING(void);
TYPE* TY_VOID(void);

TYPE* TY_ARR(TYPE* type);
TYPE* TY_NAME(Symbol* sym, TYPE* type);

TYPE_LIST TY_LIST(TYPE* head, TYPE_LIST* tail);

/* ---------- Symbol Table With Types ------------ */

/* For each ident
    If Variable -> what is its type
    If Fucntion -> What are its param and result types
*/

/* ----------- Type Environment ---------- */

typedef struct EnvironmentEntry {
    enum { ENV_VAR_ENTRY, ENV_FUNC_ENTRY } kind;
    union {
        struct { TYPE* ty; } var;
        struct { TYPE_LIST* formals; TYPE* result; } func;
    } u;
} EnvironmentEntry;

EnvironmentEntry ENV_VarEntry(TYPE* ty);
EnvironmentEntry ENV_FuncEntry(TYPE_LIST* formals, TYPE* result);

SymbolTable ENV_BaseTenv(); /* Base Type Environment (ie int -> TY_INT ... )*/
SymbolTable Env_BaseVenv(); /* Base Variable Environment */

/* ---------- Type Logic ----------- */

TYPE* TypeCheckExpr(ASTNode* node);
void  TypeCheckStmt(ASTNode* node, TYPE* expected);

void CheckTypes(Symbol** st);
void CheckSymbol(Symbol* sym);

/* 
    For declarations -  check that the value being assigned is compatible with the declaration type
    
    For Functions Calls
        -   Ensure the paramaters types are compatible with the argument types

    For Exprs - 
        Ensure the expression type is valid for the variable type

    For Arrays 
*/

#endif 