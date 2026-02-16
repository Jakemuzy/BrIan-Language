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

bool IdentIsDecl(ASTNode* ident, ASTNode* parent)
{
    NodeType type = parent->type;
    return type == VAR_NODE || type == PARAM_NODE || type == STRUCT_DECL_NODE || type == ENUM_DECL_NODE;
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

/* ---------- BETTER FUNCTIONS ---------- */

bool CanBeVar(NodeType type) { return true; }
bool CanBeType(NodeType type) { return true;}

int ResolveEverything(Scope* scope, ASTNode* current, ASTNode* parent)
{
    if (!current) return VALDN;

    if (EnterScopeIfNeeded(&scope, current, parent) == ERRN)
        return ERRN;

    do {
        if (CanBeVar(current->type)) {
            int status = ResolveVars(scope, current, parent);

            if (status == ERRN) return ERRN;
            else if (status == NAN) continue;   /* Continue since Ident can be Var or Type */
            else break;
        }
        if (CanBeType(current->type)) {
            int status = ResolveTypes(scope, current, parent);

            if (status == ERRN) return ERRN;
            else if (status == NAN) continue;   /* Continue since Ident can be Var or Type */
            else break;
        }
    } while (0);

    /* Recursively check children */
    for (size_t i = 0; i < current->childCount; i++) {
        if (ResolveEverything(scope, current->children[i], current) == ERRN) return ERRN;
    }

    if (GetScopeType(current) != INVALID_SCOPE)
        scope = ExitScope(scope);

    return VALDN;
}

int EnterScopeIfNeeded(Scope** scope, ASTNode* current, ASTNode* parent)
{
    /* Enters new scope and pushes visible names to outer scope */
    ScopeType stype;
    if ((stype = GetScopeType(current)) != INVALID_SCOPE) {
        /* TODO: make this a switch, */
        if (stype == FUNC_SCOPE) {
            /* TODO: Allow function overloading */
            ASTNode* funcIdent = FindIdentChild(current);
            if (!funcIdent) return NERROR_NO_IDENT(funcIdent);

            Symbol* sym = STPushNamespace(*scope, funcIdent, N_VAR);
            PushScope(*scope, sym, N_VAR);
        }

        *scope = BeginScope(*scope, stype);
    }
    return VALDN;
}

int ResolveVars(Scope* scope, ASTNode* current, ASTNode* parent)
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
        default: return NAN;
    } 

    return NAN;
}

int ResolveTypes(Scope* scope, ASTNode* current, ASTNode* parent)
{
    printf("Resolving types %s\n", current->token.lex.word);
    return NAN;
}