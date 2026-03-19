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


    TODO: 
        - Instead of checking type->kind == TYPE_ERROR, just check if the 
        pointer == TY_ERROR() since its static
        - Account for environments
        - Fix Parser to allow idents as types in some nodes (ie param only takes type)
*/

/* ---------- Type Checking ---------- */

TYPE* TypeCheck(Namespaces* nss, ASTNode* expr)
{
    /* TODO: Have each case call their respective function, and the function
       will recursively call this function, in order to simulate recursive descent 
    */
    switch (expr->type)
    {
        case LITERAL_NODE: /* Determine type */
            switch (expr->token.type) {
                case SLITERAL: return TY_STRING();
                case CLITERAL: return TY_U32();
                case INTEGRAL: return TY_INT();
                case DECIMAL: return TY_DOUBLE();   
                case NILL: return TY_NULL();
                case TRUE: return TY_BOOL();
                case FALSE: return TY_BOOL();
                default: return TERROR("Invalid literal type", expr, N_VAR);
            }
        case IDENT_NODE:     
            /* Lookup which namespace based on what expr its in */
            /* Lookup Name */

            /* TODO: STLookupNamespace will never return antyhing because they are 
            resolved in the inner namespaces (nested scope), need to pass scope, and 
            then look through all of them. 
            */

            //return TY_NAME(sym, NULL/* Get type from stype associtaed with ident */);
            return ValidLval(nss, expr, N_VAR);

            /* TODO: Check if sym->type is NULL */
        case FUNC_NODE: 

            /* 
            1.) Registers func return type  
                a.) If type is NULL, its a custom type, try to resolve     
            2.) Registers paramter types
                a.) If type is NULL, its a custom type, try to resolve
            3.) Return TypeCheck(expr->children['BodyNode'])
            */
           return TypeCheckFunc(nss, expr);
        case BINARY_EXPR_NODE: return TypeCheckBinExpr(nss, expr);
        case UNARY_EXPR_NODE:  return TypeCheckUnaExpr(nss, expr);

        //case VAR_NODE:  /* Check if assigned a type, and if so check type
        case ASGN_EXPR_NODE: return TypeCheckAsgn(nss, expr);   /* Check valid type */
        case VAR_DECL_NODE:  return TypeCheckVarDecl(nss, expr);
        case CALL_FUNC_NODE: return TypeCheckCallFunc(nss, expr);

        case DO_WHILE_STMT_NODE: return TypeCheckDoWhileLoop(nss, expr);
        case WHILE_STMT_NODE:    return TypeCheckWhileLoop(nss, expr);
        case FOR_STMT_NODE:      return TypeCheckForLoop(nss, expr);
        case IF_STMT_NODE:       return TypeCheckIfStmt(nss, expr);
        case SWITCH_STMT_NODE:   return TypeCheckSwitchStmt(nss, expr);
        case RETURN_STMT_NODE:   return TypeCheckReturnStmt(nss, expr);

        case ARR_DECL_NODE:     /* Check Integral Size */

        case ARR_INIT_NODE:     /* Check Valid Types */

        case ARR_INDEX_NODE:   /* Check integral */

        case MEMBER_ACCESS_NODE:  return TypeCheckMemberAccess(nss, expr);
        case TYPEDEF_DECL_NODE:   return TypeCheckTypedef(nss, expr);

        case ENUM_BODY_NODE:    /* Check all integral */

        case STRUCT_DECL_NODE: 
            /* 
                1. Type check return Type 
                2. Enter Body
                3. Type check everything in body by 
                   calling TypeCheck() 
            */
            return TypeCheckStructDecl(nss, expr);

        default:
            /* Recursively check children, and then return */
            for (size_t i = 0; i < expr->childCount; i++) {
                TYPE* type = TypeCheck(nss, expr->children[i]);
                if (type == TY_ERROR()) return type;
            } 
            break;
    }
    //printf("ERROR NAT: %s\n", expr->token.lex.word);
    return TY_NAT();
}


/* ----------- Identifiers ---------- */


TYPE* TypeCheckVarDecl(Namespaces* nss, ASTNode* expr)
{
    ASTNode* typeNode = expr->children[0];
    char* typeLex = typeNode->token.lex.word;

    TYPE* type = StrToType(typeLex);
    if (!type) {
        Symbol* sym = STLookupNamespace(nss, typeLex, N_TYPE);
        if (!sym || !sym->type)
            return TERROR_UNDEFINED(typeNode);
        type = sym->type;
    }


    ASTNode* varListNode = expr->children[1];
    for (size_t i = 0; i < varListNode->childCount; i++) {
        TYPE* ty = TypeCheckVar(nss, varListNode->children[i], type);
        if (ty == TY_ERROR()) return ty;
    }
    
    return TY_NAT();
}

TYPE* TypeCheckVar(Namespaces* nss, ASTNode* var, TYPE* type) 
{
    /* TODO: Account for Envrionments  */
    //EnvironmentEntry entry = STLookup(venv, var->token.lex.word);

    /*
    EnvironmentEntry entry;
    entry.kind = ENV_VAR_ENTRY;
    entry.u.var.ty;
    */

    /* TODO: 
    eq doesn't have a rule since it has to do with lvals, 
    must check these
    */
    
    /* Actual Var checking */
    ASTNode* identNode = var->children[0];
    char* identName = identNode->token.lex.word;
    Symbol* sym = STLookupNamespace(nss, identName, N_VAR);
    sym->type = type;     

    /* Check if pointer before do this */
    if (type->kind == TYPE_VOID) 
        return TERROR("Cannot have variable of type void", identNode, N_VAR);

    /* Check compatibility of asgnment with ident type */
    if (var->childCount > 1) {
        TYPE* rhs = TypeCheck(nss, var->children[1]);
        if (rhs == TY_ERROR()) return rhs;

        /* Use operator rules since technically an '=' */
        OperatorRule rule = FindRule(EQ, LVAL_RULE);
        if (rule.rtype == ERROR_RULE) 
            return TERROR_NO_RULE(rule, var);

        if (!TypeHasCategory(sym->type->kind, rule.rule.b.left) || !TypeHasCategory(rhs->kind, rule.rule.b.right)) 
            return TERROR_INCOMPATIBLE(rule, var);

        /* Resulting Expr Type based on operator rule */
        TYPE* result = rule.rule.b.result(sym->type, rhs);
        if (result == TY_ERROR())
            return TERROR_CAST(sym->type, rhs, identNode);
        return result;
    }

    return type;
}

TYPE* TypeCheckFunc(Namespaces* nss, ASTNode* expr) 
{
    ASTNode* typeNode = expr->children[0];
    char* typeLex = typeNode->token.lex.word;

    /* If not valid type, check N_TYPES */
    TYPE* type = StrToType(typeLex);
    if (!type) {
        Symbol* sym = STLookupNamespace(nss, typeLex, N_TYPE);
        if (!sym || !sym->type)
            return TERROR_UNDEFINED(typeNode);
        type = sym->type;
        // type = TY_NAME(sym, sym->type);  TODO: Goes in typedef
    }
    
    /* Set function return type */
    ASTNode* identNode = expr->children[1];
    char* identName = identNode->token.lex.word;
    Symbol* sym = STLookupNamespace(nss, identName, N_VAR);
    sym->type = type;

    /* Check if pointer before do this */   /* TODO: CAN have */
    if (type->kind == TYPE_VOID) 
        return TERROR("Cannot have function of type void", identNode, N_VAR);

    ASTNode* paramListNode = expr->children[2];
    TYPE* params = TypeCheckParams(nss, paramListNode); 
    if (params == TY_ERROR()) return params;
  
    ASTNode* bodyNode = expr->children[3];
    TYPE* body = TypeCheck(nss, bodyNode);
    if (body == TY_ERROR()) return body;

    return TY_NAT();
}

TYPE* TypeCheckParams(Namespaces* nss, ASTNode* expr) 
{
    for (size_t i = 0; i < expr->childCount; i++) 
    {
        TYPE* type = TypeCheckParam(nss, expr->children[i]);
        if (type == TY_ERROR()) return type;
    }
    return TY_NAT();
}

TYPE* TypeCheckParam(Namespaces* nss, ASTNode* expr)
{
    /* Early Exit */
    if (expr->type == EMPTY_NODE) return TY_NAT();

    ASTNode* typeNode = expr->children[0];
    char* typeLex = typeNode->token.lex.word;
    TYPE* type = StrToType(typeLex);

    /* If not valid type, check N_TYPES */
    if (!type) {
        Symbol* sym = STLookupNamespace(nss, typeLex, N_TYPE);
        if (!sym || !sym->type)
            return TERROR_UNDEFINED(typeNode);
        type = sym->type;
    }
    
    char* identName = expr->children[1]->token.lex.word;
    Symbol* sym = STLookupNamespace(nss, identName, N_VAR);
    sym->type = type;
    return type;
}


/* ---------- Expressions ---------- */


TYPE* TypeCheckBinExpr(Namespaces* nss, ASTNode* expr) 
{
    /* First child is ident or another epxr */
    TYPE* left = TypeCheck(nss, expr->children[0]);
    if (left == TY_ERROR()) return left;

    TYPE* right = TypeCheck(nss, expr->children[1]);
    if (right == TY_ERROR()) return right;

    Token operator = expr->token;
    OperatorRule rule = FindRule(operator.type, BINARY_RULE);
    if (rule.rtype == ERROR_RULE) 
        return TERROR_NO_RULE(rule, expr);

    /* Compare rule to current expr */
    if (!TypeHasCategory(left->kind, rule.rule.b.left) || !TypeHasCategory(right->kind, rule.rule.b.right)) 
        return TERROR_INCOMPATIBLE(rule, expr);

    /* Resulting Expr Type based on operator rule */
    TYPE* result = rule.rule.b.result(left, right);
    return result;
}

TYPE* TypeCheckUnaExpr(Namespaces* nss, ASTNode* expr)
{
    TYPE* left = TypeCheck(nss, expr->children[0]);
    if (left == TY_ERROR()) return left;

    Token operator = expr->token;
    OperatorRule rule = FindRule(operator.type, UNARY_RULE);
    if (rule.rtype == ERROR_RULE) 
        return TERROR_NO_RULE(rule, expr);

    /* For arrays */
    if (!TypeHasCategory(left->kind, rule.rule.u.cat)) 
        return TERROR_INCOMPATIBLE(rule, expr);

    /* TODO: Check if the rule actually exists first before calling func */
    TYPE* result = rule.rule.u.result(left, NULL);
    return result;
}

TYPE* TypeCheckAsgn(Namespaces* nss, ASTNode* expr) 
{
    TYPE* left = TypeCheck(nss, expr->children[0]);
    if (left == TY_ERROR()) return left;

    TYPE* right = TypeCheck(nss, expr->children[1]);
    if (right == TY_ERROR()) return right;

    Token operator = expr->token;
    OperatorRule rule = FindRule(operator.type, LVAL_RULE);
    if (rule.rtype == ERROR_RULE) 
        return TERROR_NO_RULE(rule, expr);

    /* Compare rule to current expr */
    if (!TypeHasCategory(left->kind, rule.rule.b.left) || !TypeHasCategory(right->kind, rule.rule.b.right)) 
        return TERROR_INCOMPATIBLE(rule, expr);

    TYPE* result = rule.rule.b.result(left, right);
    if (result == TY_ERROR()) 
        return TERROR_CAST(left, right, expr);

    return result;
}


/* ---------- Conditionals & Loops ----------- */

TYPE* TypeCheckWhileLoop(Namespaces* nss, ASTNode* expr)
{
    // Boolean
    ASTNode* conditionNode = expr->children[0];
    TYPE* condType;

    if (conditionNode->type == IDENT_NODE) {
        char* identLex = conditionNode->token.lex.word;
        /* Can only be a var */
        Symbol* sym = STLookupNamespace(nss, identLex, N_VAR);
        condType = sym->type;
    } else {
        condType = TypeCheck(nss, conditionNode);
        if (condType == TY_ERROR()) return condType;
    }

    if (!condType) 
        return TERROR("Undefined type", conditionNode, N_VAR);

    if (!TypeHasCategory(condType->kind, C_BOOLEAN)) 
        return TERROR("Condition in 'WHILE' loop is not boolean", conditionNode, N_VAR);

    /* Type check the body */
    return TypeCheck(nss, expr->children[1]);
}

TYPE* TypeCheckDoWhileLoop(Namespaces* nss, ASTNode* expr)
{
    // Boolean

    return TY_NAT();
}

TYPE* TypeCheckForLoop(Namespaces* nss, ASTNode* expr)
{
    /* 
        1.) Type check exprs in params (TypeCheck)
        2.) Type check body
    */
    return TY_NAT();
}

TYPE* TypeCheckIfStmt(Namespaces* nss, ASTNode* expr)
{
    for (size_t i = 0; i < expr->childCount; i++) {
        NodeType nodeType = expr->children[i]->type;
        TYPE* condType;
        if (nodeType == IF_NODE || nodeType == ELIF_NODE) 
            condType = TypeCheckIfElif(nss, expr->children[i]);
        else if (nodeType == ELSE_NODE)
            condType = TypeCheckElse(nss, expr->children[i]);
        else 
            return TERROR("Invalid node type in If Stmt", expr->children[i], N_VAR);

        if (condType == TY_ERROR())
            return TY_ERROR();
    }   
    
    return TY_NAT();
}

TYPE* TypeCheckIfElif(Namespaces* nss, ASTNode* expr)
{
    ASTNode* condNode = expr->children[0];
    TYPE* condType = TypeCheck(nss, condNode);
    if (condType == TY_ERROR())
        return TY_ERROR();
    else if (!TypeHasCategory(condType->kind, C_BOOLEAN))
        return TERROR("If stmt conditional does not evaluate to a boolean", condNode, N_VAR);
    
    ASTNode* bodyNode = expr->children[1];
    return TypeCheck(nss, bodyNode);
}

TYPE* TypeCheckElse(Namespaces* nss, ASTNode* expr)
{
    return TypeCheck(nss, expr->children[0]);
}

TYPE* TypeCheckSwitchStmt(Namespaces* nss, ASTNode* expr)
{

    return TY_NAT();
}

TYPE* TypeCheckReturnStmt(Namespaces* nss, ASTNode* expr)
{
    /* Compare return type to function type */
    return TY_NAT();
}

/* ---------- Accessors & Initalizers ----------- */


TYPE* TypeCheckMemberAccess(Namespaces* nss, ASTNode* expr)
{
    ASTNode* identNode = expr->children[0];
    char* identLex = identNode->token.lex.word;

    Symbol* sym = STLookupNamespace(nss, identLex, N_VAR);
    Namespaces* memberFields = sym->fields;

    ASTNode* memberNode = expr->children[1];
    char* memberLex = memberNode->token.lex.word;

    /* TODO: May have to look up types since struct would break this */
    Symbol* memSym = STLookupNamespace(memberFields, memberLex, N_VAR); 
    return memSym->type;
}
TYPE* TypeCheckArrInitializer(Namespaces* nss, ASTNode* decl)
{

}
TYPE* TypeCheckStructInitalizer(Namespaces* nss, ASTNode* decl)
{

}

/* ---------- Type Definitions --------- */

TYPE* TypeCheckCallFunc(Namespaces* nss, ASTNode* expr)
{
    ASTNode* identNode = expr->children[0];
    char* identLex = identNode->token.lex.word;

    Symbol* sym = STLookupNamespace(nss, identLex, N_VAR);
    if (!sym) return TERROR_UNDEFINED(identNode);

    /* TODO: Check Paramaters */


    return sym->type;
}

TYPE* TypeCheckTypedef(Namespaces* nss, ASTNode* expr)
{
    ASTNode* typeNode = expr->children[0];
    char* typeLex = typeNode->token.lex.word;

    TYPE* type = StrToType(typeLex);
    if (!type) {
        Symbol* sym = STLookupNamespace(nss, typeLex, N_TYPE);
        if (!sym || !sym->type)
            return TERROR_UNDEFINED(typeNode);
        type = sym->type;
    }
    
    // type = TY_NAME(sym, sym->type);  TODO: Goes in typedef

    ASTNode* newTypeNode = expr->children[1];
    char* newTypeLex = newTypeNode->token.lex.word;
    Symbol* sym = STLookupNamespace(nss, newTypeLex, N_TYPE);
    if (sym->type) return TERROR("Type is already defined", newTypeNode, N_TYPE);

    sym->type = type;
    return sym->type;
}

TYPE* TypeCheckStructDecl(Namespaces* nss, ASTNode* expr)
{
    ASTNode* structIdent = expr->children[0];
    char* structLex = structIdent->token.lex.word;
    Symbol* sym = STLookupNamespace(nss, structLex, N_TYPE);

    ASTNode* structBodyNode = expr->children[1];
    TYPE_FIELD_LIST* fields =  TypeCheckStructBody(nss, expr);

    /* Get Type Field List from the fields */
    sym->type = TY_STRUCT(TY_FIELD_LIST(fields->head, fields->tail));
    return TypeCheck(nss, structBodyNode);
}

TYPE_FIELD_LIST* TypeCheckStructBody(Namespaces* nss, ASTNode* expr)
{
    /* Copy of TypeCheck except it stores field info */
}

TYPE_FIELD_LIST* TypeCheckEnumBody(Namespaces* nss, ASTNode* expr)
{

}