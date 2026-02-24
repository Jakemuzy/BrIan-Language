#include "NameResolver.h"

/* TODO:
    - Allow Shadowing in ctrl scopes 
    - Fix Nested Structs
*/

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
    printf("NAME ERROR: '%s' on line %d is an undefined IDENTIFIER\n", name, curr->token.line);
    return ERRN;
}

int NERROR_UNDEFINED_TYPE(char* name, ASTNode* curr) 
{
    printf("NAME ERROR: '%s' on line %d is an undefined TYPE\n", name, curr->token.line);
    return ERRN;
}

int NERROR(char* msg, char* name, ASTNode* curr) 
{
    printf("NAME ERROR: %s '%s' on line %d\n", msg, name, curr->token.line);
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

    //if (strcmp(name, "struct") == 0)  return TY_STRUCT();
    if (strcmp(name, "null") == 0)    return TY_NULL();

    // If it wasn’t a builtin, treat as named type
    return NULL; /* This will be resolved in type resolution */
}

/* ----------- Name Resolution ---------- */

Namespaces* ResolveNames(AST* ast) 
{
    ScopeContext* scope = ScopeInit(PROG_SCOPE, 2, N_VAR, N_TYPE);

    if (ResolveEverything(scope, ast->root) != VALDN)
        return NULL;

    return scope->namespaces;
}

int ResolveEverything(ScopeContext* scope, ASTNode* current)
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
            return ResolveFuncs(scope, current);   /* Return since ResolveFuncs already traverses Children */
        case (IF_STMT_NODE):
        case (DO_WHILE_STMT_NODE):
        case (WHILE_STMT_NODE):
        case (SWITCH_STMT_NODE):
        case (RETURN_STMT_NODE):
        case (FOR_STMT_NODE):
            return ResolveStmts(scope, current);
        case (EXPR_STMT_NODE):
            status = ResolveExprs(scope, current);
            break;
        case (CALL_FUNC_NODE):
            status = ResolveFuncCall(scope, current);
            break;
        case (STRUCT_DECL_NODE):
            return ResolveStructDecl(scope, current);
            break;
        case (ENUM_DECL_NODE):
            status = ResolveEnums(scope, current);
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

int ResolveVars(ScopeContext* scope, ASTNode* current)
{
    char* typeLex = current->children[0]->token.lex.word;
    TYPE* type = StringToType(typeLex);

    if (!type) {
        Symbol* sym = LookupAllScopes(scope, typeLex, N_TYPE);

        if (!sym) 
            return NERROR_UNDEFINED_TYPE(typeLex, current->children[0]);

        /* TODO: ERROR needs to have var store typeName */

        type = TY_NAME(sym, NULL);
    }

    /* TODO: Should pass sym, so in case of enum or struct, can check members */
    return ResolveVar(scope, current->children[1], type, typeLex);   
}

int ResolveVar(ScopeContext* scope, ASTNode* current, TYPE* type, char* typeLex)
{
    /* TODO: Find the underlying cause for this null check being required */
    if (!current) return NANN;

    if (current->type == IDENT_NODE) {
        Symbol* sym;
        char* name = strdup(current->token.lex.word);

        sym = LookupCurrentScope(scope, name, N_VAR);
        if (sym) return NERROR_ALREADY_DEFINED(name, current, sym->decl);
      
        sym = STPushNamespace(scope, current, N_VAR, type, typeLex);
        sym->stype = S_VAR;
        PushScope(scope, sym, N_VAR);
        return VALDN;
    }    
    else {
        for (size_t i = 0; i < current->childCount; i++) {
            int status = ResolveVar(scope, current->children[i], type, typeLex);
            if (status == ERRN) return status;
        }
    }
    return VALDN;
}

int ResolveFuncs(ScopeContext* scope, ASTNode* current)
{
    /* Function Ident, Type and Scope */
    char* typeLex = current->children[0]->token.lex.word;
    TYPE* type = StringToType(typeLex);

    ASTNode* identNode = current->children[1];
    char* name = strdup(identNode->token.lex.word);

    /* Current since functions in structs can shadow */
    Symbol* sym = LookupCurrentScope(scope, name, N_VAR); 
    if (sym) return NERROR_ALREADY_DEFINED(name, identNode, sym->decl);

    sym = STPushNamespace(scope, identNode, N_VAR, type, typeLex);
    if (!sym) printf("INVALID SYM\n");
    PushScope(scope, sym, N_VAR);
    BeginScope(&scope, FUNC_SCOPE);

    /* Param List */
    int status = ResolveParams(scope, current->children[2]);
    if (status == ERRN) return status;

    /* Body Calls Resolve Var */
    status = ResolveEverything(scope, current->children[3]);
    ExitScope(&scope);
    return status;
}

int ResolveParams(ScopeContext* scope, ASTNode* current)
{
    if (current->type != PARAM_LIST_NODE) return NANN;

    for (size_t i = 0; i < current->childCount; i++) {
        int status = ResolveParam(scope, current->children[i]);
        if (status != VALDN) return status;
    }
    return VALDN;
}

int ResolveParam(ScopeContext* scope, ASTNode* current) 
{
    if (current->type != PARAM_NODE) return NANN;

    /* Still need to lookup because params can be duplicate */
    ASTNode* identNode = current->children[1];

    Symbol* sym = LookupCurrentScope(scope, identNode->token.lex.word, N_VAR);
    if (sym) return NERROR_ALREADY_DEFINED(identNode->token.lex.word, identNode, sym->decl);

    /* TODO: Push to Func's personal Namespace */
    char* typeLex = current->children[0]->token.lex.word;
    TYPE* type = StringToType(typeLex);
    sym = STPushNamespace(scope, identNode, N_VAR, type, typeLex);
    PushScope(scope, sym, N_VAR);

    return VALDN;
}

int ResolveExprs(ScopeContext* scope, ASTNode* current)
{
    return ResolveExpr(scope, current);
}

int ResolveExpr(ScopeContext* scope, ASTNode* current)
{
    if (current->type == IDENT_NODE) {
        char* name = current->token.lex.word;
        if (!LookupAllScopes(scope, name, N_VAR))
            return NERROR_DOESNT_EXIST(name, current);
        return VALDN;
    }
    else if (current->type == MEMBER_ACCESS_NODE) {
        Symbol* resolvedSym = NULL;
        return ResolveMemberAccess(scope, current, &resolvedSym);
    }
    else {
        for (size_t i = 0; i < current->childCount; i++) {
            int status = ResolveExpr(scope, current->children[i]);
            if (status == ERRN) return status;  /* Need to have better error handling messages */
        }
    }
    return VALDN;
}

/* ---------- Statements ---------- */

int ResolveStmts(ScopeContext* scope, ASTNode* current)
{
    /* Process paramaters then once hit body call ResolveEverything */
    /* Each Function will handle their own scope */
    switch(current->type) {
        case (IF_STMT_NODE):
            return ResolveIfStmt(scope, current);
        case (DO_WHILE_STMT_NODE):
            return ResolveDoWhileStmt(scope, current);
        case (WHILE_STMT_NODE):
            return ResolveWhileStmt(scope, current);
        case (SWITCH_STMT_NODE):
            return ResolveSwitchStmt(scope, current);
        case (FOR_STMT_NODE):
            return ResolveForStmt(scope, current);
        case (RETURN_STMT_NODE):
            return ResolveReturnStmt(scope, current);  
        default: 
            break;
    }

    return NANN;
}

int ResolveIfStmt(ScopeContext* scope, ASTNode* current) 
{
    /* TODO: could probably benefit from an ResolveIfElifStmt and ResolveElseStmt */
    for (size_t i = 0; i < current->childCount; i++) {
        if (current->children[i]->type == ELSE_NODE) {
            BeginScope(&scope, CTRL_SCOPE);

            if (ResolveEverything(scope, current->children[0]) == ERRN) return ERRN; /* Body */

            ExitScope(&scope);
            continue;
        }

        /* Otherwise if and elif have same format */
        ASTNode* ifElifNode = current->children[i];
        ASTNode* ifElifExpr = ifElifNode->children[0];
        if (ResolveExprs(scope, ifElifExpr) == ERRN) return ERRN;  

        BeginScope(&scope, CTRL_SCOPE);

        ASTNode* ifElifBody = ifElifNode->children[1];
        if (ResolveEverything(scope, ifElifBody) == ERRN) return ERRN; 

        ExitScope(&scope);
    }   

    return VALDN;
}

int ResolveDoWhileStmt(ScopeContext* scope, ASTNode* current)
{
    BeginScope(&scope, CTRL_SCOPE);

    ASTNode* doBody = current->children[0];
    if (ResolveEverything(scope, doBody) == ERRN) return ERRN;

    ExitScope(&scope);

    ASTNode* whileExpr = current->children[1];
    if (ResolveExpr(scope, whileExpr) == ERRN) return ERRN;
}

int ResolveWhileStmt(ScopeContext* scope, ASTNode* current) 
{
    ASTNode* whileExpr = current->children[0];
    if (ResolveExpr(scope, whileExpr) == ERRN) return ERRN;

    BeginScope(&scope, CTRL_SCOPE);

    ASTNode* whileBody = current->children[1];
    if (ResolveEverything(scope, whileBody) == ERRN) return ERRN;

    ExitScope(&scope);
}

int ResolveSwitchStmt(ScopeContext* scope, ASTNode* current)
{
    ASTNode* valueNode = current->children[0];
    if (ResolveExpr(scope, valueNode) != VALDN) return ERRN;

    for (size_t i = 1; i < current->childCount; i++) {
        ASTNode* caseNode = current->children[i];

        if (caseNode->type == DEFAULT_NODE) {
            BeginScope(&scope, CTRL_SCOPE);

            ASTNode* caseBody = caseNode->children[0];
            if (ResolveEverything(scope, caseBody) == ERRN) return ERRN;

            ExitScope(&scope);
            continue;
        }
        
        ASTNode* valueNode2 = caseNode->children[0];
        if (ResolveExpr(scope, valueNode2) != VALDN) return ERRN;

        BeginScope(&scope, CTRL_SCOPE);

        ASTNode* caseBody = caseNode->children[1];
        if (ResolveEverything(scope, caseBody) == ERRN) return ERRN;

        ExitScope(&scope);
    }

    return VALDN;
}

int ResolveForStmt(ScopeContext* scope, ASTNode* current)
{
    /* Scope begins early since for stmts can declare variables in their exprs */
    BeginScope(&scope, CTRL_SCOPE);

    /* TODO: Need to allow decls later one I change the grammar to accept them */
    ASTNode* declExprList = current->children[0];
    if (ResolveExprs(scope, declExprList) == ERRN) return ERRN; 

    ASTNode* exprNode = current->children[1];
    if (ResolveExprs(scope, exprNode) == ERRN) return ERRN;

    ASTNode* exprListNode = current->children[2];
    if (ResolveExprs(scope, exprListNode) == ERRN) return ERRN;

    ASTNode* forBody = current->children[3];
    if (ResolveEverything(scope, forBody) == ERRN) return ERRN;

    ExitScope(&scope);
    return VALDN;
}

int ResolveReturnStmt(ScopeContext* scope, ASTNode* current)
{
    ASTNode* returnExpr = current->children[0];
    if (ResolveExprs(scope, returnExpr) == ERRN) return ERRN;

    return VALDN;
}

/* ---------- Custom Types ---------- */

int ResolveStructDecl(ScopeContext* scope, ASTNode* current)
{
    /* Pushes Struct to scope */
    ASTNode* identNode = current->children[0];

    Symbol* sym;   
    char* name = strdup(identNode->token.lex.word);

    sym = LookupCurrentScope(scope, name, N_TYPE);
    if (sym) return NERROR_ALREADY_DEFINED(name, identNode, sym->decl);

    /* TYPE* will become TY_STRUCT during type checking, NULL for now */
    sym = STPushNamespace(scope, identNode, N_TYPE, NULL, name);
    sym->stype = S_STRUCT;
    sym->typeName = name;
    PushScope(scope, sym, N_TYPE);

    /* Stores namespae that resolves struct members inside of struct */
    BeginPersistentScope(&scope, STRUCT_SCOPE);
    sym->fields = scope->namespaces;

    // Can Nest Enum / Structs  
    // Fields CAN Shadow
    ASTNode* structBodyNode = current->children[1];
    for (size_t i = 0; i < structBodyNode->childCount; i++) {

        ASTNode* memberNode = structBodyNode->children[i];
        if (memberNode->type == VAR_DECL_NODE) {
            if (ResolveVars(scope, memberNode) == ERRN) return ERRN;
        }
        else if (memberNode->type == FUNC_NODE) {
            if (ResolveFuncs(scope, memberNode) == ERRN) return ERRN;
        }
    }

    ExitPersistentScope(&scope);
    return VALDN;
}

int ResolveEnums(ScopeContext* scope, ASTNode* current)
{
    /* Don/t allow shadowing */
    ASTNode* identNode = current->children[0];

    Symbol* sym;   
    char* name = strdup(identNode->token.lex.word);

    sym = LookupCurrentScope(scope, name, N_TYPE);
    if (sym) return NERROR_ALREADY_DEFINED(name, identNode, sym->decl);

    /* TYPE* will become TY_ENUM during type checking, NULL for now */
    sym = STPushNamespace(scope, identNode, N_TYPE, NULL, name);
    sym->stype = S_ENUM;
    PushScope(scope, sym, N_TYPE);

    /* Enum's Body Namespace */
    BeginPersistentScope(&scope, ENUM_SCOPE);
    sym->fields = scope->namespaces;

    ASTNode* enumBody = current->children[1];
    for (size_t i = 0; i < enumBody->childCount; i++) {
        ASTNode* memberNode = enumBody->children[i];
        char* memName = strdup(memberNode->token.lex.word);

        Symbol* memSym = LookupCurrentScope(scope, memName, N_VAR);
        if (memSym) return NERROR_ALREADY_DEFINED(memName, memberNode, memSym->decl);

        memSym = STPushNamespace(scope, memberNode, N_VAR, TY_INT(), memName);
        PushScope(scope, memSym, N_VAR);
    }

    ExitPersistentScope(&scope);
    return VALDN;
}

int ResolveTypedefs(ScopeContext* scope, ASTNode* current)
{
    return NANN;
}

/* ---------- Etc ---------- */

int ResolveFuncCall(ScopeContext* scope, ASTNode* current)
{
    return NANN;
}

int ResolveMemberAccess(ScopeContext* scope, ASTNode* current, Symbol** resolvedSym) 
{
    /* TODO: Not too sure how solid this 'resolvedSym' pattern is */
    Symbol* sym = NULL;
    ASTNode* accessedNode = current->children[0];
    int status;

    if (accessedNode->type == IDENT_NODE) {
        sym = LookupAllScopes(scope, accessedNode->token.lex.word, N_VAR);
        if (!sym)
            return NERROR_DOESNT_EXIST(accessedNode->token.lex.word, accessedNode);
    } else if (accessedNode->type == MEMBER_ACCESS_NODE) {
        status = ResolveMemberAccess(scope, accessedNode, resolvedSym);
        if (status != VALDN) return status; 
        sym = *resolvedSym; 
    } else {
        status = ResolveExprs(scope, accessedNode);
        if (status != VALDN) return status;

        /* Resolved Sym in case of Nested member accesses */
        sym = *resolvedSym; 
        if (!sym)
            return NERROR("Cannot resolve member base", NULL, accessedNode);
    }

    if (!sym->typeName)
        return NERROR("Type not defined for struct", sym->name, accessedNode);

    // Member lookup
    char* memName = current->children[1]->token.lex.word;
    Symbol* structNode = LookupAllScopes(scope, sym->typeName, N_TYPE);
    if (!structNode)
        return NERROR("Struct type not found", sym->typeName, accessedNode);

    Namespace* ns = GetNamespace(structNode->fields, N_VAR);
    Symbol* memberNode = NULL;
    if (ns) memberNode = LookupNamespaceCurrentScope(ns, memName);

    if (!memberNode)
        return NERROR("Undefined struct member", memName, current->children[1]);

    *resolvedSym = memberNode; // store resolved symbol for later passes
    return VALDN;
}

int ResolveArrIndex(ScopeContext* scope, ASTNode* current)
{
    return NANN;
}