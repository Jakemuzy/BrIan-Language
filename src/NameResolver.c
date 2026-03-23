#include "NameResolver.h"

/* 
TODO:
    - Fix Nested Structs
    - URGENT: name resolver should NOT try to partially determine types
        this is ONLY the TypeCheckers job snice variables of different
        types but the same name can be shadowed. This would make it 
        extremely hard to check the type from the SymbolTable.
        SOLUTION: Just check the type in here instead.

    - STRUCT FIELD VALIDATION SHOULD BE IN TYPECHECKER 
    - Have functions be stateful as well (store their own fields namespace like structs)
    - Determine if a variable is actually defined (ie int x; int b = x + 4) x is not defined

NOTE: 
    - There is no possible way to handle name resolution inside of enums and structs without
      mixing a little bit of types in here since they are technically their own types.
      That being said, there is no type checking here, just intializing and checking if the 
      it exists in N_TYPE, not actually checking the type.
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

int NERROR(char* msg, char* name, ASTNode* curr) 
{
    printf("NAME ERROR: %s '%s' on line %d\n", msg, name, curr->token.line);
    return ERRN;
}

/* ----------- Name Resolution ---------- */

Namespaces* ResolveNames(AST* ast) 
{
    ScopeContext* scope = ScopeInit(PROG_SCOPE, 2, N_VAR, N_TYPE);

    if (ResolveEverything(scope, ast->root) != VALDN) {
        printf("NAME ERROR: failed to build\n");
        return NULL;
    }

    return scope->namespaces;
}

int ResolveEverything(ScopeContext* scope, ASTNode* current)
{
    if (!current) return VALDN;

    int status = NANN;
    switch (current->type) {
        case(VAR_DECL_NODE):
            return ResolveVars(scope, current);
        case (TYPEDEF_DECL_NODE):
            status = ResolveTypedefs(scope, current);
            break;
        case (FUNC_NODE):
            return ResolveFuncs(scope, current);   /* Return since ResolveFuncs already traverses Children */
        case (IF_STMT_NODE):
        case (DO_WHILE_STMT_NODE):  // TODO: Just put them here
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
    /* If custom type like struct, need variable to have fields knowledge */
    ASTNode* typeNode = current->children[0];
    char* typeLex = typeNode->token.lex.word;

    Symbol* sym = LookupAllScopes(scope, typeLex, N_TYPE);
    if (sym && sym->stype == S_STRUCT) 
        return ResolveStructVar(scope, current->children[1], sym);

    /* Otherwise use classic variable resolution */
    return ResolveVar(scope, current->children[1]);   
}

int ResolveVar(ScopeContext* scope, ASTNode* current)
{
    /* TODO: Find the underlying cause for this null check being required */
    if (!current) return NANN;

    if (current->type == VAR_NODE) {
        ASTNode* identNode = current->children[0];
        char* name = identNode->token.lex.word;
        
        // Resolve initializer first
        if (current->childCount > 1) {
            int status = ResolveVar(scope, current->children[1]);
            if (status == ERRN) return status;
        }
        
        // Register the variable
        Symbol* sym = LookupCurrentScope(scope, name, N_VAR);
        if (sym) return NERROR_ALREADY_DEFINED(name, identNode, sym->decl);
        sym = STPushNamespace(scope, identNode, N_VAR, S_VAR);
        sym->stype = S_VAR;
        PushScope(scope, sym, N_VAR);

        return VALDN;
    }
    else if (current->type == BINARY_EXPR_NODE || current->type == UNARY_EXPR_NODE) {
        return ResolveExprs(scope, current);
    }
    else if (current->type == CALL_FUNC_NODE) {
        return ResolveFuncCall(scope, current);
    }
    else if (current->type == MEMBER_ACCESS_NODE) {
        Namespaces* nestedMembers = NULL;
        return ResolveMemberAccess(scope, current, &nestedMembers);
    }
    else {
        for (size_t i = 0; i < current->childCount; i++) {
            int status = ResolveVar(scope, current->children[i]);
            if (status == ERRN) return status;
        }
    }
    return VALDN;
}

int ResolveStructVar(ScopeContext* scope, ASTNode* current, Symbol* structSym)
{
    if (!current) return NANN;

    if (current->type == IDENT_NODE) {
        Symbol* sym;
        char* name = current->token.lex.word;

        sym = LookupCurrentScope(scope, name, N_VAR);
        if (sym) return NERROR_ALREADY_DEFINED(name, current, sym->decl);
     
        sym = STPushNamespace(scope, current, N_VAR, S_VAR);
        sym->stype = S_VAR;
        sym->fieldCount = structSym->fieldCount;    /* TODO: unsure if this is safe since the same pointer */
        sym->fields = structSym->fields;        // Copy the fields so the var has access to its own fields
        PushScope(scope, sym, N_VAR);
        return VALDN;
    }    
    else if (current->type == STRUCT_INIT_NODE) { /* TODO: not implemented yet */ 
        /* Check field count matches, type checker will resolve types */
    }
    else {
        for (size_t i = 0; i < current->childCount; i++) {
            int status = ResolveStructVar(scope, current->children[i], structSym);
            if (status == ERRN) return status;
        }
    }
    return VALDN;
}

int ResolveFuncs(ScopeContext* scope, ASTNode* current)
{
    /* Function Ident and Scope */
    ASTNode* identNode = current->children[1];
    char* name = identNode->token.lex.word;

    /* Current since functions in structs can shadow */
    Symbol* sym = LookupCurrentScope(scope, name, N_VAR); 
    if (sym) return NERROR_ALREADY_DEFINED(name, identNode, sym->decl);

    sym = STPushNamespace(scope, identNode, N_VAR, S_FUNC);
    if (!sym) printf("INVALID SYM\n");
    PushScope(scope, sym, N_VAR);

    /* Param list has own scope, stored as fields of func sym */
    BeginPersistentScope(&scope, PARAM_SCOPE);

    int status = ResolveParams(scope, current->children[2], &sym->fieldCount);
    if (status == ERRN) return status;

    /* Body Calls Resolve Vars */
    BeginScope(&scope, FUNC_SCOPE);
    status = ResolveEverything(scope, current->children[3]);

    ExitScope(&scope);  // Func Scope
    ExitPersistentScope(&scope);  // Param Scope

    /* SET AFTER EXIT SINCE REALLOC IS USED */
    sym->fields = scope->namespaces;
    return status;
}

/* Returns how many params there are */
int ResolveParams(ScopeContext* scope, ASTNode* current, size_t* paramCount)
{
    if (current->type != PARAM_LIST_NODE) return NANN;

    for (size_t i = 0; i < current->childCount; i++) {
        int status = ResolveParam(scope, current->children[i]);
        if (status != VALDN) return status;

        (*paramCount)++;
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

    /* S_VAR is not guaranteed, could be a func or another type, so determine */
    sym = STPushNamespace(scope, identNode, N_VAR, DetermineSymType(current));
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
        Namespaces* nestedMembers = NULL;
        return ResolveMemberAccess(scope, current, &nestedMembers);
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
    BeginScope(&scope, PARAM_SCOPE);

    /* TODO: enter scope for arguments, then another for the body */
    ASTNode* declExprList = current->children[0];
    if (declExprList->type == VAR_DECL_NODE) {
        if(ResolveVars(scope, declExprList) == ERRN) return ERRN;
    } else {
        if (ResolveExprs(scope, declExprList) == ERRN) return ERRN; 
    }

    ASTNode* exprNode = current->children[1];
    if (ResolveExprs(scope, exprNode) == ERRN) return ERRN;

    ASTNode* exprListNode = current->children[2];
    if (ResolveExprs(scope, exprListNode) == ERRN) return ERRN;

    BeginScope(&scope, CTRL_SCOPE);
    ASTNode* forBody = current->children[3];
    if (ResolveEverything(scope, forBody) == ERRN) return ERRN;

    ExitScope(&scope); /* Ctrl then Param */
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
    /* TODO: sym-fields should be after exit persistent scope, however 
       it currently breaks, this is because symbols in the persistent namespace gets realloced
       which I eventually have to access the symbols directly for type checking. currently this 
       seg faults, but I will have to fix this later 
    */
    /* Pushes Struct to scope */
    ASTNode* identNode = current->children[0];

    Symbol* sym;   
    char* name = identNode->token.lex.word;

    sym = LookupCurrentScope(scope, name, N_TYPE);
    if (sym) return NERROR_ALREADY_DEFINED(name, identNode, sym->decl);

    /* TYPE* will become TY_STRUCT during type checking, NULL for now */
    sym = STPushNamespace(scope, identNode, N_TYPE, S_STRUCT);
    PushScope(scope, sym, N_TYPE);

    /* Stores namespace that resolves struct members inside of struct */
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
    char* name = identNode->token.lex.word;

    sym = LookupCurrentScope(scope, name, N_TYPE);
    if (sym) return NERROR_ALREADY_DEFINED(name, identNode, sym->decl);

    /* TYPE* will become TY_ENUM during type checking, NULL for now */
    sym = STPushNamespace(scope, identNode, N_TYPE, S_ENUM);
    PushScope(scope, sym, N_TYPE);

    /* Enum's Body Namespace */
    BeginPersistentScope(&scope, ENUM_SCOPE);
    sym->fields = scope->namespaces;

    ASTNode* enumBody = current->children[1];
    for (size_t i = 0; i < enumBody->childCount; i++) {
        ASTNode* memberNode = enumBody->children[i];
        char* memName = memberNode->token.lex.word;

        Symbol* memSym = LookupCurrentScope(scope, memName, N_VAR);
        if (memSym) return NERROR_ALREADY_DEFINED(memName, memberNode, memSym->decl);

        memSym = STPushNamespace(scope, memberNode, N_VAR, S_VAR);
        PushScope(scope, memSym, N_VAR);
    }

    ExitPersistentScope(&scope);
    return VALDN;
}

int ResolveTypedefs(ScopeContext* scope, ASTNode* current)
{
    /* Not sure if this should be in name resolver */
    ASTNode* newTypeNode = current->children[1];
    char* newTypeLex = newTypeNode->token.lex.word;

    Symbol* sym = LookupCurrentScope(scope, newTypeLex, N_TYPE);
    if (sym) return NERROR_ALREADY_DEFINED(newTypeLex, newTypeNode, sym->decl);

    /* TYPE* will become TY_NAME during type checking, NULL for now */
    sym = STPushNamespace(scope, newTypeNode, N_TYPE, S_TYPEDEF);
    PushScope(scope, sym, N_TYPE); 

    /* Resolve Typedefs */
    return NANN;
}

/* ---------- Etc ---------- */

int ResolveFuncCall(ScopeContext* scope, ASTNode* current)
{
    /* TODO: This breaks recursion */
    ASTNode* identNode = current->children[0];
    char* identLex = identNode->token.lex.word;

    Symbol* sym = LookupAllScopes(scope, identLex, N_VAR);
    if (!sym)
        return NERROR_DOESNT_EXIST(identLex, identNode);
    else if (sym->stype != S_FUNC)
        return NERROR("Attempting to call a non function identifier", identLex, identNode);

    /* Ensure arg count matches */
    ASTNode* argListNode = current->children[1];
    if (sym->fieldCount != GetTotalArgCount(argListNode)) 
        return NERROR("Mismatch in argument count", identLex, identNode);

    for (size_t i = 0; i < argListNode->childCount; i++) {
        ASTNode* argNode = argListNode->children[i];
        int status = ResolveFuncArg(scope, argNode);
        if (status != VALDN)
            return status;
    }
    return VALDN;
}

int ResolveFuncArg(ScopeContext* scope, ASTNode* current)
{
    /* TODO: Resolve for func calls and array index, etc */
    if (current->type == LITERAL_NODE)
        return VALDN;
    else if (current->type == IDENT_NODE) {
        char* identLex = current->token.lex.word;
        Symbol* sym = LookupCurrentScope(scope, identLex, N_VAR);
        if (!sym) 
            return NERROR_DOESNT_EXIST(identLex, current);

        return VALDN;
    }
    return NANN;
}

int ResolveMemberAccess(ScopeContext* scope, ASTNode* current, Namespaces** nestedMembers) 
{
    ASTNode* identNode = current->children[0];
    char* identLex = identNode->token.lex.word; 

    Namespaces* memberFields; 

    // Handles Nested Member Access
    if (identNode->type == MEMBER_ACCESS_NODE) {
        if (ResolveMemberAccess(scope, identNode, &memberFields) != VALDN) 
            return ERRN;
    }
    else  {
        Symbol* sym = LookupAllScopes(scope, identLex, N_VAR);
        if (!sym)
            return NERROR_DOESNT_EXIST(identLex, identNode);
        memberFields = sym->fields;
    }
 
    /* 
        TODO: This check isn't thorough enough, this would allow functions and enums
        which could actually be a cool feature of the language. Debate on this later 
    */
    if (!memberFields)  
        return NERROR("Identifier is not a struct", identLex, identNode);

    ASTNode* memberNode = current->children[1];
    char* memberLex = memberNode->token.lex.word;
    /* TODO: Check N_TYPE if not in N_VAR, etc */
    Symbol* memSym = STLookupNamespace(memberFields, memberLex, N_VAR);

    if (!memSym)
        return NERROR_DOESNT_EXIST(memberLex, memberNode);

    // For name resolution phase: done. We assume later type checking handles the rest
    *nestedMembers = memberFields;
    return VALDN;
}

int ResolveArrIndex(ScopeContext* scope, ASTNode* current)
{
    return NANN;
}


/* ---------- Helpers ---------- */

SymbolType DetermineSymType(ASTNode* node) 
{
    switch(node->type) {
        case(VAR_DECL_NODE): return S_VAR;
        /* Check parent for type? */
        case(FUNC_NODE) : return S_FUNC;
        case(ARR_INDEX_NODE) : return S_INDEX;
        case(CALL_FUNC_NODE) : return S_CALL;
        case(TYPEDEF_DECL_NODE) : return S_TYPEDEF;
        case(STRUCT_DECL_NODE) : return S_STRUCT;
        case(ENUM_DECL_NODE) : return S_ENUM;

        /* Fall thorugh for speicifc Ctrl Scopes */
        case(IF_NODE): case(ELIF_NODE): case(ELSE_NODE):
        case(DO_WHILE_STMT_NODE): case(WHILE_STMT_NODE):
        case(SWITCH_STMT_NODE): case(FOR_STMT_NODE): return S_CTRL;
    }
    return S_ERROR; 
}