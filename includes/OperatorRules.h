#ifndef _OPERATOR_RULES_H__
#define _OPERATOR_RULES_H__

#include "Error.h"
#include "NameResolver.h"
#include "Type.h"

/* ----------- Valid Types ---------- */

typedef enum TypeCategory { C_NUMERIC, C_INTEGRAL, C_SIGNED, C_UNSIGNED, C_DECIMAL, C_BOOLEAN, C_POINTER, C_ANY } TypeCategory; 
size_t TypeSize(TypeKind kind);
bool TypeHasCategory(TypeKind kind, TypeCategory cat);
TypeCategory GetCategory(TYPE* type);
TYPE* DetermineIntType(TYPE* lhs, TYPE* rhs);

/* ----------- Lval Checking  ---------- */

void AssignTypes(ASTNode* typeNode, ASTNode* varNode); /* TODO: assigns symbols in Var_Node correct types */
TYPE* ValidLval(Namespaces* nss, ASTNode* identNode, NamespaceKind kind);
TYPE* ValidEquals(ASTNode* lhs, ASTNode* rhs, TokenType operator);

/* ---------- Table Driven Type Checking ---------- */

/* Binary */
TYPE* BoolType(TYPE* lhs, TYPE* rhs);
TYPE* ComparableTypes(TYPE* lhs, TYPE* rhs);
TYPE* IntegerPromotion(TYPE* lhs, TYPE* rhs);
TYPE* LvalPromotion(TYPE* lhs, TYPE* rhs);
TYPE* ImplicitCast(TYPE* lhs, TYPE* rhs); /* Warn */

/* Unary */
/* Placeholders since TypeResult takes two arguments */
TYPE* IncrementRule(TYPE* expr, TYPE* placeholder);
TYPE* NegateRule(TYPE* expr, TYPE* placeholder);
TYPE* BinaryNegateRule(TYPE* expr, TYPE* placeholder);
TYPE* BooleanRule(TYPE* expr, TYPE* placeholder);

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
    { PLUS, C_NUMERIC, C_NUMERIC, IntegerPromotion },     /* Function pointers for determining what output type should be */
    { MINUS, C_NUMERIC, C_NUMERIC, IntegerPromotion }, 
    { DIV, C_NUMERIC, C_NUMERIC, IntegerPromotion }, 
    { MULT, C_NUMERIC, C_NUMERIC, IntegerPromotion }, 
    { POW, C_NUMERIC, C_NUMERIC, IntegerPromotion }, 
    { MOD, C_INTEGRAL, C_INTEGRAL, IntegerPromotion }, 

    /* Bitwise Promotion is essentially integer promotion */
    { XOR, C_INTEGRAL, C_INTEGRAL,  IntegerPromotion }, 
    { OR, C_INTEGRAL, C_INTEGRAL,  IntegerPromotion }, 
    { AND, C_INTEGRAL, C_INTEGRAL,  IntegerPromotion }, 
    { LSHIFT, C_INTEGRAL, C_INTEGRAL,  IntegerPromotion }, 
    { RSHIFT, C_INTEGRAL, C_INTEGRAL,  IntegerPromotion }, 

    /* TODO: BoolType would need to check more than of the same type 
       since implicit casting is allowed
    */
    { EQQ, C_ANY, C_ANY, ComparableTypes },
    { NEQQ, C_ANY, C_ANY, ComparableTypes },
    { GEQQ, C_NUMERIC, C_NUMERIC, ComparableTypes },
    { LEQQ, C_NUMERIC, C_NUMERIC, ComparableTypes },
    { GREAT, C_BOOLEAN, C_BOOLEAN, ComparableTypes },
    { LESS, C_BOOLEAN, C_BOOLEAN, ComparableTypes },

    { ANDL, C_BOOLEAN, C_BOOLEAN, BoolType },
    { ORL, C_BOOLEAN, C_BOOLEAN, BoolType },

};
static const size_t BINARY_RULES_SIZE = sizeof(BINARY_RULES) / sizeof(BINARY_RULES[0]);

/* ---------------------------------------- */

static UnaryRule UNARY_RULES[] = {
    { PP, C_NUMERIC, IncrementRule },
    { SS, C_NUMERIC, IncrementRule },
    { MINUS, C_NUMERIC, NegateRule },
    { PLUS, C_NUMERIC, NegateRule },

    { NEG, C_INTEGRAL,  BinaryNegateRule }, 

    { NOT, C_BOOLEAN, BooleanRule },
};
static const size_t UNARY_RULES_SIZE = sizeof(UNARY_RULES) / sizeof(UNARY_RULES[0]);

/* ---------------------------------------- */

static LvalRule LVAL_RULES[] = {
    { EQ, C_ANY, C_ANY, ImplicitCast },
    { PEQ, C_ANY, C_ANY, ImplicitCast },
    { SEQ, C_ANY, C_ANY, ImplicitCast },
    { MEQ, C_ANY, C_ANY, ImplicitCast },
    { DEQ, C_ANY, C_ANY, ImplicitCast },
    { MODEQ, C_ANY, C_ANY, ImplicitCast },
    { ANDEQ, C_ANY, C_ANY, ImplicitCast },
    { OREQ, C_ANY, C_ANY, ImplicitCast },
    { ANDLEQ, C_ANY, C_ANY, ImplicitCast },
    { ORLEQ, C_ANY, C_ANY, ImplicitCast },
    { NEGEQ, C_ANY, C_ANY, ImplicitCast },
    { XOREQ, C_ANY, C_ANY, ImplicitCast },
    { RIGHTEQ, C_ANY, C_ANY, ImplicitCast },
    { LEFTEQ, C_ANY, C_ANY, ImplicitCast },

    /* ++ and -- are unary not assignemnt */
};
static const size_t LVAL_RULES_SIZE = sizeof(LVAL_RULES) / sizeof(LVAL_RULES[0]);

/* ---------- Error Handling ----------- */

char* NskindToStr(NamespaceKind kind);
char* OperatorToStr(TokenType type);

TYPE* TERROR_INCOMPATIBLE(OperatorRule rule, ASTNode* node);
TYPE* TERROR_NO_RULE(OperatorRule rule, ASTNode* node);
TYPE* TERROR_UNDEFINED(ASTNode* node);
TYPE* TERROR_CAST(TYPE* left, TYPE* right, ASTNode* expr);
TYPE* TERROR(char* msg, ASTNode* node, NamespaceKind kind);
void TWARN(char* msg);

#endif