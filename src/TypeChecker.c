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



/* Generates the Symbol Table for Types (ie, ResolveNames should be this, and it should return a SymbolTable)*/
SymbolTable ENV_BaseTenv()
{

}


TYPE* TypeCheckExpr(SymbolTable venv, SymbolTable tenv, ASTNode* expr)
{
    Token operator = expr->token;
    switch (expr->type)
    {
        case BINARY_EXPR_NODE:
            TYPE* left = TypeCheckExpr(venv, tenv, expr->children[0]);
            TYPE* right = TypeCheckExpr(venv, tenv, expr->children[1]);
            if (operator.type == PLUS) {
                if (left->kind != PLUS TYPES)
                if (left->kind == TY_INT )
            }
            break;
        
        default:
            break;
    }
}

/* ---------- Helpers ---------- */

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

    return TY_INT();
}



TYPE* BoolType(TYPE* lhs, TYPE* rhs)
{
    if (lhs->kind == rhs->kind)
        return TY_BOOL();

    return TY_ERROR();
}

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