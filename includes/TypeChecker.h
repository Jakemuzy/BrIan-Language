#ifndef _TYPE_CHECKER_H__
#define _TYPE_CHECKER_H__

#include <stdbool.h>

#include "NameResolver.h"

/* 
    TODO: 
        Disallow divide by 0
        Cap Integers to max and min
*/

/* ---------- Error Handling ----------- */

void TERROR_INCOMPATIBLE(Symbol* sym);

/* ---------- Type Structures  ----------- */

typedef enum {
    TYPE_INT, TYPE_BOOL, TYPE_DOUBLE, TYPE_FLOAT,
    TYPE_PTR, TYPE_FUNC, TYPE_STRUCT, TYPE_STRING,
    TYPE_ARR, TYPE_VOID,

    TYPE_I8, TYPE_I16, TYPE_I32, TYPE_I64,
    TYPE_U8, TYPE_U16, TYPE_U32, TYPE_U64
} TypeKind;


typedef struct TYPE {
    TypeKind kind;
    union {     /* kind == TYPE_ARR -> u.array | kind == OTHER -> u.name */
        struct TYPE* array;     
        struct {Symbol* sym; struct TYPE* type;} name;  
    } u;
} TYPE;

typedef struct TYPE_LIST {  /* Especially useful for function paramater lists */
    TYPE* head;
    struct TYPE_LIST* tail;
} TYPE_LIST;

TYPE* TY_NULL();
TYPE* TY_INT();
TYPE* TY_FLOAT();
TYPE* TY_DOUBLE();
TYPE* TY_BOOL();
TYPE* TY_STRING();

TYPE* TY_I8();
TYPE* TY_I16();
TYPE* TY_I32();
TYPE* TY_I64();

TYPE* TY_U8();
TYPE* TY_U16();
TYPE* TY_U32();
TYPE* TY_U64();

TYPE* TY_VOID();
TYPE* TY_ERROR();

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

TYPE* TypeCheckBinaryExpr(SymbolTable venv, SymbolTable tenv, ASTNode* expr);   /* Helpers */

/* ----------- Valid Types ---------- */

typedef enum TypeCategory { C_NUMERIC, C_INTEGRAL, C_DECIMAL, C_COMPARABLE, C_EQUALITY } TypeCategory; 
bool TypeHasCategory(TypeKind kind, TypeCategory cat);

/* ---------- Table Driven Type Checking ---------- */

TYPE* NumericPromotion(TYPE* lhs, TYPE* rhs);  /* Automatic type conversions based on "largest" */
TYPE* BoolType(TYPE* lhs, TYPE* rhs);

typedef TYPE* (*TypeResult)(TYPE* lhs, TYPE* rhs);
typedef struct BinaryRule {
    TokenType op;
    TypeCategory left, right;
    TypeResult result;

    bool orderMatters;
} BinaryRule;

typedef struct UnaryRule {

} UnaryRule;

static BinaryRule BINARY_RULES[] = {
    { PLUS, C_NUMERIC, C_NUMERIC, NumericPromotion },     /* Function pointers for determining what output type should be */
    { MINUS, C_NUMERIC, C_NUMERIC, NumericPromotion }, 
    { EQQ, C_EQUALITY, C_EQUALITY, BoolType },
} 

#endif 