#ifndef _BRIAN_OPERATOR_RULES_H_
#define _BRIAN_OPERATOR_RULES_H_

#include "Type.h"

/*      BrIan Operator Rules
    ----------------------------
     Refer to docs/pipeline.md 
           for details.  

       All the type checking
    rules that BrIan uses during
      its type checking phase.

*/

/* ----- Type Categories ----- */

typedef enum TypeCategory {
    C_NUMERIC, C_INTEGRAL, C_SIGNED, C_UNSIGNED,
    C_DECIMAL, C_BOOLEAN,  C_TRUTHY, C_POINTER, 
    C_ANY
} TypeCategory;

/* Binary */
TYPE* NumericPromotion(TYPE* lhs, TYPE* rhs);
TYPE* SignedPromotion(TYPE* lhs, TYPE* rhs);
TYPE* TruthyEvaluation(TYPE* lhs, TYPE* rhs);
TYPE* BooleanEvaluation(TYPE* lhs, TYPE* rhs);
TYPE* ComparisonEvaluation(TYPE* lhs, TYPE* rhs);   /* Might be better as NumericPromotion */

/* Unary */
TYPE* UnaryBitwiseEvaluation(TYPE* lhs, TYPE* placeholder);
TYPE* UnaryLogicalEvaluation(TYPE* lhs, TYPE* placeholder);

/* ----- Type Structures ----- */

typedef TYPE* (*TypeResult)(TYPE* lhs, TYPE* rhs);

// Ternary Rule

typedef struct BinaryRule {
    TokenType op;
    TypeCategory left, right;
    TypeResult result;

    bool orderMatters;
} BinaryRule;

typedef struct UnaryRule {
    TokenType op;
    TypeCategory left;
    TypeResult result;

    bool orderMatters;
} UnaryRule;

typedef struct LvalRule {
    TokenType op;
    TypeCategory left, right;
    TypeResult result;
} LvalRule;

/* ----- Actual Rule Tables ----- */

BinaryRule FindBinaryRule(TokenType ttype);
UnaryRule FindUnaryRule(TokenType ttype);
LvalRule FindLvalRule(TokenType ttype);

static BinaryRule BINARY_RULES[] = {
    { PLUS, C_NUMERIC, C_NUMERIC, NumericPromotion, false },
    { MINUS, C_NUMERIC, C_NUMERIC, NumericPromotion, false }, 
    { DIV, C_NUMERIC, C_NUMERIC, NumericPromotion, true }, 
    { MULT, C_NUMERIC, C_NUMERIC, NumericPromotion, false }, 
    { POW, C_NUMERIC, C_NUMERIC, NumericPromotion, true }, 
    { MOD, C_INTEGRAL, C_INTEGRAL, NumericPromotion, true }, 

    { XOR, C_INTEGRAL, C_INTEGRAL,  NumericPromotion, false }, 
    { OR, C_INTEGRAL, C_INTEGRAL,  NumericPromotion, false }, 
    { AND, C_INTEGRAL, C_INTEGRAL,  NumericPromotion, false }, 
    { LSHIFT, C_INTEGRAL, C_INTEGRAL,  NumericPromotion, false }, 
    { RSHIFT, C_INTEGRAL, C_INTEGRAL,  NumericPromotion, false }, 

    { EQQ, C_TRUTHY, C_TRUTHY, TruthyEvaluation, false },
    { NEQQ, C_TRUTHY, C_TRUTHY, TruthyEvaluation, false },
    { GEQQ, C_NUMERIC, C_NUMERIC, ComparisonEvaluation, false },
    { LEQQ, C_NUMERIC, C_NUMERIC, ComparisonEvaluation, false },
    { GREAT, C_BOOLEAN, C_BOOLEAN, ComparisonEvaluation, false },
    { LESS, C_BOOLEAN, C_BOOLEAN, ComparisonEvaluation, false },

    { ANDL, C_BOOLEAN, C_BOOLEAN, BooleanEvaluation, false },
    { ORL, C_BOOLEAN, C_BOOLEAN, BooleanEvaluation, false }
 
};

#endif
