#include "TypeChecker.h"

/*
Cases:
    Decl Stmt Node - Ensure TypeNode compatible with each VarNode 
    in VarListNode

        1.) VAR_NODE - Ident and Possible ExprNode OR
            a.) ARR_INIT_NODE - Possible multiple Literal Children


    Expression Node - Ensure Expression is valid given Ident'Type (stype in Symbol*)

    Properties - 
        1.) Indexing only allowed on arrays
        2.) Function calls only allowed on functions 
        3.) Range of values is respected (ie int only contains values from -2^31 to 2^31)

        4.) Char can only be assigned integral types 
            OR character literals 
            OR idents that represent Integral / literals 
        5.) Short / Ints / Long can only be assigned Integral Types 
            or idents that represent Integral
        6.) Floats / Doubles can only be assigned Integral types or Decimal Types 
            or idents that represents Decmials
        7.) Booleans can only be assiged true or false 
            or idents that represent true or false (WIP)
        8.) Strings can only be assigned string literals 
            OR idents that represnt strings   
*/

/* ---------- Types ---------- */

TYPE* TY_VOID(void)   { static TYPE t = { TYPE_VOID };   return &t; }
TYPE* TY_ERROR(void)  { static TYPE t = { TYPE_ERROR };  return &t; }
TYPE* TY_NULL(void)   { static TYPE t = { TYPE_NULL };   return &t; }

TYPE* TY_INT(void)    { static TYPE t = { TYPE_INT };    return &t; }
TYPE* TY_FLOAT(void)  { static TYPE t = { TYPE_FLOAT };  return &t; }
TYPE* TY_DOUBLE(void) { static TYPE t = { TYPE_DOUBLE }; return &t; }
TYPE* TY_BOOL(void)   { static TYPE t = { TYPE_BOOL };   return &t; }
TYPE* TY_STRING(void) { static TYPE t = { TYPE_STRING }; return &t; }

TYPE* TY_I8(void)  { static TYPE t = { TYPE_I8 };  return &t; }
TYPE* TY_I16(void) { static TYPE t = { TYPE_I16 }; return &t; }
TYPE* TY_I32(void) { static TYPE t = { TYPE_I32 }; return &t; }
TYPE* TY_I64(void) { static TYPE t = { TYPE_I64 }; return &t; }

TYPE* TY_U8(void)  { static TYPE t = { TYPE_U8 };  return &t; }
TYPE* TY_U16(void) { static TYPE t = { TYPE_U16 }; return &t; }
TYPE* TY_U32(void) { static TYPE t = { TYPE_U32 }; return &t; }
TYPE* TY_U64(void) { static TYPE t = { TYPE_U64 }; return &t; }

TYPE* TY_ARR(TYPE* type, int size) { TYPE* typ = malloc(sizeof(TYPE)); typ->kind = TYPE_ARR; typ->u.array.element = type; typ->u.array.size = size; return typ; }
TYPE* TY_NAME(Symbol* sym, TYPE* type) { TYPE* typ = malloc(sizeof(TYPE)); typ->kind = TYPE_NAME; typ->u.name.sym = sym; typ->u.name.type = type; return typ; }

TYPE* TY_NAT() { static TYPE t = { TYPE_NAT }; return &t; }
TYPE_LIST TY_LIST(TYPE* head, TYPE_LIST* tail);


/* ---------- Error Handling ----------- */

TYPE* TERROR_INCOMPATIBLE(OperatorRule rule, ASTNode* node) 
{
    int line = node->token.line;
    if (rule.rtype == BINARY_RULE)
        printf("TYPE ERROR: Incompatible types found for binary operator %d on line %d\n", rule.rule.b.op, line);  /* TODO: Translate this to  a string instead of enum */
    else if (rule.rtype == UNARY_RULE)
        printf("TYPE ERROR: Incompatible types found for unary operator %d on line %d\n", rule.rule.u.op, line);

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


/* ----------- Environments ---------- */

/* Generates the Symbol Table for Types (ie, ResolveNames should be this, and it should return a SymbolTable)*/
Dict ENV_BaseTenv()
{
    /* Maps Symbol to TYPE* */
    /* Map Tenv */

    /* Tenv[int] = TYPE_INT */
    /* Tenv[float] = TYPE_INT ...*/
    
}


TYPE* TypeCheck(Namespaces* nss, ASTNode* expr)
{
    /* TODO: Have each case call their respective function, and the function
       will recursively call this function, in order to simulate recursive descent 
    */
    switch (expr->type)
    {
        case SLITERAL: return TY_STRING();
        case CLITERAL: return TY_U32();
        case INTEGRAL: return TY_INT();
        case DECIMAL: return TY_DOUBLE();   /* THESE ARE NODE TYPES, THESE ARE TOKEN TYPES */
        case IDENT_NODE:     
            /* Lookup which namespace based on what expr its in */
            /* Lookup Name */

            /* TODO: STLookupNamespace will never return antyhing because they are 
            resolved in the inner namespaces (nested scope), need to pass scope, and 
            then look through all of them. 
            */

            //return TY_NAME(sym, NULL/* Get type from stype associtaed with ident */);
            char* ident = expr->token.lex.word;
            Symbol* sym = STLookupNamespace(nss, ident, N_VAR);
            if (!sym) {
                printf("IDENT NOT FOUND: %s\n", ident);
                return TY_ERROR();
            }
            printf("IDENT FOUND: %s\n", ident);

            /* TODO: Check if sym->type is NULL */
            return sym->type;
        case BINARY_EXPR_NODE: return TypeCheckBinExpr(nss, expr);
        case UNARY_EXPR_NODE:  return TypeCheckUnaExpr(nss, expr->children[0]);

        //case VAR_NODE:  /* Check if assigned a type, and if so check type
        case ASGN_EXPR_NODE: return TypeCheckAsgn(nss, expr);   /* Check valid type */

        case VAR_DECL_NODE:     /* Check matching type */

        case ARR_DECL_NODE:     /* Check Integral Size */

        case ARR_INIT_NODE:     /* Check Valid Types */

        case ARR_INDEX_NODE:   /* Check integral */

        case MEMBER_ACCESS_NODE:  
        
        case CALL_FUNC_NODE:    /* Check Valid Type */

        case TYPEDEF_DECL_NODE: /* Check for existance */

        case ENUM_BODY_NODE:    /* Check all integral */

        default:
            /* Recursively check children, and then return */
            for (size_t i = 0; i < expr->childCount; i++) {
                TYPE* type = TypeCheck(nss, expr->children[i]);
                if (type->kind == TYPE_ERROR) return type;
            } 
            break;
    }
    //printf("ERROR NAT: %s\n", expr->token.lex.word);
    return TY_NAT();
}

TYPE* TypeCheckBinExpr(Namespaces* nss, ASTNode* expr) 
{
 printf("Binary Expr\n"); 
    /* First child is ident or another epxr */
    TYPE* left = TypeCheck(nss, expr->children[0]);
    if (left->kind == TYPE_ERROR) return left;

    TYPE* right = TypeCheck(nss, expr->children[1]);
    if (right->kind == TYPE_ERROR) return right;

    Token operator = expr->token;
    OperatorRule rule = FindRule(operator.type, BINARY_RULE);
    if (rule.rtype == ERROR_RULE) 
        return TERROR_NO_RULE(rule, expr);

    printf("Lkind: %d, Lrule: %d, Rkind: %d, Rrule %d\n",left->kind, rule.rule.b.left, right->kind, rule.rule.b.right);
    /* Compare rule to current expr */
    if (!TypeHasCategory(left->kind, rule.rule.b.left) || !TypeHasCategory(right->kind, rule.rule.b.right)) 
        return TERROR_INCOMPATIBLE(rule, expr);

    /* Resulting Expr Type based on operator rule */
    TYPE* result = rule.rule.b.result(left, right);
    return result;
}

TYPE* TypeCheckUnaExpr(Namespaces* nss, ASTNode* expr)
{
 printf("Unary Expr\n");
    TYPE* left = TypeCheck(nss, expr->children[0]);
    if (left->kind == TYPE_ERROR) return left;

    Token operator = expr->token;
    OperatorRule rule = FindRule(operator.type, UNARY_RULE);
    if (rule.rtype == ERROR_RULE) 
        return TERROR_NO_RULE(rule, expr);

    /* For arrays */
    if (!TypeHasCategory(left->kind, rule.rule.u.cat)) 
        return TERROR_INCOMPATIBLE(rule, expr);

    TYPE* result = rule.rule.u.result(left, NULL);
    return result;
}

TYPE* TypeCheckAsgn(Namespaces* nss, ASTNode* expr) 
{
printf("Asgn Expr\n");
    TYPE* left = TypeCheck(nss, expr->children[0]);
    if (left->kind == TYPE_ERROR) return left;

    TYPE* right = TypeCheck(nss, expr->children[1]);
    if (right->kind == TYPE_ERROR) return right;

    Token operator = expr->token;
    OperatorRule rule = FindRule(operator.type, BINARY_RULE);
    if (rule.rtype == ERROR_RULE) 
        return TERROR_NO_RULE(rule, expr);

    /* Compare rule to current expr */
    if (!TypeHasCategory(left->kind, rule.rule.b.left) || !TypeHasCategory(right->kind, rule.rule.b.right)) 
        return TERROR_INCOMPATIBLE(rule, expr);

    TYPE* result = rule.rule.b.result(left, right);
    return result;
}

TYPE* TypeCheckVar(Namespaces* nss, ASTNode* var) 
{
    /* TODO: Later check typedefs, for now assume var */
    //EnvironmentEntry entry = STLookup(venv, var->token.lex.word);
    EnvironmentEntry entry;
    entry.kind = ENV_VAR_ENTRY;
    entry.u.var.ty;

    /*Symbol* sym = STLookupNamespace(nss, var->token.lex.word, N_VAR);
   
    Symbol* sym;
    switch(sym->stype) {
        default: 
            break;
    }
            */

    return TY_ERROR();
}

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