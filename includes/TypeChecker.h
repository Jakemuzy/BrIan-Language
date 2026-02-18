#ifndef _TYPE_CHECKER_H__
#define _TYPE_CHECKER_H__

#include <stdbool.h>

#include "Dict.h"
#include "NameResolver.h"

/* 
    TODO: 
        Disallow divide by 0
        Cap Integers to max and min
        Environment will need to track more if safety is guaranteed (ownership, lifetime, mutability, etc)
        Implement closures (For lambda functions)
*/

/* ---------- Type Structures  ----------- */

typedef enum TypeKind {
    TYPE_INT, TYPE_BOOL, TYPE_DOUBLE, TYPE_FLOAT,   /* TODO: could probably make int just implicity I32 */
    TYPE_PTR, TYPE_FUNC, TYPE_STRUCT, TYPE_STRING,
    TYPE_ARR, TYPE_VOID, TYPE_ERROR, TYPE_NULL,
    TYPE_NAME,

    TYPE_I8, TYPE_I16, TYPE_I32, TYPE_I64,
    TYPE_U8, TYPE_U16, TYPE_U32, TYPE_U64
} TypeKind;

typedef struct TYPE {
    TypeKind kind;
    union {     /* kind == TYPE_ARR -> u.array | kind == OTHER -> u.name */
        struct { TYPE* element; size_t size; } array;     
        struct {Symbol* sym; struct TYPE* type;} name;  
    } u;
} TYPE;

typedef struct TYPE_LIST {  /* Especially useful for function paramater lists */
    TYPE* head;
    struct TYPE_LIST* tail;
} TYPE_LIST;

typedef struct TYPE_FIELD { /* Struct Fields */
    Symbol* sym;
    TYPE* type;
} TYPE_FIELD;

typedef struct TYPE_FIELD_LIST {
    TYPE_FIELD* head;
    struct TYPE_FIELD_LIST* tail;
} TYPE_FIELD_LIST;


/* Other types already declared in Symbol.h */
TYPE_LIST TY_LIST(TYPE* head, TYPE_LIST* tail);
TYPE_FIELD* TY_FIELD(Symbol* sym, TYPE* type);
TYPE_FIELD_LIST* TY_FIELD_LIST(TYPE_FIELD* head, TYPE_FIELD_LIST* tail);

/* TODO: Implement structs as fields */

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

/* TODO: Need to use my own map of linked lists that store void* */
Dict ENV_BaseTenv(); /* Base Type Environment (ie int -> TY_INT ... )*/
Dict Env_BaseVenv(); /* Base Variable Environment (TODO: will contain predefined functions)*/

/* ---------- Type Semantic Analysis ----------- */

TYPE* TypeCheck(Namespaces* nss, ASTNode* expr);

TYPE* TypeCheckBinExpr(Namespaces* nss, ASTNode* expr);  
TYPE* TypeCheckUnaExpr(Namespaces* nss, ASTNode* expr);  
TYPE* TypeCheckAsgn(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckVar(Namespaces* nss, ASTNode* var);
TYPE* TypeCheckArrDecl(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckArrInit(Namespaces* nss, ASTNode* decl);
TYPE* TypeCheckArrIndex(Namespaces* nss, ASTNode* decl);
TYPE* TypeCheckCallFunc(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckTypedef(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckEnumBody(Namespaces* nss, ASTNode* expr);

/* ----------- Valid Types ---------- */

typedef enum TypeCategory { C_NUMERIC, C_INTEGRAL, C_DECIMAL, C_COMPARABLE, C_EQUALITY } TypeCategory; 
bool TypeHasCategory(TypeKind kind, TypeCategory cat);

/* ---------- Table Driven Type Checking ---------- */

TYPE* NumericPromotion(TYPE* lhs, TYPE* rhs);  /* Automatic type conversions based on "largest" */
TYPE* BoolType(TYPE* lhs, TYPE* rhs);

/* Placeholders since TypeResult takes two arguments */
TYPE* BlankRule(TYPE* epxr, TYPE* placeholder); /* Just returns type */

typedef enum RuleType { BINARY_RULE, UNARY_RULE, ERROR_RULE } RuleType;

typedef TYPE* (*TypeResult)(TYPE* lhs, TYPE* rhs);
typedef struct BinaryRule {
    TokenType op;
    TypeCategory left, right;
    TypeResult result;

    bool orderMatters;
} BinaryRule;

typedef struct UnaryRule {
    TokenType op;
    TypeCategory cat;
    TypeResult result;
} UnaryRule;

typedef struct OperatorRule { 
    RuleType rtype;
    union { BinaryRule b; UnaryRule u; } rule; 
} OperatorRule;

OperatorRule FindRule(TokenType ttype, RuleType rtype);
static BinaryRule BINARY_RULES[] = {    /* Maybe make this a map */
    { PLUS, C_NUMERIC, C_NUMERIC, NumericPromotion },     /* Function pointers for determining what output type should be */
    { MINUS, C_NUMERIC, C_NUMERIC, NumericPromotion }, 
    { EQQ, C_EQUALITY, C_EQUALITY, BoolType },
    // ... 
    { PEQ, C_EQUALITY, C_EQUALITY, NumericPromotion }
};
static const size_t BINARY_RULES_SIZE = sizeof(BINARY_RULES) / sizeof(BINARY_RULES[0]);

static UnaryRule UNARY_RULES[] = {
    { PP, C_NUMERIC, BlankRule }
};
static const size_t UNARY_RULES_SIZE = sizeof(UNARY_RULES) / sizeof(UNARY_RULES[0]);

/* ---------- Error Handling ----------- */

TYPE* TERROR_INCOMPATIBLE(OperatorRule rule, ASTNode* node);
TYPE* TERROR_NO_RULE(OperatorRule rule, ASTNode* node);

#endif 