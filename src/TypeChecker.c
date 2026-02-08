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

TYPE* TY_NULL() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_NULL; return type; }
TYPE* TY_INT() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_INT; return type; }
TYPE* TY_FLOAT() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_FLOAT; return type; }
TYPE* TY_DOUBLE() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_DOUBLE; return type; }
TYPE* TY_BOOL() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_BOOL; return type; }
TYPE* TY_STRING() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_STRING; return type; }

TYPE* TY_I8() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_I8; return type; }
TYPE* TY_I16() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_I16; return type; }
TYPE* TY_I32() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_I32; return type; }
TYPE* TY_I64() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_I64; return type; }

TYPE* TY_U8() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_U8; return type; }
TYPE* TY_U16() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_U16; return type; }
TYPE* TY_U32() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_U32; return type; }
TYPE* TY_U64() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_U64; return type; }

TYPE* TY_VOID() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_VOID; return type; }
TYPE* TY_ERROR() { TYPE* type = malloc(sizeof(TYPE)); type->kind = TYPE_ERROR; return type; }

TYPE* TY_ARR(TYPE* type) { TYPE* typ = malloc(sizeof(TYPE)); typ->kind = TYPE_ARR; typ->u.array = type; typ; }
TYPE* TY_NAME(Symbol* sym, TYPE* type) { TYPE* typ = malloc(sizeof(TYPE)); typ->kind = TYPE_NAME; typ->u.name.sym = sym; typ->u.name.type = type; return typ; }

TYPE_LIST TY_LIST(TYPE* head, TYPE_LIST* tail);


/* ---------- Error Handling ----------- */

void TERROR_INCOMPATIBLE(OperatorRule rule) 
{
    if (rule.rtype == BINARY_RULE)
        printf("TYPE ERROR: No rule found for operator %c\n", rule.rule.b.op);  /* TODO: Translate this to  a string instead of enum */
    else if (rule.rtype == UNARY_RULE)
        printf("TYPE ERROR: No rule found for operator %c\n", rule.rule.u.op);
}
void TERROR_NO_RULE(OperatorRule rule)
{
    /* TODO: Have this print what type it actually is */
    if (rule.rtype == BINARY_RULE)
        printf("TYPE ERROR: Invalid type %d for operator %c\n", rule.rtype, rule.rule.b.op);
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


TYPE* TypeCheckExpr(SymbolTable* venv, Dict* tenv, ASTNode* expr)
{
    Token operator = expr->token;
    switch (expr->type)
    {
        case BINARY_EXPR_NODE:
            /* First child is ident or another epxr */
            TYPE* left = TypeCheckExpr(venv, tenv, expr->children[0]);
            if (left->kind == TYPE_ERROR) return left;

            TYPE* right = TypeCheckExpr(venv, tenv, expr->children[1]);
            if (right->kind == TYPE_ERROR) return right;

            OperatorRule rule = FindRule(operator.type, BINARY_RULE);
            if (rule.rtype == ERROR_RULE) {
                TERROR_NO_RULE(rule);
                return TY_ERROR();  
            }

            /* Compare rule to current expr */
            TypeKind kind = left->kind == TYPE_NAME ? left->kind : left->u.name.type->kind;
            if (!TypeHasCategory(left->kind, rule.rule.b.left) || !TypeHasCategory(right->kind, rule.rule.b.right)) {
                TERROR_INCOMPATIBLE(rule);
                return TY_ERROR();  
            }

            /* Resulting Expr Type based on operator rule */
            TYPE* result = rule.rule.b.result(left, right);
            return result;

        case UNARY_EXPR_NODE:
            TYPE* var = TypeCheckExpr(venv, tenv, expr->children[0]);
            break;

        case IDENT_NODE:
            /* Lookup Name */
            TypeCheckVar(venv, tenv, expr);

            //return TY_NAME(sym, NULL/* Get type from stype associtaed with ident */);
        
        default:
            break;
    }
}

TYPE* TypeCheckVar(SymbolTable* venv, Dict* tenv, ASTNode* var) 
{
    //EnvironmentEntry entry = STLookup(venv, var->token.lex.word);
    switch(var->type) {
        default: 
            break;
    }
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