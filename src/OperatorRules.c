#include "OperatorRules.h"

/* ----------- Valid Types ---------- */

bool TypeHasCategory(TypeKind kind, TypeCategory cat)
{
    switch (cat) {
        case C_NUMERIC:
            return kind == TYPE_INT || kind == TYPE_FLOAT || kind == TYPE_DOUBLE;
        case C_INTEGRAL:
            return kind == TYPE_INT;
        case C_DECIMAL:
            return kind == TYPE_DOUBLE || kind == TYPE_FLOAT;
        case C_EQUALITY:
            return 1;
        default:
            return 0;
    }
}

/* ----------- Lval Checking  ---------- */

TYPE* ValidLval(Namespaces* nss, ASTNode* identNode, NamespaceKind kind)
{
    /* TODO: Lookup based on namespace */
    char* name = identNode->token.lex.word;
    Symbol* sym = STLookupNamespace(nss, name, N_VAR);
    if (!sym) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Lval identifier '%s' is invalid", identNode->token.lex.word);
        return TERROR("Lval identifier '%s' is invalid", identNode, kind);
    }

    /* Can't be Func or typedef */
    /* TODO: looks like this crashes it */
    if (sym->stype == S_FUNC) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Lval identifier '%s' cannot be a function", identNode->token.lex.word);
        return TERROR(msg, identNode, kind);
    }
    
    /* TODO:
        ident's when resolved in the name resolver don't actually store a type 
        sometimes, its supposed to happen in the type checker, add an extra
        check for whether or not its set already ( != TY_NAT ) 
    */
    if (!sym->type) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Lval identifier '%s' is missing a type associated", identNode->token.lex.word);
        return TERROR(msg, identNode, kind);
    }
    return sym->type;
}

TYPE* ValidEquals(ASTNode* lhs, ASTNode* rhs, TokenType operator)
{

}

/* ---------- Table Driven Type Checking ---------- */

/* Binary */
TYPE* NumericPromotion(TYPE* lhs, TYPE* rhs)
{
    /* TODO: Warn on implicit converions */
    /* Always promotes to signed of the largest size */
    if (lhs->kind == TYPE_DOUBLE || rhs->kind == TYPE_DOUBLE)
        return TY_DOUBLE();
    if (lhs->kind == TYPE_FLOAT || rhs->kind == TYPE_FLOAT)
        return TY_FLOAT();

    if (lhs->kind == TYPE_I64 || rhs->kind == TYPE_I64)
        return TY_I64();
    if (lhs->kind == TYPE_INT || rhs->kind == TYPE_INT)
        return TY_INT();
    if (lhs->kind == TYPE_I32 || rhs->kind == TYPE_I32)
        return TY_I32();
    if (lhs->kind == TYPE_I16 || rhs->kind == TYPE_I16)
        return TY_I16();
    if (lhs->kind == TYPE_I8 || rhs->kind == TYPE_I8)
        return TY_I8();
    if (lhs->kind == TYPE_U64 || rhs->kind == TYPE_U64)
        return TY_U64();
    if (lhs->kind == TYPE_U32 || rhs->kind == TYPE_U32)
        return TY_U32();
    if (lhs->kind == TYPE_U16 || rhs->kind == TYPE_U16)
        return TY_U16();
    if (lhs->kind == TYPE_U8 || rhs->kind == TYPE_U8)
        return TY_U8();

    return TY_ERROR();
}

TYPE* BitwisePromotion(TYPE* lhs, TYPE* rhs) 
{
    /* TODO: Warn on implicit converions */
    /* Always promotes to signed of the largest size */
    if (lhs->kind == TYPE_I64 || rhs->kind == TYPE_I64)
        return TY_I64();
    if (lhs->kind == TYPE_INT || rhs->kind == TYPE_INT)
        return TY_INT();
    if (lhs->kind == TYPE_I32 || rhs->kind == TYPE_I32)
        return TY_I32();
    if (lhs->kind == TYPE_I16 || rhs->kind == TYPE_I16)
        return TY_I16();
    if (lhs->kind == TYPE_I8 || rhs->kind == TYPE_I8)
        return TY_I8();
    if (lhs->kind == TYPE_U64 || rhs->kind == TYPE_U64)
        return TY_U64();
    if (lhs->kind == TYPE_U32 || rhs->kind == TYPE_U32)
        return TY_U32();
    if (lhs->kind == TYPE_U16 || rhs->kind == TYPE_U16)
        return TY_U16();
    if (lhs->kind == TYPE_U8 || rhs->kind == TYPE_U8)
        return TY_U8();

    return TY_ERROR();
}

TYPE* BoolType(TYPE* lhs, TYPE* rhs)
{
    if (lhs->kind == rhs->kind)
        return TY_BOOL();

    return TY_ERROR();
}

/* Unary */
TYPE* BlankRule(TYPE* expr, TYPE* placeholder) { return expr; }

OperatorRule FindRule(TokenType ttype, RuleType rtype)
{  
    int i;
    OperatorRule rule;
    rule.rtype = rtype;
    if (rtype == BINARY_RULE) {
        for (i = 0; i < BINARY_RULES_SIZE; i++) {
            if (BINARY_RULES[i].op == ttype) {
                rule.rule.b = BINARY_RULES[i];
                return rule;
            }
        }
    }
    else if (rtype == UNARY_RULE) {
        for (i = 0; i < UNARY_RULES_SIZE; i++) {
            if (UNARY_RULES[i].op == ttype) {
                rule.rule.u = UNARY_RULES[i];
                return rule;
            }
        }
    }
    OperatorRule ERR; ERR.rtype == ERROR_RULE;
    return ERR; 
}