#include "Parser.h"

/* ----------- HELPER ---------- */

int ValidTokType(const int types[], int arrSize, int type)
{
    int i;
    for(i = 0; i < arrSize; i++)
    {
        if(types[i] == type)
            return VALID;
    }
    return NAP;
}

/* Reads current token, displays error message and returns error type if not valid */ 
int CompareToken(FILE* fptr, TokenType desired, ParseError errType, char* errMessage)
{
    Token current = GetNextToken(fptr);
    if(current.type != desired)
    {   
        if(errType == ERRP)
            ERROR_MESSAGE(errMessage, NULL);
        else if (errType == NAP) 
            PutTokenBack(&current);
        return errType;
    }
   
    return VALID;
}

bool ParseOperatorToken(FILE* fptr, TokenType desired, ASTNode** resultNode)
{
    Token current = GetNextToken(fptr);
    if(current.type != desired)
    {
        PutTokenBack(&current);
        return false;
    }

    ASTNode* tokNode = InitASTNode();
    ASTMakeTokNode(tokNode, current);
    *resultNode = tokNode;

    return true;
}

/* ---------- EBNF ---------- */

AST* Program(FILE* fptr)
{
    /* TODO: errors regarding ast freeing and progNode freeing */
    AST* ast = ASTInit();
    ASTNode* progNode = ast->root;
    
    while(true)
    {
        if (PeekNextTokenP(fptr) == END)
            return ast;

        ASTNode* funcNode = Function(fptr);
        if(PARSE_ERROR == VALID) {
            ASTPushChildNode(progNode, funcNode, FUNC_NODE); 
            continue;
        }
        else if (PARSE_ERROR == ERRP) 
            return ERROR_MESSAGE("Invalid Function in Program", 1, funcNode);

        ASTNode* declStmtNode = DeclStmt(fptr);
        if(PARSE_ERROR == VALID) {
            ASTPushChildNode(progNode, declStmtNode, DECL_STMT_NODE);
            continue;
        }
        else if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid global DeclStmt in Program", 2, declStmtNode, funcNode);
        
        break;
    }

    return ast;
}

ASTNode* Function(FILE* fptr)
{
    ASTNode* typeNode = Type(fptr);
    if (!typeNode)
    {
        if (PARSE_ERROR != VALID)
            return ERROR_MESSAGE("Invalid Type in Function", 0);
        return PARSE_FAIL(NAP);
    }

    /* TODO: add function name to the ast */
    if(CompareToken(fptr, IDENT, "Function does not have a name", ERRP) != VALID) {
        ASTFreeNodes(1, typeNode);
        return PARSE_FAIL(ERRP);
    }

    if(CompareToken(fptr, LPAREN, "Missing left parenthesis in function", ERRP) != VALID) {
        ASTFreeNodes(1, typeNode);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* paramListNode = ParamListNode(fptr);
    if (!paramListNode) 
        return ERROR_MESSAGE("Invalid ParamList in function", 0);

    if(CompareToken(fptr, RPAREN, "Missing right parenthesis in function", ERRP) != VALID) {
        ASTFreeNodes(1, paramListNode);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* bodyNode = Body(fptr);
    if(!bodyNode) 
        return ERROR_MESSAGE("Invalid Body in Function", 1, paramListNode);
        
    ASTNode* funcNode = InitASTNode();
    ASTPushChildNode(funcNode, typeNode, TYPE_NODE);
    ASTPushChildNode(funcNode, paramListNode, PARAM_LIST_NODE);
    ASTPushChildNode(funcNode, bodyNode, BODY_NODE);
    return funcNode;
}

ASTNode* ParamList(FILE* fptr)
{
    ASTNode* paramNode = Param(fptr);
    if(!paramNode) {
        if (PARSE_ERROR != VALID)
            return ERROR_MESSAGE("Invalid Param in ParamList", 0);
        return PARSE_FAIL(NAP);
    }

    ASTNode* paramListNode = InitASTNode();
    ASTPushChildNode(paramListNode, paramNode, PARAM_NODE);

    while(true)
    {
        if(PeekNextTokenP(fptr) != COMMA)
            break;
        GetNextTokenP(fptr);

        paramNode = Param(fptr);
        if(!paramNode) {
            ERROR_MESSAGE("Invalid Param in ParamList", 1, paramListNode);
            return PARSE_FAIL(ERRP);
        }
        ASTPushChildNode(paramListNode, paramNode, PARAM_NODE);
    }

    return paramListNode;
}