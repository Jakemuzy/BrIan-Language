#include "NameResolver.h"

/* ----------- Error Handling ---------- */


int NERROR_NO_IDENT(ASTNode* curr)
{
    printf("NAME ERROR: No Identifier found on line '%d'\n", curr->token.line); 
    return ERRN;
}

int NERROR_ALREADY_DEFINED(char* name, ASTNode* curr, ASTNode* first)
{
    printf("NAME ERROR: Identifier '%s' on line %d already defined, first definition on line %d\n", name, curr->token.line, first->token.line); \
    return ERRN; 
}

int NERROR_DOESNT_EXIST(char* name, ASTNode* curr) 
{
    printf("NAME ERROR: Identifier '%s' on line %d is undefined\n", name, curr->token.line);
    return ERRN;
}


/* ----------- Helper ----------- */

TYPE* StringToType(const char* name)
{
    if (!name) return TY_ERROR();

    if (strcmp(name, "int") == 0)     return TY_INT();
    if (strcmp(name, "bool") == 0)    return TY_BOOL();
    if (strcmp(name, "double") == 0)  return TY_DOUBLE();
    if (strcmp(name, "float") == 0)   return TY_FLOAT();
    if (strcmp(name, "void") == 0)    return TY_VOID();
    if (strcmp(name, "string") == 0)  return TY_STRING();

    if (strcmp(name, "I8") == 0)   return TY_I8();
    if (strcmp(name, "I16") == 0)  return TY_I16();
    if (strcmp(name, "I32") == 0)  return TY_I32();
    if (strcmp(name, "I64") == 0)  return TY_I64();

    if (strcmp(name, "U8") == 0)   return TY_U8();
    if (strcmp(name, "U16") == 0)  return TY_U16();
    if (strcmp(name, "U32") == 0)  return TY_U32();
    if (strcmp(name, "U64") == 0)  return TY_U64();

    if (strcmp(name, "struct") == 0)  return TY_STRUCT();
    if (strcmp(name, "null") == 0)    return TY_NULL();

    // If it wasn’t a builtin, treat as named type
    return NULL;
}

/* ----------- Name Resolution ---------- */

Namespaces* ResolveNames(AST* ast) 
{
    Scope* scope = ScopeInit(2, N_VAR, N_TYPE);
    scope = BeginScope(scope, PROG_SCOPE);

    if (ResolveEverything(scope, ast->root) != VALDN)
        return NULL;

    scope = ExitScope(scope);
    return scope->namespaces;
}

int ResolveEverything(Scope* scope, ASTNode* current)
{
    if (!current) return VALDN;

    int status = NANN;
    switch (current->type) {
        case(VAR_DECL_NODE):
            status = ResolveVars(scope, current);
            break;
        case (TYPEDEF_DECL_NODE):
            status = ResolveTypedefs(scope, current);
            break;
        case (FUNC_NODE):
            return ResolveFuncs(&scope, current);   /* Return since ResolveFuncs already traverses Children */
        case (IF_STMT_NODE):
        case (DO_WHILE_STMT_NODE):
        case (WHILE_STMT_NODE):
        case (SWITCH_STMT_NODE):
        case (RETURN_STMT_NODE):
        case (FOR_STMT_NODE):
            return ResolveStmts(&scope, current);
        case (EXPR_STMT_NODE):
            status = ResolveExprs(scope, current);
            break;
        case (CALL_FUNC_NODE):
            status = ResolveFuncCall(scope, current);
            break;
    }

    if (status == ERRN) return status;

    /* Recursively check children if none apply */
    for (size_t i = 0; i < current->childCount; i++) {
        if (ResolveEverything(scope, current->children[i]) == ERRN) 
            return ERRN;
    }


    return VALDN;
}

int ResolveVars(Scope* scope, ASTNode* current)
{
    char* typeLex = current->children[0]->token.lex.word;
    TYPE* type = StringToType(typeLex);
    return ResolveVar(scope, current->children[1], type);   
}

int ResolveVar(Scope* scope, ASTNode* current, TYPE* type)
{
    if (current->type == IDENT_NODE) {
        SymbolType stype = S_VAR;
    
        Symbol* sym;
        char* name = current->token.lex.word;

        /* Ctrl Scopes don't allow for shadowing */
        if (scope->stype == CTRL_SCOPE) sym = LookupAllScopes(scope, name, N_VAR);
        else sym = LookupCurrentScope(scope, name, N_VAR);

        if (sym) return NERROR_ALREADY_DEFINED(name, current, sym->decl);
      
        sym = STPushNamespace(scope, current, N_VAR, type);
        PushScope(scope, sym, N_VAR);
        return VALDN;
    }    
    else {
        for (size_t i = 0; i < current->childCount; i++) {
            int status = ResolveVar(scope, current->children[i], type);
            if (status == ERRN) return status;
        }
    }
    return VALDN;
}

int ResolveFuncs(Scope** scope, ASTNode* current)
{
    /* Function Ident, Type and Scope */
    ScopeType stype = GetScopeType(current);

    char* typeLex = current->children[0]->token.lex.word;
    TYPE* type = StringToType(typeLex);


    ASTNode* identNode = current->children[1];
    char* name = identNode->token.lex.word;

    /* Lookup if it exists yet */
    Symbol* sym = LookupAllScopes(*scope, name, N_VAR); /* TODO: lookup all scopes may be wrong here */
    if (sym) return NERROR_ALREADY_DEFINED(name, current, sym->decl);

    sym = STPushNamespace(*scope, identNode, N_VAR, type);
    PushScope(*scope, sym, N_VAR);
    *scope = BeginScope(*scope, stype);

    /* Param List */
    int status = ResolveParams(*scope, current->children[2]);
    if (status != VALDN) return status;

    /* Body Calls Resolve Var */
    status = ResolveEverything(*scope, current->children[3]);
    *scope = ExitScope(*scope);
    return status;
}

int ResolveParams(Scope* scope, ASTNode* current)
{
    if (current->type != PARAM_LIST_NODE) return NANN;

    for (size_t i = 0; i < current->childCount; i++) {
        int status = ResolveParam(scope, current->children[i]);
        if (status != VALDN) return status;
    }
    return VALDN;
}

int ResolveParam(Scope* scope, ASTNode* current) 
{
    if (current->type != PARAM_NODE) return NANN;

    /* Still need to lookup because params can be duplicate */
    ASTNode* identNode = current->children[1];

    Symbol* sym = LookupCurrentScope(scope, identNode->token.lex.word, N_VAR);
    if (sym) return NERROR_ALREADY_DEFINED(identNode->token.lex.word, current, sym->decl);

    TYPE* type = StringToType(current->children[0]->token.lex.word);
    sym = STPushNamespace(scope, identNode, N_VAR, type);
    PushScope(scope, sym, N_VAR);

    return VALDN;
}

int ResolveExprs(Scope* scope, ASTNode* current)
{
    return ResolveExpr(scope, current);
}

int ResolveExpr(Scope* scope, ASTNode* current)
{
    if (current->type == IDENT_NODE) {
        char* name = current->token.lex.word;
        if (!LookupAllScopes(scope, name, N_VAR))
            return NERROR_DOESNT_EXIST(name, current);
        else return VALDN;
    }
    else {
        for (size_t i = 0; i < current->childCount; i++) {
            int status = ResolveExpr(scope, current->children[i]);
            if (status == ERRN) return status;
        }
    }
    return VALDN;
}

int ResolveStmts(Scope** scope, ASTNode* current)
{
    /* TODO: Have resolve stmts handle if / while / etc stmts, 
       and allow for this function to enter scope instead and set a 
       ctrl_stmt flag as well. That way we avoid FindIdentChild and 
       the EnterScopeAsNeeded bloat

       Would need for this function to distringuish between each 
       type of stmt as well, this should be a switch
    */

    /* Return Stmt and CtrlStmt probably the only ones need to check */
    ScopeType stype = GetScopeType(current);

    if (stype == CTRL_SCOPE)
        *scope = BeginScope(*scope, stype);
    /* Return Stmt Here */

    /* Resolve Stmt Similar to Func */

    if (stype == CTRL_SCOPE)
        *scope = ExitScope(*scope);
    return VALDN;
}