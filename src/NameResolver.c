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

ASTNode* FindIdentChild(ASTNode* node) {
    size_t i;
    for (i = 0; i < node->childCount; i++) {
        if ((node->children[i])->type == IDENT_NODE) 
            return node->children[i];
    }
    return NULL;
}

bool IsCtrlStmt(NodeType type) 
{
    for (size_t i = 0; i < CTRL_STMTS_SIZE; i++) {
        if (type == CTRL_STMTS[i] )
            return true;
    }

    return false;
}


NodeType GetScopeType(ASTNode* node) 
{
    if (IsCtrlStmt(node->type))
        return CTRL_SCOPE;
    else if (node->type == FUNC_NODE)
        return FUNC_SCOPE;

    return INVALID_SCOPE;
}


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

    if (ResolveEverything(scope, ast->root, NULL) != VALDN)
        return NULL;

    scope = ExitScope(scope);
    return scope->namespaces;
}

int ResolveEverything(Scope* scope, ASTNode* current)
{
    if (!current) return VALDN;

    if (EnterScopeIfNeeded(&scope, current) == ERRN)
        return ERRN;

    int status;
    do { 
        if ((status = ResolveVars(scope, current)) == VALDN) break;
        else if (status == ERRN) return status;

        if ((status = ResolveTypes(scope, current)) == VALDN) break;
        else if (status == ERRN) return status;

        if ((status = ResolveFuncs(scope, current)) == VALDN) break;
        else if (status == ERRN) return status;

        if ((status = ResolveStmts(scope, current)) == VALDN) break;
        else if (status == ERRN) return status;

        if ((status = ResolveExprs(scope, current)) == VALDN) break;
        else if (status == ERRN) return status;

        /* Recursively check children if none apply */
        for (size_t i = 0; i < current->childCount; i++) {
            if (ResolveEverything(scope, current->children[i]) == ERRN) return ERRN;
        }
    } while (0);


    if (GetScopeType(current) != INVALID_SCOPE)
        scope = ExitScope(scope);

    return VALDN;
}

int EnterScopeIfNeeded(Scope** scope, ASTNode* current)
{
    /* Enters new scope and pushes visible names to outer scope */
    ScopeType stype;
    if ((stype = GetScopeType(current)) != INVALID_SCOPE) {
        /* TODO: make this a switch, */
        if (stype == FUNC_SCOPE) {
            /* TODO: Allow function overloading */
            ASTNode* funcIdent = FindIdentChild(current);
            if (!funcIdent) return NERROR_NO_IDENT(funcIdent);

            TYPE* type = StringToType(current->children[1]->token.lex.word); /* Type Node */
            Symbol* sym = STPushNamespace(*scope, funcIdent, N_VAR, type);
            PushScope(*scope, sym, N_VAR);
        }

        *scope = BeginScope(*scope, stype);
    }
    return VALDN;
}

int ResolveVars(Scope* scope, ASTNode* current)
{
    if (current->type != VAR_DECL_NODE) return NANN;

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

int ResolveExprs(Scope* scope, ASTNode* current)
{
    if (current->type != EXPR_STMT_NODE) return NANN;
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

int ResolveStmts(Scope* scpoe, ASTNode* current)
{
    /* TODO: Have resolve stmts handle if / while / etc stmts, 
       and allow for this function to enter scope instead and set a 
       ctrl_stmt flag as well. That way we avoid FindIdentChild and 
       the EnterScopeAsNeeded bloat
    */

    /* Return Stmt and CtrlStmt probably the only ones need to check */
    if (!IsStmt(current)) return NANN;
}

int Resolve(Scope* scope, ASTNode* current)
{
    /* Since C considers the whole switch the same scope, can't redclare inside */
    Symbol* sym;
    char* name;     
    NodeType type = current->type;

    switch (type) {
        case(IDENT_NODE):   /* TODO: Maybe have this decl, and check children instead */
            if (!IdentIsDecl(current, parent)) return VALDN;    /* Definately not a var */
            name = current->token.lex.word;

            /* Ctrl Scopes don't allow for shadowing */
            if (scope->stype == CTRL_SCOPE) sym = LookupAllScopes(scope, name, N_VAR);
            else sym = LookupCurrentScope(scope, name, N_VAR);

            if (sym) return NERROR_ALREADY_DEFINED(name, current, sym->decl);
            
            sym = STPushNamespace(scope, current, N_VAR);
            PushScope(scope, sym, N_VAR);
            return VALDN;
        case(BINARY_EXPR_NODE):
        case(UNARY_EXPR_NODE):  /* Purposeful fall through, all do the same */
        case(ASGN_EXPR_NODE):
            ASTNode* exprIdent = FindIdentChild(current);

            /* Exprs don't need to use idents, continue if they don't */
            if (exprIdent) {
                name = exprIdent->token.lex.word;
                if (!LookupAllScopes(scope, name, N_VAR))
                    return NERROR_DOESNT_EXIST(name, current);
            }
            return VALDN;
        case(CALL_FUNC_NODE):
        case(ARR_INDEX_NODE):
            /* TODO: Check for function overloading here */
            ASTNode* identChild = FindIdentChild(current);
            if (identChild) {
                name = identChild->token.lex.word;
                if (!LookupAllScopes(scope, name , N_VAR))
                    return NERROR_DOESNT_EXIST(name, identChild);
            }
            return VALDN;
        default: return NANN;
    } 

    return NANN;
}

int ResolveTypes(Scope* scope, ASTNode* current)
{
    return NANN;
}