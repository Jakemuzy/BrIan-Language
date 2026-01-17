#include "Parser.h"

/* TODO: 
        - GetNextTokenP after PeekNextTokenP might be dangerous, but since the 
          Preprocessor only skips for now it is fine
        - 
*/

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
    if (CompareToken(fptr, IDENT, "Function does not have a name", ERRP) != VALID) {
        ASTFreeNodes(1, typeNode);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, LPAREN, "Missing left parenthesis in function", ERRP) != VALID) {
        ASTFreeNodes(1, typeNode);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* paramListNode = ParamListNode(fptr);
    if (!paramListNode) 
        return ERROR_MESSAGE("Invalid ParamList in function", 0);

    if (CompareToken(fptr, RPAREN, "Missing right parenthesis in function", ERRP) != VALID) {
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
    if (!paramNode) {
        if (PARSE_ERROR != VALID)
            return ERROR_MESSAGE("Invalid Param in ParamList", 0);
        return PARSE_FAIL(NAP);
    }

    ASTNode* paramListNode = InitASTNode();
    ASTPushChildNode(paramListNode, paramNode, PARAM_NODE);

    while(true)
    {
        if (PeekNextTokenP(fptr) != COMMA)
            break;
        GetNextTokenP(fptr);

        paramNode = Param(fptr);
        if (!paramNode) {
            ERROR_MESSAGE("Invalid Param in ParamList", 1, paramListNode);
            return PARSE_FAIL(ERRP);
        }
        ASTPushChildNode(paramListNode, paramNode, PARAM_NODE);
    }

    return paramListNode;
}

ASTNode* Param(FILE* fptr)
{
    ASTNode* typeNode = Type(fptr);
    if (!typeNode) {
        if(PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid Param", 0);
        return PARSE_FAIL(NAP);
    }
    /* TODO: Add name as a child node */ 
    if (CompareToken(fptr, IDENT, "Param doesn't have a name", ERRP) != VALID)  {
        ASTFreeNodes(1, typeNode);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* paramNode = InitASTNode();
    ASTPushChildNode(paramNode, typeNode, TYPE_NODE);
    return paramNode;
}

/* ---------- Statements ----------- */

ASTNode* Body(FILE* fptr)
{
    if (CompareToken(fptr, LBRACK, "", NAP) != VALID) /* TODO: replace with Peek */
        return PARSE_FAIL(NAP);

    ASTNode* stmtListNode = StmtList(fptr);
    if (!stmtListNode){
        ERROR_MESSAGE("Invalid StmtList in Body", 0);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, RBRACK, "Missing RBRACK in Body", NAP) != VALID) {
        ASTFreeNodes(1, stmtListNode);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* bodyNode = InitASTNode();
    ASTPushChildNode(bodyNode, stmtListNode, STMT_LIST_NODE);
    return bodyNode;
}

ASTNode* StmtList(FILE* fptr)
{
    ASTNode* stmtListNode = InitASTNode();

    while (true) {
        ASTNode* stmtNode = Stmt(fptr);
        if (stmtNode) {
            ASTPushChildNode(stmtListNode, stmtNode, STMT_NODE);
            continue;
        }

        if (PARSE_ERROR == NAP)
            break;

        return ERROR_MESSAGE("Invalid Stmt in StmtList", stmtListNode);
    }

    return stmtListNode;
}

ASTNode* Stmt(FILE* fptr)
{
    ASTNode* ctrlStmtNode = CtrlStmt(fptr);
    if (ctrlStmtNode)
        return ctrlStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid CtrlStmt in Stmt", 0);

    ASTNode* declStmtNode = DeclStmt(fptr);
    if (declStmtNode)
        return declStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid DeclStmt in Stmt", 0);

    ASTNode* exprStmtNode = ExprStmt(fptr);
    if (exprStmtNode)
        return exprStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid ExprStmt in Stmt", 0);

    ASTNode* returnStmtNode = ReturnStmt(fptr);
    if (returnStmtNode)
        return returnStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid ReturnStmt in Stmt", 0);
 
    return PARSE_FAIL(NAP);
}

ASTNode* ExprStmt(FILE* fptr)
{
    if (PeekNextTokenP(fptr) == SEMI) {
        GetNextTokenP(fptr);
        ASTNode* emptyNode = InitASTNode();
        emptyNode->type = EMPTY_NODE;
        return emptyNode;
    }

    ASTNode* exprNode = Expr(fptr);
    exprNode->type = EXPR_STMT_NODE;
    if (!exprNode) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid Expr in ExprStmt", 0);
        return PARSE_FAIL(NAP);
    }

    if(CompareToken(fptr, SEMI, "Semicolon missing in ExprStmt", ERRP) != VALID) {
        ASTFreeNodes(1, exprNode);
        return PARSE_FAIL(ERRP);
    }

    return exprNode;
}

ASTNode* DeclStmt(FILE* fptr)
{
    ASTNode* typeNode = Type(fptr);
    if (!typeNode) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid Type in DeclStmt", 0);
        return PARSE_FAIL(NAP);
    }

    ASTNode* varListNode = VarList(fptr);
    if (!varListNode) {
        ERROR_MESSAGE("Invalid VarList in DeclStmt", 1, typeNode);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, SEMI, "Semicolon missing in DeclStmt", ERRP) != VALID) {
        ASTFreeNodes(typeNode, varListNode);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* declStmtNode = InitASTNode();
    declStmtNode->type = DECL_STMT_NODE;
    ASTPushChildNode(declStmtNode, typeNode, TYPE_NODE);
    ASTPushChildNode(declStmtNode, varListNode, VAR_LIST_NODE);
    return declStmtNode;
}

ASTNode* CtrlStmt(FILE* fptr) 
{
    ASTNode* ifStmtNode = IfStmt(fptr);
    if (ifStmtNode)
        return ifStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid IfStmt in Stmt", 0);

    ASTNode* switchStmtNode = SwitchStmt(fptr);
    if (switchStmtNode)
        return switchStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid SwitchStmt in Stmt", 0);

    ASTNode* whileStmtNode = WhileStmt(fptr);
    if (whileStmtNode)
        return whileStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid WhileStmt in Stmt", 0);

    ASTNode* doWhileStmtNode = DoWhileStmt(fptr);
    if (doWhileStmtNode)
        return doWhileStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid DoWhileStmt in Stmt", 0);

    ASTNode* forStmtNode = ForStmt(fptr);
    if (forStmtNode)
        return forStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid ForStmt in Stmt", 0);
 
    return PARSE_FAIL(NAP);
}

ASTNode* ReturnStmt(FILE* fptr) 
{
    if (PeekNextTokenP(fptr) != RET)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    ASTNode* exprNode = Expr(fptr);
    if (!exprNode) {

        if (PARSE_ERROR == ERRP) 
            return ERROR_MESSAGE("Invalid Expr in ReturnStmt", 0);
         
        exprNode = InitASTNode();
        exprNode->type = EMPTY_NODE;
    }

    if (CompareToken(fptr, SEMI, "Semicolon missing in ReturnStmt", ERRP) != VALID) {
        ASTFreeNodes(1, exprNode);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* returnStmtNode = InitASTNode();
    returnStmtNode->type = RETURN_STMT_NODE;
    ASTPushChildNode(returnStmtNode, exprNode, EXPR_NODE);

    return returnStmtNode;
}

ASTNode* IfStmt(FILE* fptr) 
{
    ASTNode* ifStmtNode = InitASTnode();
    ifStmtNode->type = IF_STMT_NODE;

    ASTNode* ifNode = IfElifElse(fptr, IF);
    if (!ifNode) {
        if (PARSE_ERROR == ERRP) 
            return ERROR_MESSAGE("Invalid If in IfStmt", 1, ifStmtNode);

        return PARSE_FAIL(NAP);
    }
    ASTPushChildNode(ifStmtNode, ifNode, IF_NODE);

    while (true) {
        ASTNode* elifNode = IfElifElse(fptr, ELIF);
        if (!elifNode) {
            if (PARSE_ERROR == ERRP) 
                return ERROR_MESSAGE("Invalid Elif in IfStmt", 2, ifNode, ifStmtNode);

            break;
        }
        ASTPushChildNode(ifStmtNode, elifNode, ELIF_NODE);
    }

    ASTNode* elseNode = IfElifElse(fptr, ELSE);
    if (elseNode) 
        ASTPushChildNode(ifStmtNode, elseNode, ELSE_NODE);
    else if (PARSE_ERROR == ERRP) {
        return ERROR_MESSAGE("Invalid Else in IfStmt", 2, ifNode, ifStmtNode);
    }

    return ifStmtNode;
}


ASTNode* IfElifElse(FILE* fptr, TokenType type) 
{
    if (PeekNextTokenP(fptr) != type)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    ASTNode* ifElifElseNode = InitASTNode();

    if (type != ELSE) {
        if (CompareToken(fptr, LPAREN, "Missing LPAREN in IfStmt", ERRP) != VALID) {
            ASTFreeNodes(ifElifElseNode);
            return PARSE_FAIL(ERRP);
        }

        ASTNode* exprNode = Expr(fptr);
        if (!exprNode) {
            ERROR_MESSAGE("Invalid Expr in IfStmt", 1, ifElifElseNode);
            return PARSE_FAIL(ERRP);    
        }

        if (CompareToken(fptr, RPAREN, "Missing RPAREN in IfStmt", ERRP) != VALID) {
            ASTFreeNodes(exprNode, ifElifElseNode);
            return PARSE_FAIL(ERRP);
        }

        ASTPushChildNode(ifElifElseNode, exprNode, EXPR_NODE);
    }

    ASTNode* bodyNode = Body(fptr);
    if (!bodyNode) {
        ERROR_MESSAGE("Invalid Body in IfStmt", 1, ifElifElseNode);
        return PARSE_FAIL(ERRP); 
    }
    ASTPushChildNode(ifElifElseNode, bodyNode, BODY_NODE);

    return ifElifElseNode;
}

ASTNode* SwitchStmt(FILE* fptr) 
{
    ASTNode* switchStmtNode = InitASTNode();
    switchStmtNode->type = SWITCH_STMT_NODE;

    if (PeekNextTokenP(fptr) != SWITCH)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    if(CompareToken(fptr, LPAREN, "No LPAREN in SwitchStmt", ERRP) != VALID)
        return PARSE_FAIL(ERRP);
    
    ASTNode* exprNode = Expr(fptr);
    if (!exprNode) {
        ERROR_MESSAGE("Invalid Expr in SwtichStmt", 0);
        return PARSE_FAIL(ERRP);
    }
    ASTPushChildNode(switchStmtNode, exprNode, EXPR_NODE);

    if(CompareToken(fptr, RPAREN, "No RPAREN in SwitchStmt", ERRP) != VALID) {
        ASTFreeNodes(exprNode, switchStmtNode);
        return PARSE_FAIL(ERRP);
    }

    if(CompareToken(fptr, LBRACK, "No LBRACK in SwitchStmt", ERRP) != VALID) {
        ASTFreeNodes(exprNode, switchStmtNode);
        return PARSE_FAIL(ERRP);
    }

    while (true) {
        ASTNode* caseNode = Case(fptr);
        if (!caseNode) {
            if (PARSE_ERROR == ERRP) 
                return ERROR_MESSAGE("Invalid Case in SwitchStmt", 2, exprNode, switchStmtNode);
            break;
        }
        ASTPushChildNode(switchStmtNode, caseNode, CASE_NODE);
    }

    ASTNode* defaultNode = Default(fptr);
    if (defaultNode) 
        ASTPushChildNode(switchStmtNode, defaultNode, DEFAULT_NODE);
    else if (PARSE_ERROR == ERRP) 
        return ERROR_MESSAGE("Invalid Default in SwitchStmt", 2, exprNode, switchStmtNode);

    if(CompareToken(fptr, RBRACK, "No RBRACK in SwitchStmt", ERRP) != VALID) {
        ASTFreeNodes(exprNode, defaultNode, switchStmtNode);
        return PARSE_FAIL(ERRP);
    }

    return switchStmtNode;
}

ASTNode* Case(FILE* fptr) 
{
    if (PeekNextTokenP(fptr) != CASE)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    ASTNode* exprNode = Expr(fptr);
    if (!exprNode) {
        PARSE_FAIL(ERRP);
        return ERROR_MESSAGE("Invalid Expr in Case", 0);
    }

    if (CompareToken(fptr, COLON, "No colon found in Case", ERRP) != VALID) {
        ASTFreeNodes(exprNode);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* stmtListNode = StmtList(fptr);
    if (!stmtListNode) {
        PARSE_FAIL(ERRP);
        return ERROR_MESSAGE("Invalid StmtList in Case", 1, exprNode);
    }

    ASTNode* caseNode = InitASTNode();
    ASTPushChildNode(caseNode, exprNode, EXPR_NODE);
    ASTPushChildNode(caseNode, stmtListNode, STMT_LIST_NODE);
    return caseNode;
}

ASTNode* Default(FILE* fptr) 
{
    if (PeekNextTokenP(fptr) != DEFAULT)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    if (CompareToken(fptr, COLON, "No colon found in Default", ERRP) != VALID) 
        return PARSE_FAIL(ERRP);

    ASTNode* stmtListNode = StmtList(fptr);
    if (!stmtListNode) {
        PARSE_FAIL(ERRP);
        return ERROR_MESSAGE("Invalid StmtList in Case", 0);
    }

    ASTNode* defaultNode = InitASTNode();
    ASTPushChildNode(defaultNode, stmtListNode, STMT_LIST_NODE);
    return defaultNode;
}

ASTNode* WhileStmt(FILE* fptr)
{
    if (PeekNextTokenP(fptr) != WHILE)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    if (CompareToken(fptr, LPAREN, "No LPAREN found in WhileStmt", ERRP) != VALID)
        return PARSE_FAIL(ERRP);

    ASTNode* exprNode = Expr(fptr);
    if (!exprNode) {
        ERROR_MESSAGE("Invalid Expr in WhileStmt", 0);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, RPAREN, "No RPAREN found in WhileStmt", ERRP) != VALID) { 
        ASTFreeNodes(exprNode);
        return PARSE_FAIL(ERRP);
    }
    
    ASTNode* bodyNode = Body(fptr);
    if (!bodyNode) {
        ERROR_MESSAGE("Invalid Body in WhileStmt", 1, exprNode);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* whileStmtNode = InitASTNode();
    whileStmtNode->type = WHILE_STMT_NODE;
    ASTPushChildNode(whileStmtNode, exprNode, EXPR_NODE);
    ASTPushChildNode(whileStmtNode, bodyNode, BODY_NODE);
    return whileStmtNode;
}

ASTNode* DoWhileStmt(FILE* fptr) 
{
    if (PeekNextTokenP(fptr) != DO)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    ASTNode* bodyNode = Body(fptr);
    if (!bodyNode) {
        ERROR_MESSAGE("Invalid Body in DoWhileStmt", 0);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, WHILE, "No WHILE found in DoWhileStmt", ERRP) != VALID) {
        ASTFreeNodes(bodyNode);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, LPAREN, "No LPAREN found in DoWhileStmt", ERRP) != VALID) {
        ASTFreeNodes(bodyNode);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* exprNode = Expr(fptr);
    if (!exprNode) {
        ERROR_MESSAGE("Invalid Expr in WhileStmt", 1, bodyNode);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, RPAREN, "No RPAREN found in WhileStmt", ERRP) != VALID) {
        ASTFreeNodes(bodyNode, exprNode);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, SEMI, "No SEMI in DoWhileStmt", ERRP) != VALID) {
        ASTFreeNodes(bodyNode, exprNode);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* doWhileStmtNode = InitASTNode();
    doWhileStmtNode->type = DO_WHILE_STMT_NODE ;
    ASTPushChildNode(doWhileStmtNode, bodyNode, BODY_NODE);
    ASTPushChildNode(doWhileStmtNode, exprNode, EXPR_NODE);
    return doWhileStmtNode;
}

ASTNode* ForStmt(FILE* fptr) 
{
    /* TODO: All Expr and ExprList are optional, make them behave like it */

    if (PeekNextTokenP(fptr) != FOR)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    if (CompareToken(fptr, LPAREN, "No LPAREN found in ForStmt", ERRP) != VALID)
        return PARSE_FAIL(ERRP);

    ASTNode* exprListNode = ExprList(fptr);
    if (!exprListNode) {
        ERROR_MESSAGE("Invalid ExprList in ForStmt", 0);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, SEMI, "Missing SEMI in ForStmt", ERRP)) {
        ASTFreeNodes(exprListNode);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* exprNode = Expr(fptr);
    if (!exprNode) {
        ERROR_MESSAGE("Invalid Expr in ForStmt", 1, exprListNode);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, SEMI, "Missing SEMI in ForStmt", ERRP)) {
        ASTFreeNodes(exprListNode, exprNode);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* exprListNode2 = ExprList(fptr);
    if (!exprListNode2) {
        ERROR_MESSAGE("Invalid ExprList in ForStmt", 2, exprListNode, exprNode);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, RPAREN, "No RPAREN found in ForStmt", ERRP) != VALID) { 
        ASTFreeNodes(exprListNode, exprNode, exprListNode2);
        return PARSE_FAIL(ERRP);
    }
    
    ASTNode* bodyNode = Body(fptr);
    if (!bodyNode) {
        ERROR_MESSAGE("Invalid Body in ForStmt", 3, exprListNode, exprNode, exprListNode2);
        return PARSE_FAIL(ERRP);
    }

    ASTNode* forStmtNode = InitASTNode();
    forStmtNode->type = FOR_STMT_NODE;
    ASTPushChildNode(forStmtNode, exprListNode, EXPR_LIST_NODE);
    ASTPushChildNode(forStmtNode, exprNode, EXPR_NODE);
    ASTPushChildNode(forStmtNode, exprListNode2, EXPR_LIST_NODE);
    return forStmtNode;
}

/* ----------- Expressions ---------- */

ASTNode* ExprList(FILE* fptr) 
{
    ASTNode* exprListNode = InitASTNode();

    ASTNode* exprNode = Expr(fptr);
    if (!exprNode) {
        if (PARSE_ERROR == ERRP) 
            return ERROR_MESSAGE("Invalid Expr in ExprList", 0);
        return PARSE_FAIL(NAP);
    }
    ASTPushChildNode(exprListNode, exprNode, EXPR_NODE);

    while (true) {
        if (PeekNextTokenP(fptr) != COMMA) 
            break;
        GetNextTokenP(fptr);

        exprNode = Expr(fptr);
        if (!exprNode) {
            if (PARSE_ERROR == ERRP) 
                return ERROR_MESSAGE("Invalid Expr in ExprList", 2, exprNode, exprListNode);
            return PARSE_FAIL(NAP);
        }
        ASTPushChildNode(exprListNode, exprNode, EXPR_NODE);
    }

    return exprListNode;
}

ASTNode* Expr(FILE* fptr) 
{   
    /* TODO: Technically an Alias for AsgnEpxr, but allows for easier readability */
    return AsgnExpr(fptr);
}

ASTNode* AsgnExpr(FILE* fptr)
{

    ASTNode* lhs = OrlExpr(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid OrlExpr in AsgnExpr", 0);
        return PARSER_FAIL(NAP);
    }

    Token tok = GetNextTokenP(fptr);
    if (ValidTokType(ASSIGNS, ASSIGNS_COUNT, tok.type) != VALID) {
        ERROR_MESSAGE()
    }

    ASTNode* rhs = AsgnExpr(fptr);
    if (!rhs) {

    }
    
    ASTNode* operatorNode = InitASTNode();
}