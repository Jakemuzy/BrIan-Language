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
        - Split into 4 headers, TYPE*, 
        Environment, TypeCheck Functions, and Table Driven Rules
        - Instead of checking type->kind == TYPE_ERROR, just check if the 
        pointer == TY_ERROR() since its static
*/

/* ---------- Error Handling ----------- */

TYPE* TERROR_INCOMPATIBLE(OperatorRule rule, ASTNode* node) 
{
    int line = node->token.line;
    if (rule.rtype == BINARY_RULE)
        printf("TYPE ERROR: Incompatible types found for binary operator %d on line %d\n", rule.rule.b.op, line);  /* TODO: Translate this to  a string instead of enum */
    else if (rule.rtype == UNARY_RULE)
        printf("TYPE ERROR: Incompatible types found for unary operator %d on line %d\n", rule.rule.u.op, line);

    printf("NEITHER\n");
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
    printf("TYPE ERROR: Undefined type %s on line %d\n", name, line);

    return TY_ERROR();
}

TYPE* TERROR(char* msg, ASTNode* node, NamespaceKind kind)
{
    int line = node->token.line;
    char* name = node->token.lex.word;
    printf("TYPE ERROR: %s in namespace %d on line %d\n", msg, kind, line);

    return TY_ERROR();
}





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
        case UNARY_EXPR_NODE:  return TypeCheckUnaExpr(nss, expr->children[0]);

        //case VAR_NODE:  /* Check if assigned a type, and if so check type
        case ASGN_EXPR_NODE: return TypeCheckAsgn(nss, expr);   /* Check valid type */

        case VAR_DECL_NODE:     /* TODO: Assign type to symbols (currently null from name reoslver) */
            return TypeCheckVarDecl(nss, expr);

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


/* ----------- Identifiers ---------- */


TYPE* TypeCheckVarDecl(Namespaces* nss, ASTNode* expr)
{
    /* Check if asignments are of valid type */
    /* Set the TYPE of idents in Symbol* so further mentions can grab type */
    ASTNode* typeNode = expr->children[0];
    
    return TY_NAT();
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


    ASTNode* paramListNode = expr->children[2];
    TYPE* params = TypeCheckParams(nss, paramListNode); 
    if (params->kind == TYPE_ERROR) return params;
  
    ASTNode* bodyNode = expr->children[3];
    TYPE* body = TypeCheck(nss, bodyNode);
    if (body->kind == TYPE_ERROR) return body;

    return TY_NAT();
}

TYPE* TypeCheckParams(Namespaces* nss, ASTNode* expr) 
{
    for (size_t i = 0; i < expr->childCount; i++) 
    {
        TYPE* type = TypeCheckParam(nss, expr->children[i]);
        if (type->kind == TYPE_ERROR) return type;
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

    /* TODO: Lval check instead */

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


/* ---------- Type Definitions --------- */