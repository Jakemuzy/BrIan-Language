#ifndef _OPERATOR_RULES_H__
#define _OPERATOR_RULES_H__

#include "NameResolver.h"
#include "Type.h"

/* ----------- Valid Types ---------- */

typedef enum TypeCategory { C_NUMERIC, C_INTEGRAL, C_DECIMAL, C_COMPARABLE, C_EQUALITY, C_ANY } TypeCategory; 
bool TypeHasCategory(TypeKind kind, TypeCategory cat);

/* ----------- Lval Checking  ---------- */

void AssignTypes(ASTNode* typeNode, ASTNode* varNode); /* TODO: assigns symbols in Var_Node correct types */
TYPE* ValidLval(Namespaces* nss, ASTNode* identNode, NamespaceKind kind);
TYPE* ValidEquals(ASTNode* lhs, ASTNode* rhs, TokenType operator);

/* ---------- Table Driven Type Checking ---------- */

TYPE* NumericPromotion(TYPE* lhs, TYPE* rhs);  /* Automatic type conversions based on "largest" */
TYPE* BitwisePromotion(TYPE* lhs, TYPE* rhs);  
TYPE* EqPromotion(TYPE* lhs, TYPE* rhs);
TYPE* BoolType(TYPE* lhs, TYPE* rhs);
TYPE* ImplicitCast(TYPE* lhs, TYPE* rhs); /* Warn */

/* Placeholders since TypeResult takes two arguments */
TYPE* BlankRule(TYPE* epxr, TYPE* placeholder); /* Just returns type */

typedef enum RuleType { BINARY_RULE, UNARY_RULE, LVAL_RULE, ERROR_RULE } RuleType;

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

typedef struct LvalRule {
    TokenType op;
    TypeCategory left, right;
    TypeResult result;
} LvalRule;

typedef struct OperatorRule { 
    RuleType rtype;
    union { BinaryRule b; UnaryRule u; LvalRule l; } rule; 
} OperatorRule;

/* ---------- Actual Rule Tables ---------- */

OperatorRule FindRule(TokenType ttype, RuleType rtype);
static BinaryRule BINARY_RULES[] = {    /* Maybe make this a map */
    /* TOOD: Anything depending on lval cannot go in the table */
    { PLUS, C_NUMERIC, C_NUMERIC, NumericPromotion },     /* Function pointers for determining what output type should be */
    { MINUS, C_NUMERIC, C_NUMERIC, NumericPromotion }, 
    { DIV, C_NUMERIC, C_NUMERIC, NumericPromotion }, 
    { MULT, C_NUMERIC, C_NUMERIC, NumericPromotion }, 
    { POW, C_NUMERIC, C_NUMERIC, NumericPromotion }, 
    { MOD, C_INTEGRAL, C_INTEGRAL, NumericPromotion }, 

    /* TODO: CHECK w of lhs and rhs, warn on diff sizes */
    { XOR, C_INTEGRAL, C_INTEGRAL,  BitwisePromotion }, 
    { OR, C_INTEGRAL, C_INTEGRAL,  BitwisePromotion }, 
    { AND, C_INTEGRAL, C_INTEGRAL,  BitwisePromotion }, 
    { LSHIFT, C_INTEGRAL, C_INTEGRAL,  BitwisePromotion }, 
    { RSHIFT, C_INTEGRAL, C_INTEGRAL,  BitwisePromotion }, 

    { EQQ, C_EQUALITY, C_EQUALITY, BoolType },
    { NEQQ, C_EQUALITY, C_EQUALITY, BoolType },
    { GEQQ, C_EQUALITY, C_EQUALITY, BoolType },
    { LEQQ, C_EQUALITY, C_EQUALITY, BoolType },

    { ANDL, C_EQUALITY, C_EQUALITY, BoolType },
    { ORL, C_EQUALITY, C_EQUALITY, BoolType },
    { GREAT, C_EQUALITY, C_EQUALITY, BoolType },
    { LESS, C_EQUALITY, C_EQUALITY, BoolType },

    //{ EQ, C_ANY, C_ANY, EqPromotion },    /* Will be handled directly, since lval dependent */
    //{ PEQ, C_EQUALITY, C_EQUALITY, NumericEqPromotion }
};
static const size_t BINARY_RULES_SIZE = sizeof(BINARY_RULES) / sizeof(BINARY_RULES[0]);

/* ---------------------------------------- */

static UnaryRule UNARY_RULES[] = {
    { PP, C_NUMERIC, BlankRule },

    { NEG, C_INTEGRAL,  BitwisePromotion }, 

    { NOT, C_EQUALITY, BoolType },
};
static const size_t UNARY_RULES_SIZE = sizeof(UNARY_RULES) / sizeof(UNARY_RULES[0]);

/* ---------------------------------------- */

static LvalRule LVAL_RULES[] = {
    { EQ, C_ANY, C_ANY, ImplicitCast }
};
static const size_t LVAL_RULES_SIZE = sizeof(LVAL_RULES) / sizeof(LVAL_RULES[0]);

/* ---------- Error Handling ----------- */

char* NskindToStr(NamespaceKind kind);
char* OperatorToStr(TokenType type);

TYPE* TERROR_INCOMPATIBLE(OperatorRule rule, ASTNode* node);
TYPE* TERROR_NO_RULE(OperatorRule rule, ASTNode* node);
TYPE* TERROR_UNDEFINED(ASTNode* node);
TYPE* TERROR(char* msg, ASTNode* node, NamespaceKind kind);
void TWARN(char* msg);

#endif