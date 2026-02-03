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