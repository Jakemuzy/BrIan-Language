#include "Parser.h"

/* TODO: 
        - GetNextTokenP after PeekNextTokenP might be dangerous, but since the 
          preprocessor only skips directors, for now it is fine.
        - Can easily simplify most of the expressions with a helper function to reduce
          boilerplate code, which is making the file artifically longer than it needs
        - Currently freeing more than I need to since ASTFreeNodes is recursive, ONLY
          free nodes that aren't parented yet
        - DON'T FORGET TO ADD ARRAY INDEXING
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

ParseResult EmptyNode()
{
    ASTNode* emptyNode = InitASTNode();
    return PARSE_VALID(emptyNode, EMPTY_NODE);
}

ParseResult ProgNode() {
    ASTNode* progNode = InitASTNode();
    return PARSE_VALID(progNode, PROG_NODE); 
}

ParseResult ArbitraryNode(Token tok, NodeType type)
{
    ASTNode* arbitraryNode = InitASTNode();
    arbitraryNode->token = tok;
    return PARSE_VALID(arbitraryNode, type);
}

/* ---------- EBNF ---------- */

AST* Program(FILE* fptr)
{
    /* TODO: errors regarding ast freeing and progNode freeing */
    /* Allow global DeclStmts without abiguity (both start with Type Ident) */
    AST* ast = ASTInit();
    ast->root = ProgNode().node;
    ASTNode* progNode = ast->root;
    
    while (true) {
        if (PeekNextTokenP(fptr) == END)
            return ast;

        ParseResult funcNode = Function(fptr);
        if (funcNode.status == VALID) {
            ASTPushChildNode(progNode, funcNode.node);
            continue;
        } 
        else if (funcNode.status == ERRP)  {
            ASTFreeNodes(1, progNode);
            return NULL;
        }

        ParseResult declStmtNode = DeclStmt(fptr);
        if (declStmtNode.status == VALID) {
            ASTPushChildNode(progNode, declStmtNode.node);
            continue;
        }
        else if (declStmtNode.status == ERRP) {
            DEBUG_MESSAGE("Invalid DeclStmt in Global Scope");
            ASTFreeNodes(1, progNode);
            return NULL;
        }

        DEBUG_MESSAGE("Failed to Parse AST\n");
        ASTFreeNodes(1, progNode);
        return NULL;
    }


    return ast;
}

ParseResult Function(FILE* fptr)
{
    DEBUG_MESSAGE("Starting Function\n");
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
    DEBUG_MESSAGE("Starting ParamList\n");
    if (PeekNextTokenP(fptr) == RPAREN) {
        ASTNode* paramListNode = InitASTNode();
        ASTPushChildNode(paramListNode, EmptyNode().node);
        return PARSE_VALID(paramListNode, PARAM_LIST_NODE);
    }
    
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
    DEBUG_MESSAGE("Starting Param\n");

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
    DEBUG_MESSAGE("Entering Body\n");
    
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
    GetNextTokenP(fptr);

    ASTNode* bodyNode = InitASTNode();
    ASTPushChildNode(bodyNode, stmtListNode.node);
    return PARSE_VALID(bodyNode, BODY_NODE);
}

ParseResult StmtList(FILE* fptr)
{
    DEBUG_MESSAGE("Entering StmtList\n");
    
    ASTNode* stmtListNode = InitASTNode();
    if (PeekNextTokenP(fptr) == RBRACK)  {      /* Assume all StmtLists are followed by RBRACK */
        ASTPushChildNode(stmtListNode, EmptyNode().node);     
        return PARSE_VALID(stmtListNode, STMT_LIST_NODE);
    }

    while (true) {
        if (PeekNextTokenP(fptr) == RBRACK) 
            break;

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
    DEBUG_MESSAGE("Entering Stmt\n");

    ParseResult ctrlStmtNode = CtrlStmt(fptr);
    if (ctrlStmtNode.status == VALID)
        return ctrlStmtNode;    /* Child already properly sets type to valid ctrl stmt */
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
    DEBUG_MESSAGE("Entering ExprStmt\n");

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
    DEBUG_MESSAGE("Enter DeclStmt\n");
    
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
    GetNextTokenP(fptr);

    ASTNode* declStmtNode = InitASTNode();
    ASTPushChildNode(declStmtNode, typeNode.node);
    ASTPushChildNode(declStmtNode, varListNode.node);
    return PARSE_VALID(declStmtNode, DECL_STMT_NODE);
}

ParseResult CtrlStmt(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering CtrlStmt\n");
    
    ParseResult ifStmtNode = IfStmt(fptr);
    if (ifStmtNode.status == VALID)
        return PARSE_VALID(ifStmtNode.node, IF_STMT_NODE);
    else if (ifStmtNode.status == ERRP)
        return PARSE_ERRP("Invalid IfStmt in Stmt");

    ParseResult switchStmtNode = SwitchStmt(fptr);
    if (switchStmtNode.status == VALID)
        return PARSE_VALID(switchStmtNode.node, SWITCH_STMT_NODE);
    else if (switchStmtNode.status == ERRP)
        return PARSE_ERRP("Invalid SwitchStmt in Stmt");

    ParseResult whileStmtNode = WhileStmt(fptr);
    if (whileStmtNode.status == VALID)
        return PARSE_VALID(whileStmtNode.node, WHILE_STMT_NODE);
    else if (whileStmtNode.status == ERRP)
        return PARSE_ERRP("Invalid WhileStmt in Stmt");

    ParseResult doWhileStmtNode = DoWhileStmt(fptr);
    if (doWhileStmtNode.status == VALID)
        return PARSE_VALID(doWhileStmtNode.node, DO_WHILE_STMT_NODE);
    else if (doWhileStmtNode.status == ERRP)
        return  PARSE_ERRP("Invalid DoWhileStmt in Stmt");

    ParseResult forStmtNode = ForStmt(fptr);
    if (forStmtNode.status == VALID)
        return PARSE_VALID(forStmtNode.node, FOR_STMT_NODE);
    else if (forStmtNode.status == ERRP)
        return PARSE_ERRP("Invalid ForStmt in Stmt");

    DEBUG_MESSAGE("Not a CtrlStmt\n");
    return PARSE_NAP();
}

ParseResult ReturnStmt(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering ReturnStmt\n");
    
    if (PeekNextTokenP(fptr) != RET)
        return PARSE_NAP();
    GetNextTokenP(fptr);

    if (PeekNextTokenP(fptr) == SEMI) {
        GetNextTokenP(fptr);
        return EmptyNode();
    }

    ParseResult exprNode = Expr(fptr);
    if (exprNode.status != VALID)
        return PARSE_ERRP("Invalid Expr in ReturnStmt");
    
    if (PeekNextTokenP(fptr) != SEMI) {
        ASTFreeNodes(1, exprNode.node);
        return PARSE_ERRP("No semicolon in ReturnStmt");
    }
    GetNextTokenP(fptr);

    ASTNode* returnStmtNode = InitASTNode();
    ASTPushChildNode(returnStmtNode, exprNode.node);
    return PARSE_VALID(returnStmtNode, RETURN_STMT_NODE);
}

ParseResult IfStmt(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering IfStmt\n");
    
    ASTNode* ifStmtNode = InitASTNode();

    ParseResult ifNode = IfElifElse(fptr, IF);
    if (ifNode.status == ERRP)
        return PARSE_ERRP("Invalid IfStmt");
    if (ifNode.status == NAP)
        return PARSE_NAP();
    ASTPushChildNode(ifStmtNode, ifNode.node);

    while (true) {
        ParseResult elifNode = IfElifElse(fptr, ELIF);
        if (elifNode.status == ERRP) {
            ASTFreeNodes(2, ifNode.node, ifStmtNode);
            return PARSE_ERRP("Invalid Elif in IfStmt");
        }
        else if (elifNode.status == NAP)
            break;

        ASTPushChildNode(ifStmtNode, elifNode.node);
    }

    ParseResult elseNode = IfElifElse(fptr, ELSE);
    if (elseNode.status == VALID) 
        ASTPushChildNode(ifStmtNode, elseNode.node);
    else if (elseNode.status == ERRP) {
        ASTFreeNodes(2, ifNode.node, ifStmtNode);
        return PARSE_ERRP("Invalid Else in IfStmt");
    }

    return PARSE_VALID(ifStmtNode, IF_STMT_NODE);
}


ParseResult IfElifElse(FILE* fptr, TokenType type) 
{
    DEBUG_MESSAGE("Entering IfElifElse\n");
    
    if (PeekNextTokenP(fptr) != type)
        return PARSE_NAP();
    GetNextTokenP(fptr);

    ASTNode* ifElifElseNode = InitASTNode();

    if (type != ELSE) {
        if (PeekNextTokenP(fptr) != LPAREN) {
            ASTFreeNodes(1, ifElifElseNode);
            return PARSE_ERRP("Missing left parenthesis in IfStmt");
        }
        GetNextTokenP(fptr);

        ParseResult exprNode = Expr(fptr);
        if (exprNode.status != VALID) {
            ASTFreeNodes(1, ifElifElseNode);
            return PARSE_ERRP("Invalid Expr in IfStmt");
        }

        if (PeekNextTokenP(fptr) != RPAREN) {
            ASTFreeNodes(2, exprNode, ifElifElseNode);
            return PARSE_ERRP("Mising right parenthesis in IfStmt");
        }
        GetNextTokenP(fptr);

        ASTPushChildNode(ifElifElseNode, exprNode.node);
    }

    ParseResult bodyNode = Body(fptr);
    if (bodyNode.status != VALID) {
        ASTFreeNodes(1, ifElifElseNode);
        return PARSE_ERRP("Invaldi Body in IfStmt");
        
    }
    ASTPushChildNode(ifElifElseNode, bodyNode.node);

    NodeType nodeType;      /* A bit unorthodox, not sure if incorrect */
    if (type == IF) nodeType = IF_NODE;
    else if (type == ELIF) nodeType = ELIF_NODE;
    else nodeType = ELSE_NODE;

    return PARSE_VALID(ifElifElseNode, nodeType);
}

ParseResult SwitchStmt(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering SwtichStmt\n");
    
    ASTNode* switchStmtNode = InitASTNode();

    if (PeekNextTokenP(fptr) != SWITCH)
        return PARSE_NAP();
    GetNextTokenP(fptr);

    if (PeekNextTokenP(fptr) != LPAREN) {
        ASTFreeNodes(1, switchStmtNode);
        return PARSE_ERRP("No Left Parenthesis in SwitchStmt");
    }
    GetNextTokenP(fptr);
    
    ParseResult exprNode = Expr(fptr);
    if (exprNode.status != VALID) {
        ASTFreeNodes(1, switchStmtNode);
        return PARSE_ERRP("Invalid Expr in SwitchStmt");
    }
    ASTPushChildNode(switchStmtNode, exprNode.node);

    if (PeekNextTokenP(fptr) != RPAREN) {
        ASTFreeNodes(2, exprNode, switchStmtNode);
        return PARSE_ERRP("No Right Parenthesis in SwitchStmt");
    }
    GetNextTokenP(fptr);

    if (PeekNextTokenP(fptr) != LBRACK) {
        ASTFreeNodes(2, exprNode, switchStmtNode);
        return PARSE_ERRP("No Left Bracket in SwitchStmt");
    }
    GetNextTokenP(fptr);

    while (true) {
        ParseResult caseNode = Case(fptr);
        if (caseNode.status == ERRP) {
            ASTFreeNodes(1, switchStmtNode);
            return PARSE_ERRP("Invalid Case in SwitchStmt");
        }
        else if (caseNode.status == NAP)
            break;

        ASTPushChildNode(switchStmtNode, caseNode.node);
    }

    ParseResult defaultNode = Default(fptr);
    if (defaultNode.status == VALID)
        ASTPushChildNode(switchStmtNode, defaultNode.node);
    else if (defaultNode.status == ERRP) {
        ASTFreeNodes(2, exprNode.node, switchStmtNode);
        return PARSE_ERRP("Invalid Default in SwitchStmt");
    }

    if (PeekNextTokenP(fptr) != RBRACK) {
        ASTFreeNodes(3, exprNode.node, defaultNode.node, switchStmtNode);
        return PARSE_ERRP("No Right Bracket in SwitchStmt");
    }
    GetNextTokenP(fptr);

    return PARSE_VALID(switchStmtNode, SWITCH_STMT_NODE);
}

ParseResult Case(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering Case\n");
    
    if (PeekNextTokenP(fptr) != CASE)
        return PARSE_NAP();
    GetNextTokenP(fptr);

    ParseResult exprNode = Expr(fptr);
    if (exprNode.status != VALID)
        return PARSE_ERRP("Invalid Expr in Case");

    ParseResult bodyNode = Body(fptr);
    if (bodyNode.status != VALID) {
        ASTFreeNodes(1, exprNode.node);
        return PARSE_ERRP("Invalid Body in Case");
    }

    ASTNode* caseNode = InitASTNode();
    ASTPushChildNode(caseNode, exprNode.node);
    ASTPushChildNode(caseNode, bodyNode.node);
    return PARSE_VALID(caseNode, CASE_NODE);
}

ParseResult Default(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering Default\n");
    
    if (PeekNextTokenP(fptr) != DEFAULT)
        return PARSE_NAP();
    GetNextTokenP(fptr);

    ParseResult bodyNode = Body(fptr);
    if (bodyNode.status != VALID)
        return PARSE_ERRP("Invalid Body in Default");

    ASTNode* defaultNode = InitASTNode();
    ASTPushChildNode(defaultNode, bodyNode.node);
    return PARSE_VALID(defaultNode, DEFAULT_NODE);
}

ParseResult WhileStmt(FILE* fptr)
{
    DEBUG_MESSAGE("Entering WhileStmt\n");
    
    if (PeekNextTokenP(fptr) != WHILE)
        return PARSE_NAP();
    GetNextTokenP(fptr);

    if (PeekNextTokenP(fptr) != LPAREN)
        return PARSE_ERRP("No left parenthesis in WhileStmt");
    GetNextTokenP(fptr);

    ParseResult exprNode = Expr(fptr);
    if (exprNode.status != VALID)
        return PARSE_ERRP("Invalid Expr in WhileStmt");

    if (PeekNextTokenP(fptr) != RPAREN) {
        ASTFreeNodes(1, exprNode.node);
        return PARSE_ERRP("No right parenthesis in WhileStmt");
    }
    GetNextTokenP(fptr);
    
    ParseResult bodyNode = Body(fptr);
    if (bodyNode.status != VALID) {
        ASTFreeNodes(1, exprNode.node);
        return PARSE_ERRP("Invalid Body in WhileStmt");
    }

    ASTNode* whileStmtNode = InitASTNode();
    ASTPushChildNode(whileStmtNode, exprNode.node);
    ASTPushChildNode(whileStmtNode, bodyNode.node);
    return PARSE_VALID(whileStmtNode, WHILE_STMT_NODE);
}

ParseResult DoWhileStmt(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering DoWhileStmt\n");
    
    if (PeekNextTokenP(fptr) != DO)
        return PARSE_NAP();
    GetNextTokenP(fptr);

    ParseResult bodyNode = Body(fptr);
    if (bodyNode.status != VALID) 
        return PARSE_ERRP("Invalid Body in DoWhileStmt");

    if (PeekNextTokenP(fptr) != WHILE) {
        ASTFreeNodes(1, bodyNode);
        return PARSE_ERRP("No While found in DoWhileStmt");
    }
    GetNextTokenP(fptr);

    if (PeekNextTokenP(fptr) != LPAREN){
        ASTFreeNodes(1, bodyNode);
        return PARSE_ERRP("No left parenthesis in DoWhileStmt");
    }
    GetNextTokenP(fptr);

    ParseResult exprNode = Expr(fptr);
    if (exprNode.status != VALID) {
        ASTFreeNodes(1, bodyNode);
        return PARSE_ERRP("Invalid Expr in WhileStmt");
    }

    if (PeekNextTokenP(fptr) != RPAREN) {
        ASTFreeNodes(2, bodyNode.node, exprNode.node);
        return PARSE_ERRP("No right parenthesis in DoWhileStmt");
    }
    GetNextTokenP(fptr);

    if (PeekNextTokenP(fptr) != SEMI) {
        ASTFreeNodes(2, bodyNode.node, exprNode.node);
        return PARSE_ERRP("No semicolon in DoWhileStmt");
    }
    GetNextTokenP(fptr);

    ASTNode* doWhileStmtNode = InitASTNode();
    ASTPushChildNode(doWhileStmtNode, bodyNode.node);
    ASTPushChildNode(doWhileStmtNode, exprNode.node);
    return PARSE_VALID(doWhileStmtNode, DO_WHILE_STMT_NODE);
}

ParseResult ForStmt(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering ForStmt\n");
    
    /* TODO: Expr does not check for semi colon, therefore empty exprs not allowed rn
       Need to have a helper function to determine if it is valid.
    */

    if (PeekNextTokenP(fptr) != FOR)
        return PARSE_NAP();
    GetNextTokenP(fptr);

    if (PeekNextTokenP(fptr) != LPAREN) 
       return PARSE_ERRP("No left parenthesis found in ForStmt"); 
    GetNextTokenP(fptr);

    ParseResult exprListNode = ExprList(fptr);
    if (exprListNode.status != VALID) 
        return PARSE_ERRP("Invalid ExprList in ForStmt");

    if (PeekNextTokenP(fptr) != SEMI)  {
        ASTFreeNodes(1, exprListNode.node);
        return PARSE_ERRP("Missing semicolon in ForStmt"); 
    }
    GetNextTokenP(fptr);

    ParseResult exprNode = OptionalExpr(fptr);
    if (exprNode.status != VALID) {
        ASTFreeNodes(1, exprListNode.node);
        return PARSE_ERRP("Invalid Expr in ForStmt");
    }

    if (PeekNextTokenP(fptr) != SEMI)  {
        ASTFreeNodes(2, exprListNode.node, exprNode.node);
        return PARSE_ERRP("Missing semicolon in ForStmt"); 
    }
    GetNextTokenP(fptr);

    ParseResult exprListNode2 = ExprList(fptr);
    if (exprListNode2.status != VALID) {
        ASTFreeNodes(2, exprListNode.node, exprNode.node);
        return PARSE_ERRP("Invalid ExprList in ForStmt");
    }

    if (PeekNextTokenP(fptr) != RPAREN)  {
        ASTFreeNodes(3, exprListNode.node, exprNode.node, exprListNode2.node);
        return PARSE_ERRP("No right parenthesis in ForStmt"); 
    }
    GetNextTokenP(fptr);
    
    ParseResult bodyNode = Body(fptr);
    if (bodyNode.status != VALID) {
        ASTFreeNodes(3, exprListNode.node, exprNode.node, exprListNode2.node);
        return PARSE_ERRP("Invalid Body in ForStmt");
    }

    ASTNode* forStmtNode = InitASTNode();
    ASTPushChildNode(forStmtNode, exprListNode.node);
    ASTPushChildNode(forStmtNode, exprNode.node);
    ASTPushChildNode(forStmtNode, exprListNode2.node);
    return PARSE_VALID(forStmtNode, FOR_STMT_NODE);
}

ParseResult OptionalExpr(FILE* fptr) 
{
    /* TODO: A helper fucntion here would probably be benficial to determine starts to Expr */
    if (PeekNextTokenP(fptr) == SEMI)
        return EmptyNode();

    ParseResult exprNode = Expr(fptr);
    if (exprNode.status != VALID) 
        return PARSE_ERRP("Invalid Optional Expr");
    
    return exprNode;
}

/* ----------- Expressions ---------- */

ParseResult ExprList(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering ExprList\n");

    if (PeekNextTokenP(fptr) == SEMI || PeekNextTokenP(fptr) == RPAREN)     /* Empty ExprList in ForStmt */
        return EmptyNode();


    ParseResult exprNode = Expr(fptr);
    if (exprNode.status == ERRP) 
        return PARSE_ERRP("Invalid Expr in ExprList"); 
    else if (exprNode.status == NAP) 
        return PARSE_NAP();

    ASTNode* exprListNode = InitASTNode();
    ASTPushChildNode(exprListNode, exprNode.node);

    while (true) {
        if (PeekNextTokenP(fptr) != COMMA) 
            break;
        GetNextTokenP(fptr);

        exprNode = Expr(fptr);
        if (exprNode.status != VALID) {
            ASTFreeNodes(1, exprListNode);
            return PARSE_ERRP("Invalid Expr in ExprList");
        }

        ASTPushChildNode(exprListNode, exprNode.node);
    }

    return PARSE_VALID(exprListNode, EXPR_LIST_NODE);
}

ParseResult Expr(FILE* fptr) 
{   
    DEBUG_MESSAGE("Entering Expr\n");
    
    /* TODO: Technically an Alias for AsgnEpxr, but allows for easier readability */
    return AsgnExpr(fptr);
}

ParseResult AsgnExpr(FILE* fptr)
{
    DEBUG_MESSAGE("Entering AsgnExpr\n");
    
    ParseResult lhs = OrlExpr(fptr);
    if (lhs.status == ERRP) 
        return PARSE_ERRP("Invalid OrlExpr in AsgnExpr");
    else if (lhs.status == NAP)
        return PARSE_NAP();

    if (ValidTokType(ASSIGNS, ASSIGNS_COUNT, PeekNextTokenP(fptr)) == VALID) 
    {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = AsgnExpr(fptr);
        if (rhs.status != VALID) {
            ASTFreeNodes(1, lhs.node);
            return PARSE_ERRP("Invalid AsgnExpr in AsgnExpr");
        }
        
        ParseResult operatorNode = ArbitraryNode(tok, ASGN_EXPR_NODE);

        ASTPushChildNode(operatorNode.node, lhs.node);
        ASTPushChildNode(operatorNode.node, rhs.node);

        lhs = operatorNode;
    }
    return lhs;
}

ParseResult OrlExpr(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering OrlExpr\n");
    
    ParseResult lhs = AndlExpr(fptr);
    if (lhs.status == ERRP)
        return PARSE_ERRP("Invalid AndlExpr in OrlExpr");
    else if (lhs.status == NAP)
        return PARSE_NAP();

    while (PeekNextTokenP(fptr) == ORL) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = AndlExpr(fptr);
        if (rhs.status != VALID) {
            ASTFreeNodes(1, lhs.node);
            return PARSE_ERRP("Invalid AndlExpr in OrlExpr");
        }

        ParseResult operatorNode = ArbitraryNode(tok, BINARY_EXPR_NODE);

        ASTPushChildNode(operatorNode.node, lhs.node);
        ASTPushChildNode(operatorNode.node, rhs.node);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult AndlExpr(FILE* fptr)
{
    DEBUG_MESSAGE("Entering AndlExpr\n");
    
    ParseResult lhs = OrExpr(fptr);
    if (lhs.status == ERRP) 
        return PARSE_ERRP("invalid OrExpr in AndlExpr");
    else if (lhs.status == NAP)
        return PARSE_NAP();

    while (PeekNextTokenP(fptr) == ANDL) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = XorExpr(fptr);
        if (rhs.status != VALID) {
            ASTFreeNodes(1, lhs.node);
            return PARSE_ERRP("invalid OrEpxr in AndlExpr");
        }

        ParseResult operatorNode = ArbitraryNode(tok, BINARY_EXPR_NODE);
        ASTPushChildNode(operatorNode.node, lhs.node);
        ASTPushChildNode(operatorNode.node, rhs.node);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult OrExpr(FILE* fptr)
{
    DEBUG_MESSAGE("Entering OrExpr\n");
    
    ParseResult lhs = XorExpr(fptr);
    if (lhs.status == ERRP)
        return PARSE_ERRP("Invalid XorExpr in OrExpr");
    else if (lhs.status == NAP)
        return PARSE_NAP();

    while (PeekNextTokenP(fptr) == OR) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = XorExpr(fptr);
        if (rhs.status != VALID) {
            ASTFreeNodes(1, lhs.node);
            return PARSE_ERRP("Invalid XorExpr in OrExpr");
        }

        ParseResult operatorNode = ArbitraryNode(tok, BINARY_EXPR_NODE);
        ASTPushChildNode(operatorNode.node, lhs.node);
        ASTPushChildNode(operatorNode.node, rhs.node);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult XorExpr(FILE* fptr)
{
    DEBUG_MESSAGE("Entering XorExpr\n");
    
    ParseResult lhs = AndExpr(fptr);
    if (lhs.status == ERRP)
        return PARSE_ERRP("Invalid AndExpr in XorExpr");
    else if (lhs.status == NAP)
        return PARSE_NAP();

    while (PeekNextTokenP(fptr) == XOR) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = AndExpr(fptr);
        if (rhs.status != VALID) {
            ASTFreeNodes(1, lhs.node);
            return PARSE_ERRP("Invalid AndExpr in XorExpr");
        }

        ParseResult operatorNode = ArbitraryNode(tok, BINARY_EXPR_NODE);
        ASTPushChildNode(operatorNode.node, lhs.node);
        ASTPushChildNode(operatorNode.node, rhs.node);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult AndExpr(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering AndExpr\n");
    
    ParseResult lhs = EqqExpr(fptr);
    if (lhs.status == ERRP)
        return PARSE_ERRP("invalid EqqExpr in AndExpr");
    else if (lhs.status == NAP)
        return PARSE_NAP();

    while (PeekNextTokenP(fptr) == AND) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = EqqExpr(fptr);
        if (rhs.status != VALID) {
            ASTFreeNodes(1, lhs.node);
            return PARSE_ERRP("Invalid EqqExpr in AndExpr");
        }

        ParseResult operatorNode = ArbitraryNode(tok, BINARY_EXPR_NODE);
        ASTPushChildNode(operatorNode.node, lhs.node);
        ASTPushChildNode(operatorNode.node, rhs.node);
        lhs = operatorNode;
    }

    return lhs;
}

ParseResult EqqExpr(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering EqqExpr\n");
    
    ParseResult lhs = RelationExpr(fptr);
    if (lhs.status == ERRP)
        return PARSE_ERRP("invalid RelationExpr in EqqExpr");
    else if (lhs.status == NAP)
        return PARSE_NAP();

    TokenType tokType = PeekNextTokenP(fptr);
    if (tokType == EQQ || tokType == NEQQ) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = RelationExpr(fptr);
        if (rhs.status != VALID) {
            ASTFreeNodes(1, lhs.node);
            return PARSE_ERRP("Invalid RelationExpr in EqqExpr");
        }

        ParseResult operatorNode = ArbitraryNode(tok, BINARY_EXPR_NODE);
        ASTPushChildNode(operatorNode.node, lhs.node);
        ASTPushChildNode(operatorNode.node, rhs.node);
        lhs = operatorNode;
    }

    return lhs;
}

ParseResult RelationExpr(FILE* fptr)
{
    DEBUG_MESSAGE("Entering RelationExpr\n");
    
    ParseResult lhs = ShiftExpr(fptr);
    if (lhs.status == ERRP)
        return PARSE_ERRP("invalid ShiftExpr in RelationExpr");
    else if (lhs.status == NAP)
        return PARSE_NAP();

    TokenType tokType = PeekNextTokenP(fptr);
    if (ValidTokType(RELATIONAL, RELATIONAL_COUNT, tokType) == VALID) 
    {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = ShiftExpr(fptr);
        if (rhs.status != VALID) {
            ASTFreeNodes(1, lhs.node);
            return PARSE_ERRP("Invalid ShftExpr in RelationExpr");
        }

        ParseResult operatorNode = ArbitraryNode(tok, BINARY_EXPR_NODE);
        ASTPushChildNode(operatorNode.node, lhs.node);
        ASTPushChildNode(operatorNode.node, rhs.node);
        lhs = operatorNode;
    }

    return lhs;
}

ParseResult ShiftExpr(FILE* fptr)
{
    DEBUG_MESSAGE("Entering ShiftExpr\n");
    
    ParseResult lhs = AddExpr(fptr);
    if (lhs.status == ERRP)
        return PARSE_ERRP("Invalid AddExpr in ShiftExpr");
    else if (lhs.status == NAP)
        return PARSE_NAP();

    TokenType tokType = PeekNextTokenP(fptr);;
    while (tokType == LSHIFT || tokType == RSHIFT) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = MultExpr(fptr);
        if (rhs.status != VALID) {
            ASTFreeNodes(1, lhs.node);
            return PARSE_ERRP("Invalid AddExpr in ShfitExpr");
        }

        ParseResult operatorNode = ArbitraryNode(tok, BINARY_EXPR_NODE);
        ASTPushChildNode(operatorNode.node, lhs.node);
        ASTPushChildNode(operatorNode.node, rhs.node);

        lhs = operatorNode;
    }

    return lhs;
}

ParseResult AddExpr(FILE* fptr)
{
    DEBUG_MESSAGE("Entering AddExpr\n");
    
    ParseResult lhs = MultExpr(fptr);
    if (lhs.status == ERRP)
        return PARSE_ERRP("Invalid MultExpr in AddExpr");
    else if (lhs.status == NAP)
        return PARSE_NAP();

    while (ValidTokType(ADDS, ADDS_COUNT, PeekNextTokenP(fptr)) == VALID) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = MultExpr(fptr);
        if (rhs.status != VALID) {
            ASTFreeNodes(1, lhs.node);
            return PARSE_ERRP("Invalid MultExpr in AddExpr");
        }

        ParseResult operatorNode = ArbitraryNode(tok, BINARY_EXPR_NODE);
        ASTPushChildNode(operatorNode.node, lhs.node);
        ASTPushChildNode(operatorNode.node, rhs.node);
        lhs = operatorNode;
    }

    return lhs;
}

ParseResult MultExpr(FILE* fptr)
{
    DEBUG_MESSAGE("Entering MultExpr\n");
    
    ParseResult lhs = PowExpr(fptr);
    if (lhs.status == ERRP)
        return PARSE_ERRP("Invalid PowExpr in MultExpr");
    else if (lhs.status == NAP)
        return PARSE_NAP();

    while (ValidTokType(MULTS, MULTS_COUNT, PeekNextTokenP(fptr)) == VALID) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = PowExpr(fptr);
        if (rhs.status != VALID) {
            ASTFreeNodes(1, lhs.node);
            return PARSE_ERRP("Invalid PowExpr in MultExpr");
        }

        ParseResult operatorNode = ArbitraryNode(tok, BINARY_EXPR_NODE);
        ASTPushChildNode(operatorNode.node, lhs.node);
        ASTPushChildNode(operatorNode.node, rhs.node);
        lhs = operatorNode;
    }

    return lhs;
}

ParseResult PowExpr(FILE* fptr)
{
    DEBUG_MESSAGE("Entering PowExpr\n");
    
    ParseResult lhs = Prefix(fptr);
    if (lhs.status == ERRP)
        return PARSE_ERRP("Invalid Prefix in PowExpr");
    else if (lhs.status == NAP)
        return PARSE_NAP();

    if (PeekNextTokenP(fptr) == POW) {
        Token tok = GetNextTokenP(fptr);

        ParseResult rhs = PowExpr(fptr);
        if (rhs.status != VALID) {
            ASTFreeNodes(1, lhs.node);
            return PARSE_ERRP("Invalid PowExpr in PowExpr");
        }

        ParseResult operatorNode = ArbitraryNode(tok, BINARY_EXPR_NODE);
        ASTPushChildNode(operatorNode.node, lhs.node);
        ASTPushChildNode(operatorNode.node, rhs.node);
        lhs = operatorNode;
    }

    return lhs;
}

ParseResult Prefix(FILE* fptr)
{
    DEBUG_MESSAGE("Entering Prefix\n");
    
    /* TODO: Add Casts */

    if (ValidTokType(PREFIXS, PREFIXS_COUNT, PeekNextTokenP(fptr)) == VALID) {
        Token tok = GetNextTokenP(fptr);

        ParseResult operandNode = Prefix(fptr);
        if (operandNode.status != VALID) 
            return PARSE_ERRP("Invalid Prefix in Prefix");

        ParseResult operatorNode = ArbitraryNode(tok, UNARY_EXPR_NODE);
        ASTPushChildNode(operatorNode.node, operandNode.node);
        return operatorNode;
    }

    ParseResult postfix = Postfix(fptr);
    if (postfix.status == ERRP)
        return PARSE_ERRP("Invalid Postfix in Prefix");
    else if (postfix.status == NAP)
        return PARSE_NAP();

    return postfix;
}

ParseResult Postfix(FILE* fptr)
{
    DEBUG_MESSAGE("Entering Postfix\n");
    
    /* TODO: Include Array Indexing and Function Calling */
    ParseResult lhs = Primary(fptr);
    if (lhs.status == ERRP)
        return PARSE_ERRP("Invalid Primary in Postfix");
    else if (lhs.status == NAP)
        return PARSE_NAP();

    while (true) {
        TokenType tokType = PeekNextTokenP(fptr);

        if (tokType == LPAREN) {
            ParseResult callFuncNode = CallFunc(fptr, lhs.node);
            if (callFuncNode.status != VALID) {
                ASTFreeNodes(1, lhs.node);
                return PARSE_ERRP("Invalid Function Call in Postfix");
            }
            lhs = callFuncNode;
        }
        else if (tokType == LBRACE) {
            /* Array Index */
        }
        else if (ValidTokType(POSTFIXS, POSTFIXS_COUNT, tokType) == VALID ) {
            Token tok = GetNextTokenP(fptr);
            ParseResult operatorNode = ArbitraryNode(tok, UNARY_EXPR_NODE);
            ASTPushChildNode(operatorNode.node, lhs.node);
            lhs = operatorNode;
        }
        else 
            break;
    }

    return lhs;
}

ParseResult CallFunc(FILE* fptr, ASTNode* callee)
{
    if (PeekNextTokenP(fptr) != LPAREN)
        return PARSE_NAP();
    GetNextTokenP(fptr);    /* Postfix already checks for LPAREN, I just thought it would be cleaner and more true to the grammar to have it here*/

    ParseResult argListNode = ArgList(fptr);
    if (argListNode.status != VALID) 
        return PARSE_ERRP("Invalid Argument List in Function Call");

    if (PeekNextTokenP(fptr) != RPAREN) {
        ASTFreeNodes(1, argListNode.node);
        return PARSE_ERRP("No Right Parenthesis in Function Call");
    }
    GetNextTokenP(fptr);

    ASTNode* callFuncNode = InitASTNode();
    ASTPushChildNode(callFuncNode, callee);
    ASTPushChildNode(callFuncNode, argListNode.node);
    return PARSE_VALID(callFuncNode, CALL_FUNC_NODE);
}

ParseResult Primary(FILE* fptr)
{
    DEBUG_MESSAGE("Entering Primary\n");
    
    /* TODO: Clean up Expr */

    if (ValidTokType(PRIMARYS, PRIMARYS_COUNT, PeekNextTokenP(fptr)) == VALID) {
        Token tok = GetNextTokenP(fptr);
        
        ParseResult operandNode;
        if (tok.type == IDENT)
            operandNode = ArbitraryNode(tok, IDENT_NODE);
        else 
            operandNode = ArbitraryNode(tok, LITERAL_NODE);
        return operandNode;
    } 
    /* Check Parenthesis Helper Function Needed */
    else if (PeekNextTokenP(fptr) == LPAREN) {
        GetNextTokenP(fptr);

        ParseResult exprNode = Expr(fptr);
        if (exprNode.status != VALID) 
            return PARSE_ERRP("Missing Expr in parenthesized Expr");
    
        if (PeekNextTokenP(fptr) != RPAREN) {
            ASTFreeNodes(1, exprNode);
            return PARSE_ERRP("Missing right parenthesis in parenthesized Epxr");
        }
        GetNextTokenP(fptr);

        return exprNode;
    }

    DEBUG_MESSAGE("Failed to Parse Primary, Climbing Tree\n");
    return PARSE_NAP();
}

/* ---------- Etc ---------- */

ParseResult Type(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering Type\n");
    
    if (ValidTokType(TYPES, TYPES_COUNT, PeekNextTokenP(fptr)) != VALID) 
        return PARSE_NAP();
    Token tok = GetNextTokenP(fptr);

    ParseResult typeNode = ArbitraryNode(tok, TYPE_NODE);
    return typeNode;
}

ParseResult ArgList(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering ArgList\n");
    if (PeekNextTokenP(fptr) == RPAREN) {
        ASTNode* argListNode = InitASTNode();
        ASTPushChildNode(argListNode, EmptyNode().node);
        return PARSE_VALID(argListNode, ARG_LIST_NODE);
    }
    
    ParseResult exprNode = Expr(fptr);
    if (exprNode.status == ERRP) 
        return PARSE_ERRP("Invalid Expr in ArgList");
    else if (exprNode.status == NAP)
        return PARSE_NAP();
    
    ASTNode* argListNode = InitASTNode();
    ASTPushChildNode(argListNode, exprNode.node);

    while(true)
    {
        if (PeekNextTokenP(fptr) != COMMA)
            break;
        GetNextTokenP(fptr);

        exprNode = Expr(fptr);
        if (exprNode.status != VALID) {
            ASTFreeNodes(1, argListNode);
            return PARSE_ERRP("Invalid Expr in ArgList");
        }
        ASTPushChildNode(argListNode, exprNode.node);
    }

    return PARSE_VALID(argListNode, ARG_LIST_NODE);
}

ParseResult VarList(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering VarList\n");
    
    ParseResult varNode = Var(fptr);
    if (varNode.status == ERRP)
        return PARSE_ERRP("Invalid Var in VarList");
    else if (varNode.status == NAP)
        return PARSE_NAP();

    ASTNode* varListNode = InitASTNode();
    ASTPushChildNode(varListNode, varNode.node);

    while(true)
    {
        if (PeekNextTokenP(fptr) != COMMA)
            break;
        GetNextTokenP(fptr);

        varNode = Var(fptr);
        if (varNode.status != VALID) {
            ASTFreeNodes(1, varListNode);
            return PARSE_ERRP("Invalid Var in VarList");
        }

        ASTPushChildNode(varListNode, varNode.node);
    }

    return PARSE_VALID(varListNode, VAR_LIST_NODE);
}

ParseResult Var(FILE* fptr) 
{
    DEBUG_MESSAGE("Entering Var\n");
    
    if (PeekNextTokenP(fptr) != IDENT) 
        return PARSE_NAP();
    ParseResult identNode = IdentNode(GetNextTokenP(fptr));

    ASTNode* varNode = InitASTNode();
    ASTPushChildNode(varNode, identNode.node);

    if (PeekNextTokenP(fptr) == EQ) {
        GetNextTokenP(fptr);

        ParseResult exprNode = Expr(fptr);
        if (exprNode.status != VALID) {
            ASTFreeNodes(1, identNode);
            return PARSE_ERRP("Invalid Expr in Var");
        }

        ASTPushChildNode(varNode, exprNode.node);
    }

    return PARSE_VALID(varNode, VAR_NODE);
}