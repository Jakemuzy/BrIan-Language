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
            return C_ANY;
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
        return TERROR(msg, identNode, kind);
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

TYPE* IntegerPromotion(TYPE* lhs, TYPE* rhs)
{
    TypeKind lkind = lhs->kind, rkind = rhs->kind;

    if (lkind == TYPE_I64 || rkind == TYPE_I64) return TY_I64();
    if (lkind == TYPE_U64 || rkind == TYPE_U64) return TY_U64();
    if (lkind == TYPE_I32 || rkind == TYPE_I32) return TY_I32();
    if (lkind == TYPE_INT || rkind == TYPE_INT) return TY_INT();
    if (lkind == TYPE_U32 || rkind == TYPE_U32) return TY_U32();
    if (lkind == TYPE_I16 || rkind == TYPE_I16) return TY_I16();
    if (lkind == TYPE_U16 || rkind == TYPE_U16) return TY_U16();
    if (lkind == TYPE_I8  || rkind == TYPE_I8)  return TY_I8();
    if (lkind == TYPE_U8  || rkind == TYPE_U8)  return TY_U8();

    return TY_ERROR();
}

TYPE* ImplicitCast(TYPE* lhs, TYPE* rhs)
{
    if (!lhs || !rhs) return TY_ERROR();
    TypeKind lkind = lhs->kind, rkind = rhs->kind;

    if (lkind == rkind) return KindToType(lkind);
    if (lkind == TYPE_ERROR || rkind == TYPE_ERROR) return TY_ERROR();
    if (lkind == TYPE_VOID  || rkind == TYPE_VOID)  return TY_ERROR();

    if (rkind == TYPE_NULL) {
        if (lkind == TYPE_PTR || lkind == TYPE_STRING || lkind == TYPE_ARR)
            return lhs;
        return TY_ERROR();
    }

    if (lkind == TYPE_BOOL && TypeHasCategory(rkind, C_NUMERIC)) return rhs;
    if (rkind == TYPE_BOOL && TypeHasCategory(lkind, C_NUMERIC)) return lhs;

    if (lkind == TYPE_DOUBLE || rkind == TYPE_DOUBLE) {
        if (TypeHasCategory(lkind, C_NUMERIC) && TypeHasCategory(rkind, C_NUMERIC))
            return TY_DOUBLE();
    }
    if (lkind == TYPE_FLOAT || rkind == TYPE_FLOAT) {
        if (TypeHasCategory(lkind, C_NUMERIC) && TypeHasCategory(rkind, C_NUMERIC))
            return TY_FLOAT();
    }

    if (TypeHasCategory(lkind, C_INTEGRAL) && TypeHasCategory(rkind, C_INTEGRAL))
        return IntegerPromotion(lhs, rhs);

    if (lkind == TYPE_PTR && TypeHasCategory(rkind, C_INTEGRAL)) return lhs;
    if (rkind == TYPE_PTR && TypeHasCategory(lkind, C_INTEGRAL)) return rhs;

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
    else if (rtype == LVAL_RULE) {
        for (i = 0; i < LVAL_RULES_SIZE; i++) {
            if (LVAL_RULES[i].op == ttype) {
                rule.rule.l = LVAL_RULES[i];
                return rule;
            }
        }
    }
    OperatorRule ERR; ERR.rtype == ERROR_RULE;
    return ERR; 
}

/* ---------- Error Handling ----------- */

TYPE* TERROR_INCOMPATIBLE(OperatorRule rule, ASTNode* node) 
{
    int line = node->token.line;
    if (rule.rtype == BINARY_RULE)
        printf("TYPE ERROR: Incompatible types found for binary operator '%s' on line %d\n", TokenToStr(rule.rule.b.op), line);  /* TODO: Translate this to  a string instead of enum */
    else if (rule.rtype == UNARY_RULE)
        printf("TYPE ERROR: Incompatible types found for unary operator '%s' on line %d\n", TokenToStr(rule.rule.u.op), line);
    else if (rule.rtype == LVAL_RULE)
        printf("TYPE ERROR: Incompatible types found for lvalue operator '%s' on line %d\n", TokenToStr(rule.rule.u.op), line);

    return TY_ERROR();
}

TYPE* TERROR_NO_RULE(OperatorRule rule, ASTNode* node)
{
    /* TODO: Have this print what type it actually is (Map of types to string) */
    /* TODO: Not a very clear error message AT ALL, fix this */
    int line = node->token.line;
    printf("TYPE ERROR: No rule found for operator %d on line%d\n",  rule.rule.b.op, line);

    return TY_ERROR();
}

TYPE* TERROR_UNDEFINED(ASTNode* node) 
{
    int line = node->token.line;
    char* name = node->token.lex.word;
    printf("TYPE ERROR: Undefined type '%s' on line %d\n", name, line);

    return TY_ERROR();
}

TYPE* TERROR_CAST(TYPE* left, TYPE* right, ASTNode* expr) 
{
    int line = expr->token.line;
    char* name = expr->token.lex.word;
    printf("TYPE ERROR: Invalid implicit cast between type %d and type %d on line %d\n", left->kind, left->kind, line);

    return TY_ERROR();
}

TYPE* TERROR(char* msg, ASTNode* node, NamespaceKind kind)
{
    /* 
        TODO: Check if standard type, TypeKindTo str
        if user defined type, get type name and print 
    */
    int line = node->token.line;
    char* name = node->token.lex.word;

    printf("TYPE ERROR: %s in namespace %d on line %d\n", msg, kind, line);

    return TY_ERROR();
}

void TWARN(char* msg)
{
    if (WARN)
        printf("%s\n", msg);
}
