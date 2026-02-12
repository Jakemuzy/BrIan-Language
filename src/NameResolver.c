#include "NameResolver.h"

/* ----------- Error Handling ---------- */


bool NERROR_NO_IDENT(ASTNode* curr)
{
    printf("NAME ERROR: No Identifier found on line '%d'\n", curr->token.line); 
    return ERRN;
}

bool NERROR_ALREADY_DEFINED(char* name, ASTNode* curr, ASTNode* first)
{
    printf("NAME ERROR: Identifier '%s' on line %d already defined, first definition on line %d\n", name, curr->token.line, first->token.line); \
    return ERRN; 
}

bool NERROR_DOESNT_EXIST(char* name, ASTNode* curr) 
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

bool IdentIsDecl(ASTNode* ident, ASTNode* parent)
{
    NodeType type = parent->type;
    return type == VAR_NODE || type == PARAM_NODE || type == STRUCT_DECL_NODE || type == ENUM_DECL_NODE;
}

bool IsCtrlStmt(NodeType type) 
{
    int i;
    for (i = 0; i < CTRL_STMTS_SIZE; i++) {
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


/* ----------- Name Resolution ---------- */

Namespaces* ResolveNames(AST* ast) 
{
    Scope* scope = ScopeInit(2, N_VAR, N_TYPE);
    scope = BeginScope(scope, PROG_SCOPE);

    if (ResolveNamesInNode(scope, ast->root, NULL) == ERRN)
        return NULL;

    scope = ExitScope(scope);
    return scope->namespaces;
}

bool ResolveNamesInNode(Scope* scope, ASTNode* current, ASTNode* parent) 
{
    if (!current) return VALDN;

    NodeType type = current->type;
    if (type == FUNC_NODE) {
        ASTNode* funcIdent = FindIdentChild(current);
        if (!funcIdent)
            return NERROR_NO_IDENT(funcIdent);

        SymbolTable* venv = GetSTfromNS(scope, N_VAR);
        Symbol* sym = STPush(venv, funcIdent);
        PushScope(scope, sym, N_VAR);
    }

    ScopeType stype;
    if ((stype = GetScopeType(current)) != INVALID_SCOPE)
        scope = BeginScope(scope, stype);


    if (type == IDENT_NODE && IdentIsDecl(current, parent)) {
        char* name = current->token.lex.word;
        SymbolTable* venv = GetSTfromNS(scope, N_VAR);

        if (LookupCurrentScope(scope, name, N_VAR)) 
            return NERROR_ALREADY_DEFINED(name, current, STLookup(venv, name)->decl);
        else if (scope->stype == CTRL_SCOPE && STLookup(venv, name))
            return NERROR_ALREADY_DEFINED(name, current, STLookup(venv, name)->decl);
    
        Symbol* sym = STPush(venv, current);
        PushScope(scope, sym, N_VAR);
    }
    else if (type == BINARY_EXPR_NODE || type == UNARY_EXPR_NODE || type == ASGN_EXPR_NODE) {
        ASTNode* node = FindIdentChild(current);
        SymbolTable* venv = GetSTfromNS(scope, N_VAR);

        if (node) {                 /* Exprs don't need to use idents, continue if they don't */
            char* name = node->token.lex.word;
            if (!STLookup(venv, name)) 
                return NERROR_DOESNT_EXIST(name, current);
        }
    }
    else if (type == CALL_FUNC_NODE || type == ARR_INDEX_NODE) {
        /* Paramaters, func name, arr name, and arr params *//* Allow Function Overloading here */
        ASTNode* node = FindIdentChild(current);
        if (node) {
            char* name = node->token.lex.word;
            SymbolTable* venv = GetSTfromNS(scope, N_VAR);

            if (!STLookup(venv, name))    
                return NERROR_DOESNT_EXIST(name, node);
        }
    }


    /* Recursively check children */
    int i;
    for (i = 0; i < current->childCount; i++) {
        if(ResolveNamesInNode(scope, current->children[i], current) == ERRN)
            return ERRN;
    }


    if (GetScopeType(current) != INVALID_SCOPE)
        scope = ExitScope(scope);
    return VALDN;
}

/* ---------- BETTER FUNCTIONS ---------- */

bool ResolveNames(Scope* scope, ASTNode* current, ASTNode* parent)
{
    if (!current) return VALDN;

    ScopeType stype;
    if ((stype = GetScopeType(current)) != INVALID_SCOPE)
        scope = BeginScope(scope, stype);

    if (CanBeVar(current->type)) {
        if (ResolveVars(scope, current, parent) == ERRN) return ERRN;
        
    }
    else if (CanBeType(current->type))
        ResolveTypes(scope, current, parent);

    /* Recursively check children */
    for (size_t i = 0; i < current->childCount; i++) {
        if (ResolveNamesInNode(scope, current->children[i], current) == ERRN) return ERRN;
    }

    if (GetScopeType(current) != INVALID_SCOPE)
        scope = ExitScope(scope);

    return VALDN;
}

bool ResolveVars(Scope* scope, ASTNode* current, ASTNode* parent)
{
    NodeType type = current->type;
    switch (type) {
        case(FUNC_NODE):
            /* TODO: Allow function overloading */
            ASTNode* funcIdent = FindIdentChild(current);
            if (!funcIdent) return NERROR_NO_IDENT(funcIdent);

            Symbol* sym = STPushNamespace(scope, funcIdent, N_VAR);
            PushScope(scope, sym, N_VAR);
            break;
        case(IDENT_NODE):
            if (!IdentIsDecl(current, parent)) return VALDN;    /* Definately not a var */
            char* name = current->token.lex.word;
            Symbol* sym = STLookupNamespace(scope, name, N_VAR);

            if (LookupCurrentScope(scope, name, N_VAR))
                return NERROR_ALREADY_DEFINED(name, current, sym->decl);
            else if (scope->stype == CTRL_SCOPE && sym)     /* Ctrl scopes don't allow shadowing */
                return NERROR_ALREADY_DEFINED(name, current, sym->decl);
            
            sym = STPushNamespace(scope, current, N_VAR);
            PushScope(scope, sym, N_VAR);
            break;
        case(BINARY_EXPR_NODE):
        case(UNARY_EXPR_NODE):  /* Purposeful fall through, all do the same */
        case(ASGN_EXPR_NODE):
            ASTNode* exprIdent = FindIdentChild(current);

            /* Exprs don't need to use idents, continue if they don't */
            if (exprIdent) {
                char* name = exprIdent->token.lex.word;
                if (!STLookupNamespace(scope, name, N_VAR))
                    return NERROR_DOESNT_EXIST(name, current);
            }
            break;
        case(CALL_FUNC_NODE):
        case(ARR_INDEX_NODE):
            /* TODO: Check for function overloading here */
            ASTNode* identChild = FindIdentChild(current);
            if (identChild) {
                char* name = identChild->token.lex.word;
                if (!STLookupNameSpace(name, current))
                    return NERROR_DOESNT_EXIST(name, identChild);
            }
            break;
    } 

    return VALDN;
}

bool ResolveTypes(Scope* scope, ASTNode* current, ASTNode* parent)
{

}