#include "NameResolver.h"

Scope* CurrentScope = NULL;


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
    return type == VAR_NODE || type == PARAM_NODE;
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
    /* TODO: Have these as a static array to iterate through */
    /* TODO: Avoid Variable shadowing in these types of nodes */
    if (node->type == FUNC_NODE)
        return FUNC_SCOPE;
    else if (IsCtrlStmt(node->type))
        return CTRL_SCOPE;

    return INVALID_SCOPE;
}


/* ----------- Name Resolution ---------- */

Symbol** ResolveNames(AST* ast) 
{
    BeginScope(&CurrentScope, PROG_SCOPE);

    if(!ResolveNamesInNode(ast->root, NULL))
        return NULL;

    ExitScope(&CurrentScope);
    return SymbolTable;
}

bool ResolveNamesInNode(ASTNode* current, ASTNode* parent) 
{
    NodeType type = current->type;
    if (type == FUNC_NODE) {
        ASTNode* funcIdent = FindIdentChild(current);
        if (!funcIdent)
            return NERROR_NO_IDENT(funcIdent);

        Symbol* sym = STPush(funcIdent);
        PushScope(&CurrentScope, sym);
    }

    ScopeType stype;
    if ((stype = GetScopeType(current)) != INVALID_SCOPE)
        BeginScope(&CurrentScope, stype);



    if (type == IDENT_NODE && IdentIsDecl(current, parent)) {
        char* name = current->token.lex.word;

        if (LookupCurrentScope(&CurrentScope, name)) 
            return NERROR_ALREADY_DEFINED(name, current, STLookup(name)->decl);
        else if (CurrentScope->stype == CTRL_SCOPE && STLookup(name))
            return NERROR_ALREADY_DEFINED(name, current, STLookup(name)->decl);
    
        Symbol* sym = STPush(current);
        PushScope(&CurrentScope, sym);
    }
    else if (type == BINARY_EXPR_NODE || type == UNARY_EXPR_NODE || type == ASGN_EXPR_NODE) {
        ASTNode* node = FindIdentChild(current);
        if (node) {                 /* Exprs don't need to use idents, continue if they don't */
            char* name = node->token.lex.word;
            if (!STLookup(name)) 
                return NERROR_DOESNT_EXIST(name, current);
        }
    }
    else if (type == CALL_FUNC_NODE || type == ARR_INDEX_NODE) {
        /* Paramaters, func name, arr name, and arr params *//* Allow Function Overloading here */
        ASTNode* node = FindIdentChild(current);
        char* name = node->token.lex.word;

        if (!STLookup(name))    
            return NERROR_DOESNT_EXIST(name, node);
    }


    /* Recursively check children */
    int i;
    for (i = 0; i < current->childCount; i++) {
        if(ResolveNamesInNode(current->children[i], current) == ERRN)
            return ERRN;
    }


    if (GetScopeType(current) != INVALID_SCOPE)
        ExitScope(&CurrentScope);
    return VALDN;
}

