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
    union {     /* Array vs Variable */
        struct TYPE* array;     
        struct {Symbol* sym; struct TYPE* type;} name;  
    } u;
} TYPE;

typedef struct TYPE_LIST {  /* Especially useful for function paramater lists */
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

/* ----------- Environments ---------- */

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
SymbolTable Env_BaseVenv(); /* Base Variable Environment (TODO: will contain predefined functions)*/

/* ---------- Type Semantic Analysis ----------- */

TYPE*  TypeCheckVar(SymbolTable venv, SymbolTable tenv, ASTNode* var);
TYPE* TypeCheckExpr(SymbolTable venv, SymbolTable tenv, ASTNode* expr);
TYPE* TypeCheckDecl(SymbolTable venv, SymbolTable tenv, ASTNode* decl);
TYPE* TypeCheckType(                  SymbolTable tenv, ASTNode* expr);

#endif 