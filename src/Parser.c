#include "Parser.h"

/* TODO: 
        - GetNextTokenP after PeekNextTokenP might be dangerous, but since the 
          preprocessor only skips directors, for now it is fine.
        - Can easily simplify most of the expressions with a helper function to reduce
          boilerplate code, which is making the file artifically longer than it needs
        - Instead of having a global error state, have it per function, that way
          resetting at the entrance of each function isn't mandatory.
        - Having the child set its own type isn't consistent with the most functions,
          however it is important for children that can have multiple types to do this. 
          Need to figure out a way to fix this.
*/

/* ----------- ERRORS ---------- */

ParseResult PARSE_VALID(ASTNode* node, NodeType type)
{
    ParseResult result = {VALID, node};
    result.node->type = type;
    return result;
}

ParseResult PARSE_NAP() 
{
    ParseResult result = {NAP, NULL};
    return result;
}

ParseResult PARSE_ERRP(char* message)
{
    ParseResult result = {ERRP, NULL};
    ERROR_MESSAGE(message);
    return result;
}



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

ParseResult IdentNode(Token tok)
{
    ASTNode* identNode = InitASTNode();
    identNode->token = tok;
    return PARSE_VALID(identNode, IDENT_NODE);
}

ParseResult ExprNode(Token tok)
{
    ASTNode* exprNode = InitASTNode();
    exprNode->token = tok;
    return PARSE_VALID(exprNode, EXPR_NODE);
}

ParseResult EmptyNode()
{
    ASTNode* emptyNode = InitASTNode();
    emptyNode->type = EMPTY_NODE;
    return PARSE_VALID(emptyNode, EMPTY_NODE)
}


/* ---------- EBNF ---------- */

AST* Program(FILE* fptr)
{
    /* TODO: errors regarding ast freeing and progNode freeing */
    /* Allow global DeclStmts without abiguity (both start with Type Ident) */
    AST* ast = ASTInit();
    ASTNode* progNode = ast->root;
    
    while (true) {
        if (PeekNextTokenP(fptr) == END)
            return ast;

        ParseResult node = Function(fptr);
        if (node->node) {
            if (!node){
                ERROR_MESSAGE("Invalid Function", 0);
                return NULL;
            }
            ASTPushChildNode(progNode, node, FUNC_NODE);
        } else {
            node = DeclStmt(fptr);
            if (!node) {
                ERROR_MESSAGE("Invalid DeclStmt in Global", 0);
                return NULL;
            }
            ASTPushChildNode(progNode, node, DECL_STMT_NODE);
        }

        printf("Failed to Parse AST\n");
        break;
    }


    return ast;
}

ParseResult Function(FILE* fptr)
{
    printf("Starting Function\n");
    ParseResult typeNode = Type(fptr);
    if (typeNode.status == ERRP)
        return PARSE_ERRP("Invalid Type in Function");
    else if (typeNode.status == NAP)
        return PARSE_NAP();

    if (PeekNextTokenP(fptr) != IDENT) {
        ASTFreeNodes(1, typeNode.node);
        return PARSE_ERRP("Function does not have a name");
    }
    ParseResult identNode = IdentNode(GetNextTokenP(fptr));

    if (PeekNextTokenP(fptr) != LPAREN) {
        ASTFreeNodes(2, typeNode.node, identNode.node);
        return PARSE_ERRP("Missing left parenthesis in function");
    }
    GetNextTokenP(fptr);

    ParseResult paramListNode = ParamList(fptr);
    if (paramListNode.status != VALID ) {
        ASTFreeNodes(2, typeNode.node, identNode.node);
        return PARSE_ERRP("Invalid ParamList in Function");
    }

    if (PeekNextTokenP(fptr) != RPAREN) {
        ASTFreeNodes(3, typeNode.node, paramListNode.node, identNode.node);
        return PARSE_ERRP("Missing right parenthesis in function");
    }
    GetNextTokenP(fptr);

    ParseResult bodyNode = Body(fptr);
    if (bodyNode.status != VALID) {
        ASTFreeNodes(3, typeNode.node, paramListNode.node, identNode);
        return PARSE_ERRP("Invalid Body in Function");
    }

    ASTNode* funcNode = InitASTNode();
    ASTPushChildNode(funcNode, typeNode.node);
    ASTPushChildNode(funcNode, identNode.node);
    ASTPushChildNode(funcNode, paramListNode.node);
    ASTPushChildNode(funcNode, bodyNode.node);
    return PARSE_VALID(funcNode, FUNC_NODE);
}

ParseResult ParamList(FILE* fptr)
{
    printf("Starting ParamList\n");
    if (PeekNextTokenP(fptr) == RPAREN) 
        return EmptyNode();
    
    ParseResult paramNode = Param(fptr);
    if (paramNode.status == ERRP)
        return PARSE_ERRP("Invalid Param in ParamList");
    else if (paramNode.status == NAP)
        return PARSE_NAP();

    ASTNode* paramListNode = InitASTNode();
    ASTPushChildNode(paramListNode, paramNode.node);

    while(true)
    {
        if (PeekNextTokenP(fptr) != COMMA)
            break;
        GetNextTokenP(fptr);

        paramNode = Param(fptr);
        if (paramNode.status != VALID){
            ASTFreeNodes(2, paramNode.node, paramListNode);
            return PARSE_ERRP("Invalid Param in ParamList");
        }
        ASTPushChildNode(paramListNode, paramNode.node);
    }

    return PARSE_VALID(paramListNode, PARAM_LIST_NODE);
}

ParseResult Param(FILE* fptr)
{
    printf("Starting Param\n");

    ParseResult typeNode = Type(fptr);
    if (typeNode.status == ERRP)
        return PARSE_ERRP("Invalid Param");
    else if (typeNode.status == NAP)
        return PARSE_NAP();

    if (PeekNextTokenP(fptr) != IDENT) {
        ASTFreeNodes(1, typeNode.node);
        return PARSE_ERRP("Param does not have a name");
    }
    ParseResult identNode = IdentNode(GetNextTokenP(fptr));

    ASTNode* paramNode = InitASTNode();
    ASTPushChildNode(paramNode, typeNode.node);
    ASTPushChildNode(paramNode, identNode.node);
    return PARSE_VALID(paramNode, PARAM_NODE);;
}

/* ---------- Statements ----------- */

ParseResult Body(FILE* fptr)
{
    printf("Entering Body\n");
    
    if (PeekNextTokenP(fptr) != LBRACK)
        return PARSE_NAP();
    GetNextTokenP(fptr);

    ParseResult stmtListNode = StmtList(fptr);
    if (stmtListNode.status != VALID)
        return PARSE_ERRP("Invalid StmtList in Body");

    if (PeekNextTokenP(fptr) != RBRACK) {
        ASTFreeNodes(1, stmtListNode.node);
        return PARSE_ERRP("Missing Right Bracket in Body");
    }

    ASTNode* bodyNode = InitASTNode();
    ASTPushChildNode(bodyNode, stmtListNode.node);
    return PARSE_VALID(bodyNode, BODY_NODE);
}

ParseResult StmtList(FILE* fptr)
{
    printf("Entering StmtList\n");
    
    ASTNode* stmtListNode = InitASTNode();

    while (true) {
        if (PeekNextTokenP(fptr) == RBRACK)
            return PARSE_NAP();

        ParseResult stmtNode = Stmt(fptr);
        if (stmtNode.status == VALID) {
            ASTPushChildNode(stmtListNode, stmtNode.node);
            continue;
        } else if (stmtNode.status == NAP)
            break;

        ASTFreeNodes(2, stmtNode.node, stmtListNode);
        return PARSE_ERRP("Invalid Stmt in StmtList");
    }

    return PARSE_VALID(stmtListNode, STMT_LIST_NODE);
}

ParseResult Stmt(FILE* fptr)
{
    printf("Entering Stmt\n");

    ParseResult ctrlStmtNode = CtrlStmt(fptr);
    if (ctrlStmtNode.status == VALID)
        return PARSE_VALID(ctrlStmtNode.node, CTRL_STMT_NODE);
    else if (ctrlStmtNode.status == ERRP)
        return PARSE_ERRP("Invalid CtrlStmt in Stmt");

    ParseResult declStmtNode = DeclStmt(fptr);
    if (declStmtNode.status == VALID)
        return PARSE_VALID(declStmtNode.node, DECL_STMT_NODE);
    else if (declStmtNode.status == ERRP)
        return PARSE_ERRP("Invalid DeclStmt in Stmt");

    ParseResult exprStmtNode = ExprStmt(fptr);
    if (exprStmtNode.status == VALID)
        return PARSE_VALID(exprStmtNode.node, EXPR_STMT_NODE);
    else if (exprStmtNode.status == ERRP)
        return PARSE_ERRP("Invalid ExprStmt in Stmt");

    ParseResult returnStmtNode = ReturnStmt(fptr);
    if (returnStmtNode.status == VALID)
        return PARSE_VALID(returnStmtNode.node, RETURN_STMT_NODE);
    else if (returnStmtNode.status == ERRP)
        return PARSE_ERRP("Invalid ReturnStmt in Stmt");
 
    return PARSE_NAP();
}

ParseResult ExprStmt(FILE* fptr)
{
    printf("Entering ExprStmt\n");

    if (PeekNextTokenP(fptr) == SEMI) {
        GetNextTokenP(fptr);
        return EmptyNode();
    }
    
    ParseResult exprNode = Expr(fptr);
    if (exprNode.status == ERRP) 
        return PARSE_ERRP("Invalid Expr in ExprStmt");
    else if (exprNode.status == NAP)
        return PARSE_NAP();

    if (PeekNextTokenP(fptr) != SEMI) {
        ASTFreeNodes(1, exprNode.node);
        return PARSE_ERRP("No Semicolon after Expr in ExprStmt");
    }
    GetNextTokenP(fptr);

    ASTNode* stmt = InitASTNode();
    ASTPushChildNode(stmt, exprNode.node);

    return PARSE_VALID(exprNode.node, EXPR_STMT_NODE);
}

ParseResult DeclStmt(FILE* fptr)
{
    printf("Enter DeclStmt\n");
    
    ParseResult typeNode = Type(fptr);
    if (typeNode.status == ERRP)
        return PARSE_ERRP("Invalid Type in DeclStmt");
    else if (typeNode.status == NAP)
        return PARSE_NAP();

    ParseResult varListNode = VarList(fptr);
    if (varListNode.status != VALID) {
        ASTFreeNodes(1, typeNode.node);
        return PARSE_ERRP("invalid VarList in DeclStmt");
    }

    if (PeekNextTokenP(fptr) != SEMI) {
        ASTFreeNodes(2, typeNode, varListNode);
        return PARSE_ERRP("Semicolon missing in DeclStmt");
    }

    ASTNode* declStmtNode = InitASTNode();
    ASTPushChildNode(declStmtNode, typeNode.node);
    ASTPushChildNode(declStmtNode, varListNode.node);
    return PARSE_VALID(declStmtNode, DECL_STMT_NODE);
}

ParseResult CtrlStmt(FILE* fptr) 
{
    printf("Entering CtrlStmt\n");
    
    ParseResult ifStmtNode = IfStmt(fptr);
    if (ifStmtNode)
        return ifStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid IfStmt in Stmt", 0);

    ParseResult switchStmtNode = SwitchStmt(fptr);
    if (switchStmtNode)
        return switchStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid SwitchStmt in Stmt", 0);

    ParseResult whileStmtNode = WhileStmt(fptr);
    if (whileStmtNode)
        return whileStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid WhileStmt in Stmt", 0);

    ParseResult doWhileStmtNode = DoWhileStmt(fptr);
    if (doWhileStmtNode)
        return doWhileStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid DoWhileStmt in Stmt", 0);

    ParseResult forStmtNode = ForStmt(fptr);
    if (forStmtNode)
        return forStmtNode;
    else if (PARSE_ERROR == ERRP)
        return ERROR_MESSAGE("Invalid ForStmt in Stmt", 0);
 
    return PARSE_FAIL(NAP);
}

ParseResult ReturnStmt(FILE* fptr) 
{
    printf("Entering ReturnStmt\n");
    
    if (PeekNextTokenP(fptr) != RET)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    ParseResult exprNode = Expr(fptr);
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

    ParseResult returnStmtNode = InitASTNode();
    returnStmtNode->type = RETURN_STMT_NODE;
    ASTPushChildNode(returnStmtNode, exprNode, EXPR_NODE);

    return returnStmtNode;
}

ParseResult IfStmt(FILE* fptr) 
{
    printf("Entering IfStmt\n");
    
    ParseResult ifStmtNode = InitASTNode();
    ifStmtNode->type = IF_STMT_NODE;

    ParseResult ifNode = IfElifElse(fptr, IF);
    if (!ifNode) {
        if (PARSE_ERROR == ERRP) 
            return ERROR_MESSAGE("Invalid If in IfStmt", 1, ifStmtNode);

        return PARSE_FAIL(NAP);
    }
    ASTPushChildNode(ifStmtNode, ifNode, IF_NODE);

    while (true) {
        ParseResult elifNode = IfElifElse(fptr, ELIF);
        if (!elifNode) {
            if (PARSE_ERROR == ERRP) 
                return ERROR_MESSAGE("Invalid Elif in IfStmt", 2, ifNode, ifStmtNode);

            break;
        }
        ASTPushChildNode(ifStmtNode, elifNode, ELIF_NODE);
    }

    ParseResult elseNode = IfElifElse(fptr, ELSE);
    if (elseNode) 
        ASTPushChildNode(ifStmtNode, elseNode, ELSE_NODE);
    else if (PARSE_ERROR == ERRP) {
        return ERROR_MESSAGE("Invalid Else in IfStmt", 2, ifNode, ifStmtNode);
    }

    return ifStmtNode;
}


ParseResult IfElifElse(FILE* fptr, TokenType type) 
{
    printf("Entering IfElifElse\n");
    
    if (PeekNextTokenP(fptr) != type)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    ParseResult ifElifElseNode = InitASTNode();

    if (type != ELSE) {
        if (CompareToken(fptr, LPAREN, "Missing LPAREN in IfStmt", ERRP) != VALID) {
            ASTFreeNodes(1, ifElifElseNode);
            return PARSE_FAIL(ERRP);
        }

        ParseResult exprNode = Expr(fptr);
        if (!exprNode) {
            ERROR_MESSAGE("Invalid Expr in IfStmt", 1, ifElifElseNode);
            return PARSE_FAIL(ERRP);    
        }

        if (CompareToken(fptr, RPAREN, "Missing RPAREN in IfStmt", ERRP) != VALID) {
            ASTFreeNodes(2, exprNode, ifElifElseNode);
            return PARSE_FAIL(ERRP);
        }

        ASTPushChildNode(ifElifElseNode, exprNode, EXPR_NODE);
    }

    ParseResult bodyNode = Body(fptr);
    if (!bodyNode) {
        ERROR_MESSAGE("Invalid Body in IfStmt", 1, ifElifElseNode);
        return PARSE_FAIL(ERRP); 
    }
    ASTPushChildNode(ifElifElseNode, bodyNode, BODY_NODE);

    return ifElifElseNode;
}

ParseResult SwitchStmt(FILE* fptr) 
{
    printf("Entering SwtichStmt\n");
    
    ParseResult switchStmtNode = InitASTNode();
    switchStmtNode->type = SWITCH_STMT_NODE;

    if (PeekNextTokenP(fptr) != SWITCH)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    if(CompareToken(fptr, LPAREN, "No LPAREN in SwitchStmt", ERRP) != VALID)
        return PARSE_FAIL(ERRP);
    
    ParseResult exprNode = Expr(fptr);
    if (!exprNode) {
        ERROR_MESSAGE("Invalid Expr in SwtichStmt", 0);
        return PARSE_FAIL(ERRP);
    }
    ASTPushChildNode(switchStmtNode, exprNode, EXPR_NODE);

    if(CompareToken(fptr, RPAREN, "No RPAREN in SwitchStmt", ERRP) != VALID) {
        ASTFreeNodes(2, exprNode, switchStmtNode);
        return PARSE_FAIL(ERRP);
    }

    if(CompareToken(fptr, LBRACK, "No LBRACK in SwitchStmt", ERRP) != VALID) {
        ASTFreeNodes(2, exprNode, switchStmtNode);
        return PARSE_FAIL(ERRP);
    }

    while (true) {
        ParseResult caseNode = Case(fptr);
        if (!caseNode) {
            if (PARSE_ERROR == ERRP) 
                return ERROR_MESSAGE("Invalid Case in SwitchStmt", 2, exprNode, switchStmtNode);
            break;
        }
        ASTPushChildNode(switchStmtNode, caseNode, CASE_NODE);
    }

    ParseResult defaultNode = Default(fptr);
    if (defaultNode) 
        ASTPushChildNode(switchStmtNode, defaultNode, DEFAULT_NODE);
    else if (PARSE_ERROR == ERRP) 
        return ERROR_MESSAGE("Invalid Default in SwitchStmt", 2, exprNode, switchStmtNode);

    if(CompareToken(fptr, RBRACK, "No RBRACK in SwitchStmt", ERRP) != VALID) {
        ASTFreeNodes(3, exprNode, defaultNode, switchStmtNode);
        return PARSE_FAIL(ERRP);
    }

    return switchStmtNode;
}

ParseResult Case(FILE* fptr) 
{
    printf("Entering Case\n");
    
    if (PeekNextTokenP(fptr) != CASE)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    ParseResult exprNode = Expr(fptr);
    if (!exprNode) {
        PARSE_FAIL(ERRP);
        return ERROR_MESSAGE("Invalid Expr in Case", 0);
    }

    if (CompareToken(fptr, COLON, "No colon found in Case", ERRP) != VALID) {
        ASTFreeNodes(1, exprNode);
        return PARSE_FAIL(ERRP);
    }

    ParseResult stmtListNode = StmtList(fptr);
    if (!stmtListNode) {
        PARSE_FAIL(ERRP);
        return ERROR_MESSAGE("Invalid StmtList in Case", 1, exprNode);
    }

    ParseResult caseNode = InitASTNode();
    ASTPushChildNode(caseNode, exprNode, EXPR_NODE);
    ASTPushChildNode(caseNode, stmtListNode, STMT_LIST_NODE);
    return caseNode;
}

ParseResult Default(FILE* fptr) 
{
    printf("Entering Default\n");
    
    if (PeekNextTokenP(fptr) != DEFAULT)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    if (CompareToken(fptr, COLON, "No colon found in Default", ERRP) != VALID) 
        return PARSE_FAIL(ERRP);

    ParseResult stmtListNode = StmtList(fptr);
    if (!stmtListNode) {
        PARSE_FAIL(ERRP);
        return ERROR_MESSAGE("Invalid StmtList in Case", 0);
    }

    ParseResult defaultNode = InitASTNode();
    ASTPushChildNode(defaultNode, stmtListNode, STMT_LIST_NODE);
    return defaultNode;
}

ParseResult WhileStmt(FILE* fptr)
{
    printf("Entering WhileStmt\n");
    
    if (PeekNextTokenP(fptr) != WHILE)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    if (CompareToken(fptr, LPAREN, "No LPAREN found in WhileStmt", ERRP) != VALID)
        return PARSE_FAIL(ERRP);

    ParseResult exprNode = Expr(fptr);
    if (!exprNode) {
        ERROR_MESSAGE("Invalid Expr in WhileStmt", 0);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, RPAREN, "No RPAREN found in WhileStmt", ERRP) != VALID) { 
        ASTFreeNodes(1, exprNode);
        return PARSE_FAIL(ERRP);
    }
    
    ParseResult bodyNode = Body(fptr);
    if (!bodyNode) {
        ERROR_MESSAGE("Invalid Body in WhileStmt", 1, exprNode);
        return PARSE_FAIL(ERRP);
    }

    ParseResult whileStmtNode = InitASTNode();
    whileStmtNode->type = WHILE_STMT_NODE;
    ASTPushChildNode(whileStmtNode, exprNode, EXPR_NODE);
    ASTPushChildNode(whileStmtNode, bodyNode, BODY_NODE);
    return whileStmtNode;
}

ParseResult DoWhileStmt(FILE* fptr) 
{
    printf("Entering DoWhileStmt\n");
    
    if (PeekNextTokenP(fptr) != DO)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    ParseResult bodyNode = Body(fptr);
    if (!bodyNode) {
        ERROR_MESSAGE("Invalid Body in DoWhileStmt", 0);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, WHILE, "No WHILE found in DoWhileStmt", ERRP) != VALID) {
        ASTFreeNodes(1, bodyNode);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, LPAREN, "No LPAREN found in DoWhileStmt", ERRP) != VALID) {
        ASTFreeNodes(1, bodyNode);
        return PARSE_FAIL(ERRP);
    }

    ParseResult exprNode = Expr(fptr);
    if (!exprNode) {
        ERROR_MESSAGE("Invalid Expr in WhileStmt", 1, bodyNode);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, RPAREN, "No RPAREN found in WhileStmt", ERRP) != VALID) {
        ASTFreeNodes(2, bodyNode, exprNode);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, SEMI, "No SEMI in DoWhileStmt", ERRP) != VALID) {
        ASTFreeNodes(2, bodyNode, exprNode);
        return PARSE_FAIL(ERRP);
    }

    ParseResult doWhileStmtNode = InitASTNode();
    doWhileStmtNode->type = DO_WHILE_STMT_NODE ;
    ASTPushChildNode(doWhileStmtNode, bodyNode, BODY_NODE);
    ASTPushChildNode(doWhileStmtNode, exprNode, EXPR_NODE);
    return doWhileStmtNode;
}

ParseResult ForStmt(FILE* fptr) 
{
    printf("Entering ForStmt\n");
    
    /* TODO: All Expr and ExprList are optional, make them behave like it */

    if (PeekNextTokenP(fptr) != FOR)
        return PARSE_FAIL(NAP);
    GetNextTokenP(fptr);

    if (CompareToken(fptr, LPAREN, "No LPAREN found in ForStmt", ERRP) != VALID)
        return PARSE_FAIL(ERRP);

    ParseResult exprListNode = ExprList(fptr);
    if (!exprListNode) {
        ERROR_MESSAGE("Invalid ExprList in ForStmt", 0);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, SEMI, "Missing SEMI in ForStmt", ERRP) != VALID) {
        ASTFreeNodes(1, exprListNode);
        return PARSE_FAIL(ERRP);
    }

    ParseResult exprNode = Expr(fptr);
    if (!exprNode) {
        ERROR_MESSAGE("Invalid Expr in ForStmt", 1, exprListNode);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, SEMI, "Missing SEMI in ForStmt", ERRP) != VALID) {
        ASTFreeNodes(2, exprListNode, exprNode);
        return PARSE_FAIL(ERRP);
    }

    ParseResult exprListNode2 = ExprList(fptr);
    if (!exprListNode2) {
        ERROR_MESSAGE("Invalid ExprList in ForStmt", 2, exprListNode, exprNode);
        return PARSE_FAIL(ERRP);
    }

    if (CompareToken(fptr, RPAREN, "No RPAREN found in ForStmt", ERRP) != VALID) { 
        ASTFreeNodes(3, exprListNode, exprNode, exprListNode2);
        return PARSE_FAIL(ERRP);
    }
    
    ParseResult bodyNode = Body(fptr);
    if (!bodyNode) {
        ERROR_MESSAGE("Invalid Body in ForStmt", 3, exprListNode, exprNode, exprListNode2);
        return PARSE_FAIL(ERRP);
    }

    ParseResult forStmtNode = InitASTNode();
    forStmtNode->type = FOR_STMT_NODE;
    ASTPushChildNode(forStmtNode, exprListNode, EXPR_LIST_NODE);
    ASTPushChildNode(forStmtNode, exprNode, EXPR_NODE);
    ASTPushChildNode(forStmtNode, exprListNode2, EXPR_LIST_NODE);
    return forStmtNode;
}

/* ----------- Expressions ---------- */

ParseResult ExprList(FILE* fptr) 
{
    printf("Entering ExprList\n");
    

    if (PeekNextTokenP(fptr) == SEMI || PeekNextTokenP(fptr) == RPAREN) {
        ParseResult emptyNode = InitASTNode();
        emptyNode->type = EMPTY_NODE;
        return emptyNode;
    }

    ParseResult exprListNode = InitASTNode();

    ParseResult exprNode = Expr(fptr);
    if (!exprNode) {
        if (PARSE_ERROR == ERRP) 
            return ERROR_MESSAGE("Invalid Expr in ExprList", 0);
        printf("Exiting Expr\n");
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

ParseResult Expr(FILE* fptr) 
{   
    printf("Entering Expr\n");
    
    /* TODO: Technically an Alias for AsgnEpxr, but allows for easier readability */
    ParseResult asgnExpr = AsgnExpr(fptr);
    if (!asgnExpr) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid AsgnExpr in Expr", 0);
        printf("Exiting AsgnExpr\n");
        return PARSE_FAIL(NAP);
    }
    return asgnExpr;
}

ParseResult AsgnExpr(FILE* fptr)
{
    printf("Entering AsgnExpr\n");
    
    ParseResult lhs = OrlExpr(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid OrlExpr in AsgnExpr", 0);
        printf("Exiting OrlExpr\n");
        return PARSE_FAIL(NAP);
    }

    if (ValidTokType(ASSIGNS, ASSIGNS_COUNT, PeekNextTokenP(fptr)) == VALID) 
    {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = AsgnExpr(fptr);
        if (!rhs) {
            ERROR_MESSAGE("Invalid AsgnExpr in AsgnExpr", 1, lhs);
            return PARSE_FAIL(ERRP);
        }
        
        ParseResult operatorNode = InitASTNode();
        operatorNode->type = EXPR_NODE;
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, lhs, LHS_NODE);
        ASTPushChildNode(operatorNode, rhs, RHS_NODE);

        lhs = operatorNode;
    }
    return lhs;
}

ParseResult OrlExpr(FILE* fptr) 
{
    printf("Entering OrlExpr\n");
    
    ParseResult lhs = AndlExpr(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid AndlExpr in OrlExpr", 0);
        printf("Exiting AndlExpr\n");
        return PARSE_FAIL(NAP);
    }

    while (PeekNextTokenP(fptr) == ORL) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = AndlExpr(fptr);
        if (!rhs) {
            ERROR_MESSAGE("Invalid AndlExpr in OrlExpr", 1, lhs);
            return PARSE_FAIL(ERRP);
        }

        ParseResult operatorNode = InitASTNode();
        operatorNode->type = EXPR_NODE; /* TODO: Not entirely sure it needs to be Expr_Node since the parent makes it LHS or RHS anyways */
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, lhs, LHS_NODE);
        ASTPushChildNode(operatorNode, rhs, RHS_NODE);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult AndlExpr(FILE* fptr)
{
    printf("Entering AndlExpr\n");
    
    ParseResult lhs = OrExpr(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid OrExpr in AndlExpr", 0);
        printf("Exiting OrExpr\n");
        return PARSE_FAIL(NAP);
    }

    while (PeekNextTokenP(fptr) == ANDL) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = XorExpr(fptr);
        if (!rhs) {
            ERROR_MESSAGE("Invalid OrExpr in AndlExpr", 1, lhs);
            return PARSE_FAIL(ERRP);
        }

        ParseResult operatorNode = InitASTNode();
        operatorNode->type = EXPR_NODE;
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, lhs, LHS_NODE);
        ASTPushChildNode(operatorNode, rhs, RHS_NODE);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult OrExpr(FILE* fptr)
{
    printf("Entering OrExpr\n");
    
    ParseResult lhs = XorExpr(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid XorExpr in OrExpr", 0);
        printf("Exiting XorExpr\n");
        return PARSE_FAIL(NAP);
    }

    while (PeekNextTokenP(fptr) == OR) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = XorExpr(fptr);
        if (!rhs) {
            ERROR_MESSAGE("Invalid XorExpr in OrExpr", 1, lhs);
            return PARSE_FAIL(ERRP);
        }

        ParseResult operatorNode = InitASTNode();
        operatorNode->type = EXPR_NODE;
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, lhs, LHS_NODE);
        ASTPushChildNode(operatorNode, rhs, RHS_NODE);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult XorExpr(FILE* fptr)
{
    printf("Entering XorExpr\n");
    
    ParseResult lhs = AndExpr(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid AndExpr in XorExpr", 0);
        printf("Exiting AndExpr\n");
        return PARSE_FAIL(NAP);
    }

    while (PeekNextTokenP(fptr) == XOR) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = AndExpr(fptr);
        if (!rhs) {
            ERROR_MESSAGE("Invalid AndExpr in XorExpr", 1, lhs);
            return PARSE_FAIL(ERRP);
        }

        ParseResult operatorNode = InitASTNode();
        operatorNode->type = EXPR_NODE;
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, lhs, LHS_NODE);
        ASTPushChildNode(operatorNode, rhs, RHS_NODE);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult AndExpr(FILE* fptr) 
{
    printf("Entering AndExpr\n");
    
    ParseResult lhs = EqqExpr(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid EqqExpr in AndExpr", 0);
        printf("Exiting EqqExpr\n");
        return PARSE_FAIL(NAP);
    }

    while (PeekNextTokenP(fptr) == AND) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = EqqExpr(fptr);
        if (!rhs) {
            ERROR_MESSAGE("Invalid EqqExpr in AndExpr", 1, lhs);
            return PARSE_FAIL(ERRP);
        }

        ParseResult operatorNode = InitASTNode();
        operatorNode->type = EXPR_NODE;
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, lhs, LHS_NODE);
        ASTPushChildNode(operatorNode, rhs, RHS_NODE);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult EqqExpr(FILE* fptr) 
{
    printf("Entering EqqExpr\n");
    
    ParseResult lhs = RelationExpr(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid RelationExpr in EqqExpr", 0);
        printf("Exiting RelationExpr\n");
        return PARSE_FAIL(NAP);
    }

    TokenType tokType = PeekNextTokenP(fptr);
    if (tokType == EQQ || tokType == NEQQ) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = RelationExpr(fptr);
        if (!rhs) {
            ERROR_MESSAGE("Invalid RelationExpr in EqqExpr", 1, lhs);
            return PARSE_FAIL(ERRP);
        }

        ParseResult operatorNode = InitASTNode();
        operatorNode->type = EXPR_NODE;
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, lhs, LHS_NODE);
        ASTPushChildNode(operatorNode, rhs, RHS_NODE);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult RelationExpr(FILE* fptr)
{
    printf("Entering RelationExpr\n");
    
    ParseResult lhs = ShiftExpr(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid ShiftExpr in RelationExpr", 0);
        printf("Exiting ShiftExpr\n");        
        return PARSE_FAIL(NAP);
    }

    TokenType tokType = PeekNextTokenP(fptr);
    if (ValidTokType(RELATIONAL, RELATIONAL_COUNT, tokType) == VALID) 
    {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = ShiftExpr(fptr);
        if (!rhs) {
            ERROR_MESSAGE("Invalid ShiftExpr in RelationExpr", 1, lhs);
            return PARSE_FAIL(ERRP);
        }

        ParseResult operatorNode = InitASTNode();
        operatorNode->type = EXPR_NODE;
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, lhs, LHS_NODE);
        ASTPushChildNode(operatorNode, rhs, RHS_NODE);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult ShiftExpr(FILE* fptr)
{
    printf("Entering ShiftExpr\n");
    
    ParseResult lhs = AddExpr(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid AddExpr in ShiftExpr", 0);
        printf("Exiting AddExpr\n");        
        return PARSE_FAIL(NAP);
    }

    TokenType tokType;
    while ((tokType = PeekNextTokenP(fptr)) == LSHIFT || tokType == RSHIFT) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = MultExpr(fptr);
        if (!rhs) {
            ERROR_MESSAGE("Invalid AddExpr in ShiftExpr", 1, lhs);
            return PARSE_FAIL(ERRP);
        }

        ParseResult operatorNode = InitASTNode();
        operatorNode->type = EXPR_NODE;
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, lhs, LHS_NODE);
        ASTPushChildNode(operatorNode, rhs, RHS_NODE);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult AddExpr(FILE* fptr)
{
    printf("Entering AddExpr\n");
    
    ParseResult lhs = MultExpr(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid MultExpr in AddExpr", 0);
        printf("Exiting MultExpr\n");        
        return PARSE_FAIL(NAP);
    }

    while (ValidTokType(ADDS, ADDS_COUNT, PeekNextTokenP(fptr)) == VALID) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = MultExpr(fptr);
        if (!rhs) {
            ERROR_MESSAGE("Invalid AddExpr in ShiftExpr", 1, lhs);
            return PARSE_FAIL(ERRP);
        }

        ParseResult operatorNode = InitASTNode();
        operatorNode->type = EXPR_NODE;
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, lhs, LHS_NODE);
        ASTPushChildNode(operatorNode, rhs, RHS_NODE);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult MultExpr(FILE* fptr)
{
    printf("Entering MultExpr\n");
    
    ParseResult lhs = PowExpr(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid PowExpr in MultExpr", 0);
        printf("Exiting PowEpxr\n");        
        return PARSE_FAIL(NAP);
    }

    while (ValidTokType(MULTS, MULTS_COUNT, PeekNextTokenP(fptr)) == VALID) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = PowExpr(fptr);
        if (!rhs) {
            ERROR_MESSAGE("Invalid PowExpr in MultExpr", 1, lhs);
            return PARSE_FAIL(ERRP);
        }

        ParseResult operatorNode = InitASTNode();
        operatorNode->type = EXPR_NODE;
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, lhs, LHS_NODE);
        ASTPushChildNode(operatorNode, rhs, RHS_NODE);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult PowExpr(FILE* fptr)
{
    printf("Entering PowExpr\n");
    
    ParseResult lhs = Prefix(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid Prefix in PowExpr", 0);
        printf("Exiting Prefix\n");        
        return PARSE_FAIL(NAP);
    }

    if (PeekNextTokenP(fptr) == POW) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = PowExpr(fptr);
        if (!rhs) {
            ERROR_MESSAGE("Invalid PowExpr in PowExpr", 1, lhs);
            return PARSE_FAIL(ERRP);
        }

        ParseResult operatorNode = InitASTNode();
        operatorNode->type = EXPR_NODE;
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, lhs, LHS_NODE);
        ASTPushChildNode(operatorNode, rhs, RHS_NODE);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult Prefix(FILE* fptr)
{
    printf("Entering Prefix\n");
    
    /* TODO: Add Casts */

    TokenType tokType = PeekNextTokenP(fptr);
    if (ValidTokType(PREFIXS, PREFIXS_COUNT, tokType) == VALID) {
        Token tok = GetNextTokenP(fptr);

        ParseResult operandNode = Prefix(fptr);
        if (!operandNode) {
            ERROR_MESSAGE("Invalid Prefix in Prefix", 0);
            return PARSE_FAIL(ERRP);
        }

        ParseResult operatorNode = InitASTNode();
        operatorNode->type = OPERATOR_NODE;
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, operandNode, OPERAND_NODE);
        return operatorNode;
    }

    ParseResult postfix = Postfix(fptr);
    if (!postfix) {
        if (PARSE_ERROR == ERRP) 
            return ERROR_MESSAGE("Invalid Postfix in Prefix", 0);
        printf("Exiting Postfix\n");
        return PARSE_FAIL(NAP);
    }

    return postfix;
}

ParseResult Postfix(FILE* fptr)
{
    printf("Entering Postfix\n");
    
    /* TODO: Include Array Indexing and Function Calling */
    ParseResult lhs = Primary(fptr);
    if (!lhs) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid Primary in Postfix", 0);
        printf("Exiting Primary\n");
        return PARSE_FAIL(NAP);
    }

    while (ValidTokType(POSTFIXS, POSTFIXS_COUNT, PeekNextTokenP(fptr)) == VALID) {
        Token tok = GetNextTokenP(fptr);

        ParseResult operatorNode = InitASTNode();
        operatorNode->type = EXPR_NODE;
        operatorNode->token = tok;

        ASTPushChildNode(operatorNode, lhs, LHS_NODE);
        lhs = operatorNode;
    }

    return lhs;
}

ParseResult Primary(FILE* fptr)
{
    printf("Entering Primary\n");
    
    /* TODO: Clean up Expr */

    if (ValidTokType(PRIMARYS, PRIMARYS_COUNT, PeekNextTokenP(fptr)) == VALID) {
        Token tok = GetNextTokenP(fptr);

        ParseResult operandNode = InitASTNode();
        operandNode->type = OPERAND_NODE;
        operandNode->token = tok;
        return operandNode;
    }  
    else if (PeekNextTokenP(fptr) == LPAREN) {
        GetNextTokenP(fptr);

        ParseResult expr = Expr(fptr);
        if (!expr) 
            return PARSE_FAIL(ERRP);
        expr->type = EXPR_NODE;

        if (CompareToken(fptr, RPAREN, "Missing )", ERRP) != VALID)
            return PARSE_FAIL(ERRP);
        return expr;
    }

    printf("Failed to Parse Primary, Climbing Tree\n");
    return PARSE_FAIL(NAP);
}

/* ---------- Etc ---------- */

ParseResult Type(FILE* fptr) 
{
    printf("Entering Type\n");
    
    if (ValidTokType(TYPES, TYPES_COUNT, PeekNextTokenP(fptr)) != VALID) 
        return PARSE_FAIL(NAP);

    Token tok = GetNextTokenP(fptr);
    ParseResult typeNode = InitASTNode();
    typeNode->token = tok;
    return typeNode;
}

ParseResult ArgList(FILE* fptr) 
{
    printf("Entering ArgList\n");
    
    ParseResult exprNode = Expr(fptr);
    if (!exprNode) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid Expr in ArgList", 0);
        return PARSE_FAIL(NAP);
    }

    ParseResult argListNode = InitASTNode();
    ASTPushChildNode(argListNode, exprNode, EXPR_NODE);

    while(true)
    {
        if (PeekNextTokenP(fptr) != COMMA)
            break;
        GetNextTokenP(fptr);

        exprNode = Expr(fptr);
        if (!exprNode) {
            ERROR_MESSAGE("Invalid Expr in ArgList", 1, argListNode);
            return PARSE_FAIL(ERRP);
        }
        ASTPushChildNode(argListNode, exprNode, EXPR_NODE);
    }

    return argListNode;
}

ParseResult VarList(FILE* fptr) 
{
    printf("Entering VarList\n");
    
    ParseResult varNode = Var(fptr);
    if (!varNode) {
        if (PARSE_ERROR == ERRP)
            return ERROR_MESSAGE("Invalid Var in VarList", 0);
        return PARSE_FAIL(NAP);
    }

    ParseResult varListNode = InitASTNode();
    ASTPushChildNode(varListNode, varNode, VAR_NODE);

    while(true)
    {
        if (PeekNextTokenP(fptr) != COMMA)
            break;
        GetNextTokenP(fptr);

        varNode = Var(fptr);
        if (!varNode) {
            ERROR_MESSAGE("Invalid Var in VarList", 1, varListNode);
            return PARSE_FAIL(ERRP);
        }
        ASTPushChildNode(varListNode, varNode, EXPR_NODE);
    }

    return varListNode;
}

ParseResult Var(FILE* fptr) 
{
    printf("Entering Var\n");
    
    ParseResult varNode;
    if (PeekNextTokenP(fptr) != IDENT) {
        if (PARSE_ERROR == ERRP) 
            return ERROR_MESSAGE("Invalid IDENT in Var", 0);
        return PARSE_FAIL(NAP);
    }

    Token tok = GetNextTokenP(fptr);
    varNode = InitASTNode();
    varNode->token = tok;
   
    if (PeekNextTokenP(fptr) == EQ) {
        GetNextTokenP(fptr);

        ParseResult exprNode = Expr(fptr);
        if (!exprNode) {
            PARSE_FAIL(ERRP);
            return ERROR_MESSAGE("Invalid Expr in Var", 1, varNode);
        }

        ASTPushChildNode(varNode, exprNode, EXPR_NODE);
    }

    return varNode;
}