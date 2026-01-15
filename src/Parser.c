#include "Parser.h"

/* ---------- HELPER ---------- */

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
int CompareToken(FILE* fptr, TokenType desired, char* errMessage, int errType)
{
    Token current = GetNextToken(fptr);
    if(current.type != desired)
    {   
        if(errType == ERRP)
            printf("ERROR: %s\n", errMessage);
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

AST* Program(FILE* fptr, AST* ast)
{
    Token t; 
    int status; 
		
    while(true)
    {
        t = GetNextTokenP(fptr);
        if(t.type == END)
            exit(0);
        PutTokenBack(&t);

        ASTNode* funcNode = InitASTNode();
        if((status = Function(fptr, funcNode)) == VALID)
        {
            ASTPushChildNode(ast->root, funcNode, FUNC_NODE);	
            continue;
        }
        else if(status == ERRP)
        { 
            printf("ERROR: Invalid Function in Program\n");
            ASTFreeNode(funcNode);
            return NULL;
        }
        
    	ASTNode* declStmtNode = InitASTNode();
        if((status = DeclStmt(fptr, declStmtNode)) == VALID)
        {
            ASTPushChildNode(ast->root, declStmtNode, DECL_STMT_NODE);
            continue;
        }
        else if(status == ERRP)
        {
            printf("ERROR: Invalid global DeclStmt in Program\n");
            ASTFreeNode(declStmtNode);
	        return NULL;
        }
        
        printf("ERROR: Unexpected token in global scope\n"); 
        break;
    }

    /* TODO: Would also have to have global vars */

    return ast;
}

int Function(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* typeNode = InitASTNode();
    if((status = Type(fptr, typeNode)) != VALID)
    {
        ASTFreeNode(typeNode);
        return status; 
    }
    ASTPushChildNode(parent, typeNode, TYPE_NODE);
    
    /* TODO: add function name to the ast */
    if(CompareToken(fptr, IDENT, "Function does not have a name", ERRP) != VALID)
        return ERRP;

    if(CompareToken(fptr, LPAREN, "Missing left parenthesis in function", ERRP) != VALID)
        return ERRP;
    
    ASTNode* paramListNode = InitASTNode();
    if((status = ParamList(fptr, paramListNode)) == ERRP)
    {
        printf("ERROR: Invalid ParamList in function\n");
        ASTFreeNode(paramListNode);
        return ERRP;
    }
    ASTPushChildNode(parent, paramListNode, PARAM_LIST_NODE);

    if(CompareToken(fptr, RPAREN, "Missing right parenthesis in function", ERRP) != VALID)
        return ERRP;

    ASTNode* bodyNode = InitASTNode();
    if((status = Body(fptr, bodyNode)) != VALID)
    {
        printf("ERROR: Invalid body in function");
        ASTFreeNode(bodyNode);
        return status;
    }
    ASTPushChildNode(parent, bodyNode, BODY_NODE);

    return VALID;
}

int ParamList(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* paramNode = InitASTNode(); 
    if((status = Param(fptr, paramNode)) != VALID)
    {
        ASTFreeNode(paramNode);
        return status;
    }
    ASTPushChildNode(parent, paramNode, PARAM_NODE);

    Token t;
    while(true)
    {
        t = GetNextToken(fptr);
        if(t.type != COMMA)
        {
            PutTokenBack(&t);
            break;
        }

        paramNode = InitASTNode(); 
        if((status = Param(fptr, paramNode)) != VALID)
        {
            ASTFreeNode(paramNode);
            return ERRP;
        }
        ASTPushChildNode(parent, paramNode, PARAM_NODE);
    }

    return VALID;
}

int Param(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* typeNode = InitASTNode();
    if((status = Type(fptr, typeNode)) != VALID)
    {
        ASTFreeNode(typeNode);
        return status; 
    }
    ASTPushChildNode(parent, typeNode, TYPE_NODE);
    
    /* TODO: have it register param name */
    if(CompareToken(fptr, IDENT, "param doesn't have a name", ERRP) != VALID)
        return ERRP;

    return VALID;
}



/* ---------- Statements ---------- */

int Body(FILE* fptr, ASTNode* parent)
{
    if(CompareToken(fptr, LBRACK, "", NAP) != VALID)
        return NAP;
    
    ASTNode* stmtListNode = InitASTNode();
    if(StmtList(fptr, stmtListNode) == ERRP)
    {
        printf("ERROR: Invalid StmtList in Body\n");
        ASTFreeNode(stmtListNode);
        return ERRP;
    }
    ASTPushChildNode(parent, stmtListNode, STMT_LIST_NODE);

    if(CompareToken(fptr, RBRACK, "Missing RBRACK in Body", ERRP) != VALID)
        return ERRP;

    return VALID;
}

int StmtList(FILE* fptr, ASTNode* parent)
{
    int status;
    while(true)
    {

        /* Mayhaps this would be better to put parent node in the Stmt function
           Because right now stmt is there between stmtlist and the actual stmt type  
        */
        ASTNode* stmtNode = InitASTNode();
        if((status = Stmt(fptr, stmtNode)) == VALID)
        {
            ASTPushChildNode(parent, stmtNode, STMT_NODE);
            continue;
        }
        ASTFreeNode(stmtNode);

        if(status == NAP)
            return VALID;

        return ERRP;
    }
    return VALID;
}

int Stmt(FILE* fptr, ASTNode* parent)
{
    /* TODO: fix this so ASTPushChildNode discerns ERRP and VALID */
    int status;

    ASTNode* ctrlStmtNode = InitASTNode();
    if((status = CtrlStmt(fptr, ctrlStmtNode)) != NAP)
    {
        ASTPushChildNode(parent, ctrlStmtNode, CTRL_STMT_NODE);
        return status;
    }
    ASTFreeNode(ctrlStmtNode);

    ASTNode* declStmtNode = InitASTNode();
	if((status = DeclStmt(fptr, declStmtNode)) != NAP)
    {
        ASTPushChildNode(parent, declStmtNode, DECL_STMT_NODE);
		return status;
    }
    ASTFreeNode(declStmtNode);

    ASTNode* exprStmtNode = InitASTNode();
	if ((status = ExprStmt(fptr, exprStmtNode)) != NAP) 
    {
        ASTPushChildNode(parent, exprStmtNode, EXPR_STMT_NODE);
		return status;
    }
    ASTFreeNode(exprStmtNode);

    ASTNode* returnStmtNode = InitASTNode();
	if ((status = ReturnStmt(fptr, returnStmtNode)) != NAP)
    {
        ASTPushChildNode(parent, returnStmtNode, RETURN_STMT_NODE);
		return status;
    }
    ASTFreeNode(returnStmtNode);

    return NAP;
}

int ExprStmt(FILE* fptr, ASTNode* parent)
{
	Token t = GetNextTokenP(fptr);
    if(t.type == SEMI)
        return VALID;
    PutTokenBack(&t);

    int status;

    ASTNode* exprNode = InitASTNode();
	if((status = Expr(fptr, exprNode)) != VALID)
    {
        ASTFreeNode(exprNode);
		return status;
    }
    ASTPushChildNode(parent, exprNode, EXPR_NODE);

    if(CompareToken(fptr, SEMI, "Semicolon missing in ExprStmt", ERRP) != VALID)
        return ERRP;

	return VALID;
}

int DeclStmt(FILE* fptr, ASTNode* parent)
{
	int status;

    ASTNode* typeNode = InitASTNode();
	if((status = Type(fptr, typeNode)) != VALID)
    {
        ASTFreeNode(typeNode);
		return status;
    }
    ASTPushChildNode(parent, typeNode, TYPE_NODE);

    ASTNode* varListNode = InitASTNode();
	if((status = VarList(fptr, varListNode)) != VALID)
    {
        printf("ERROR: Invalid VarList in DeclStmt\n");
        ASTFreeNode(varListNode);
		return ERRP;
    }
    ASTPushChildNode(parent, varListNode, VAR_LIST_NODE);

    if(CompareToken(fptr, SEMI, "Semicolon missing in DeclStmt", ERRP) != VALID)
        return ERRP;

    return VALID;
}

int CtrlStmt(FILE* fptr, ASTNode* parent)
{
    /* TODO: fix this so ASTPushChildNode discerns ERRP and VALID */
    int status;

    ASTNode* ifStmtNode = InitASTNode();
    if ((status = IfStmt(fptr, ifStmtNode)) != NAP) 
    {
        ASTPushChildNode(parent, ifStmtNode, IF_STMT_NODE);
        return status;
    }
    ASTFreeNode(ifStmtNode);

    ASTNode* switchStmtNode = InitASTNode();
    if ((status = SwitchStmt(fptr, switchStmtNode)) != NAP) 
    {
        ASTPushChildNode(parent, switchStmtNode, SWITCH_STMT_NODE);
        return status;
    }
    ASTFreeNode(switchStmtNode);

    ASTNode* whileStmtNode = InitASTNode();
    if ((status = WhileStmt(fptr, whileStmtNode)) != NAP) 
    {
        ASTPushChildNode(parent, whileStmtNode, WHILE_STMT_NODE);
        return status;
    }
    ASTFreeNode(whileStmtNode);

    ASTNode* doWhileStmtNode = InitASTNode();
    if ((status = DoWhileStmt(fptr, doWhileStmtNode)) != NAP) 
    {
        ASTPushChildNode(parent, doWhileStmtNode, DO_WHILE_STMT_NODE);
        return status;
    }
    ASTFreeNode(doWhileStmtNode);

    ASTNode* forStmtNode = InitASTNode();
    if ((status = ForStmt(fptr, forStmtNode)) != NAP) 
    {
        ASTPushChildNode(parent, forStmtNode, FOR_STMT_NODE);
        return status;
    }
    ASTFreeNode(forStmtNode);

    return NAP;
}

int ReturnStmt(FILE* fptr, ASTNode* parent)
{
    if(CompareToken(fptr, RET, "", NAP) != VALID)
        return NAP;

    int status;
    
    ASTNode* exprNode = InitASTNode();
    if((status = Expr(fptr, exprNode)) == ERRP)
    {
        printf("Return Stmt has invalid ExprStmt\n");
        ASTFreeNode(exprNode);
        return status;
    }
    if(status == VALID)
        ASTPushChildNode(parent, exprNode, EXPR_NODE);
    else 
        ASTFreeNode(exprNode);

    if(CompareToken(fptr, SEMI, "Semicolon missing in ReturnStmt", ERRP) != VALID)
        return ERRP;

	return VALID;
}


int IfStmt(FILE* fptr, ASTNode* parent)
{
    /* TODO: If body not valid, check for ExprStmt\ */
    if(CompareToken(fptr, IF, "", NAP) != VALID)
        return NAP;
    
    Token t;
	int status;
    while(true)
    {
        if(CompareToken(fptr, LPAREN, "Missing left parenthesis for IfStmt", ERRP) != VALID) 
            return ERRP;

        ASTNode* exprNode = InitASTNode();
        if((status = Expr(fptr, exprNode)) != VALID)
        {
            printf("ERROR: Invalid Expr in IfStmt\n");
            ASTFreeNode(exprNode);
            return status;
        }
        ASTPushChildNode(parent, exprNode, EXPR_NODE);

        if(CompareToken(fptr, RPAREN, "Missing right parenthesis for IfStmt", ERRP) != VALID) 
            return ERRP;
        
        ASTNode* bodyNode = InitASTNode();
        if((status = Body(fptr, bodyNode)) != VALID)
        {
            ASTFreeNode(bodyNode);
            return status; 
        }
        ASTPushChildNode(parent, bodyNode, BODY_NODE);

        t = GetNextTokenP(fptr);
        if(t.type == ELIF)
            continue;
        else if(t.type == ELSE)
        {
            bodyNode = InitASTNode();
            if((status = Body(fptr, bodyNode)) != VALID)
            {
                ASTFreeNode(bodyNode);
                return status;
            }
            ASTPushChildNode(parent, bodyNode, BODY_NODE);
            break;
        }

        PutTokenBack(&t);
        break;
    }

    return VALID;
}

int SwitchStmt(FILE* fptr, ASTNode* parent)
{
    if(CompareToken(fptr, SWITCH, "", NAP) != VALID)
        return NAP;

    if(CompareToken(fptr, LPAREN, "No LPAREN in SwitchStmt", ERRP) != VALID)
        return ERRP;

	int status;

    ASTNode* exprNode = InitASTNode();
    if((status = Expr(fptr, exprNode)) != VALID)
    {
        printf("ERROR: no bad expr in switch stmt\n");
        ASTFreeNode(exprNode);
        return ERRP;
    }
    ASTPushChildNode(parent, exprNode, EXPR_NODE);

    if(CompareToken(fptr, RPAREN, "No LPAREN in SwitchStmt", ERRP) != VALID)
        return ERRP;

    if(CompareToken(fptr, LBRACK, "No LBRACK in SwitchStmt", ERRP) != VALID)
        return ERRP;

    /* TODO: add cases */
    while(true)
    {
        if(CompareToken(fptr, CASE, "", NAP) != VALID)
            break;
        exprNode = InitASTNode();
        if((status = Expr(fptr, exprNode)) != VALID)
        {
            printf("ERROR: Invalid case in switch stmt\n");
            ASTFreeNode(exprNode);
            return ERRP;
        }
        ASTPushChildNode(parent, exprNode, EXPR_NODE);

        if(CompareToken(fptr, COLON, "No colon found for case in SwitchStmt", ERRP) != VALID)
            return ERRP;

        ASTNode* stmtListNode = InitASTNode();
        if((status = StmtList(fptr, stmtListNode)) != VALID)
        {
            printf("ERROR: Invalid StmtList in body of case in switch stmt\n");
            ASTFreeNode(stmtListNode);
            return ERRP;
        }
        ASTPushChildNode(parent, stmtListNode, STMT_LIST_NODE);

    }

    if(CompareToken(fptr, DEFAULT, "", NAP) == VALID)
    {
        if(CompareToken(fptr, COLON, "No colon found for default in SwitchStmt", ERRP) != VALID)
            return ERRP;
        
        ASTNode* stmtListNode = InitASTNode();
        if((status = StmtList(fptr, stmtListNode)) != VALID)
        {
            printf("ERROR: Invalid StmtList in body of default in switch stmt\n");
            ASTFreeNode(stmtListNode);
            return ERRP;
        }
        ASTPushChildNode(parent, stmtListNode, STMT_LIST_NODE);
    }

    if(CompareToken(fptr, RBRACK, "No RBRACK in SwitchStmt", ERRP) != VALID)
        return ERRP;

    return VALID;
}

int WhileStmt(FILE* fptr, ASTNode* parent)
{
    if(CompareToken(fptr, WHILE, "", NAP) != VALID)
        return NAP;

    if(CompareToken(fptr, LPAREN, "No LPAREN in WhileStmt", ERRP) != VALID)
        return ERRP;

    ASTNode* exprNode = InitASTNode();
    if(Expr(fptr, exprNode) != VALID)
    {
        printf("ERROR: Invalid Expr in WhileStmt\n");
        ASTFreeNode(exprNode);
        return ERRP;
    }
    ASTPushChildNode(parent, exprNode, EXPR_NODE);

    if(CompareToken(fptr, RPAREN, "No RPAREN in WhileStmt", ERRP) != VALID)
        return ERRP;

    int status;

    ASTNode* bodyNode = InitASTNode();
    if((status = Body(fptr, bodyNode)) != VALID)
    {
        ASTFreeNode(bodyNode);

        ASTNode* exprStmtNode = InitASTNode();
        if(ExprStmt(fptr, exprStmtNode) != VALID )
        {
            printf("ERROR: Invalid Body in WhileStmt\n");
            ASTFreeNode(exprStmtNode); 
            return ERRP;
        }
        ASTPushChildNode(parent, exprStmtNode, EXPR_STMT_NODE); 
    }
    if(status == VALID)
        ASTPushChildNode(parent, bodyNode, BODY_NODE);

    return VALID;
}

int DoWhileStmt(FILE* fptr, ASTNode* parent)
{
    if(CompareToken(fptr, DO, "", NAP) != VALID)
        return NAP;

    int status;

    ASTNode* bodyNode = InitASTNode();
    if((status = Body(fptr, bodyNode)) != VALID)
    {
        ASTFreeNode(bodyNode);

        ASTNode* exprStmtNode = InitASTNode();
        if(ExprStmt(fptr, exprStmtNode) != VALID )
        {
            printf("ERROR: Invalid Body after Do in DoWhileStmt\n");
            ASTFreeNode(exprStmtNode);
            return ERRP;
        }
        ASTPushChildNode(parent, exprStmtNode, EXPR_STMT_NODE);
    }
    if(status == VALID)
        ASTPushChildNode(parent, bodyNode, BODY_NODE);

    if(CompareToken(fptr, WHILE, "No while after in DoWhileStmt", ERRP) != VALID)
        return ERRP;

    if(CompareToken(fptr, LPAREN, "No LPAREN in DoWhileStmt", ERRP) != VALID)
        return ERRP;

    ASTNode* exprNode = InitASTNode();
    if(Expr(fptr, exprNode) != VALID)
    {
        printf("ERROR: Invalid Expr in while of DoWhileStmt\n");
        ASTFreeNode(exprNode);
        return ERRP;
    }
    ASTPushChildNode(parent, exprNode, EXPR_NODE);

    if(CompareToken(fptr, RPAREN, "No RPAREN in DoWhileStmt", ERRP) != VALID)
        return ERRP;

    if(CompareToken(fptr, SEMI, "No SEMI in DoWhileStmt", ERRP) != VALID)
        return ERRP;

    return VALID;
}

int ForStmt(FILE* fptr, ASTNode* parent)
{
    if(CompareToken(fptr, FOR, "", NAP) != VALID)
        return NAP;
    
    if(CompareToken(fptr, LPAREN, "No LPAREN in ForStmt", ERRP) != VALID)
        return ERRP;

    ASTNode* exprListNode = InitASTNode();
    if(ExprList(fptr, exprListNode) == ERRP)
    {
        printf("ERROR: invalid Inital Expr in for stmt\n");
        ASTFreeNode(exprListNode);
        return ERRP;
    }
    ASTPushChildNode(parent, exprListNode, EXPR_LIST_NODE);

    if(CompareToken(fptr, SEMI, "No SEMI in ForStmt", ERRP) != VALID)
        return ERRP;

    ASTNode* exprNode = InitASTNode();
    if(Expr(fptr, exprNode) == ERRP)
    {
        printf("ERROR: invalid Comparison Expr in for stmt\n");
        ASTFreeNode(exprNode);
        return ERRP;
    }
    ASTPushChildNode(parent, exprNode, EXPR_NODE);

    if(CompareToken(fptr, SEMI, "No SEMI in ForStmt", ERRP) != VALID)
        return ERRP;

    exprListNode = InitASTNode();
    if(ExprList(fptr, exprListNode) == ERRP)
    {
        printf("ERROR: invalid Increment Expr in for stmt\n");
        ASTFreeNode(exprListNode);
        return ERRP;
    }
    ASTPushChildNode(parent, exprListNode, EXPR_LIST_NODE);

    if(CompareToken(fptr, RPAREN, "No LPAREN in ForStmt", ERRP) != VALID)
        return ERRP;

    int status;

    ASTNode* bodyNode = InitASTNode();
    if((status = Body(fptr, bodyNode)) != VALID)
    {
        ASTFreeNode(bodyNode);

        ASTNode* exprStmtNode = InitASTNode();
        if(ExprStmt(fptr, exprStmtNode) != VALID )
        {
            printf("ERROR: Invalid Body after Do in ForStmt\n");
            ASTFreeNode(exprStmtNode);
            return ERRP;
        }
        ASTPushChildNode(parent, exprStmtNode, EXPR_STMT_NODE);
    }
    if(status == VALID)
        ASTPushChildNode(parent, bodyNode, BODY_NODE);

    return VALID;
}



/* ---------- Expressions ---------- */

int ExprList(FILE* fptr, ASTNode* parent)
{
    int status;
    
    ASTNode* exprNode = InitASTNode();
    if((status = Expr(fptr, exprNode)) != VALID)
    {
        ASTFreeNode(exprNode);
        return status;
    }
    ASTPushChildNode(parent, exprNode, EXPR_NODE);

    Token t;
    while(true)
    {
        if(CompareToken(fptr, COMMA, "", NAP) != VALID)
            break;

        exprNode = InitASTNode();
        if(Expr(fptr, exprNode) != VALID)
        {
            printf("ERROR: Expr in ExprList is invalid\n");
            ASTFreeNode(exprNode);
            return ERRP;
        }
        ASTPushChildNode(parent, exprNode, EXPR_NODE);
    }
    
    return VALID;
}

int Expr(FILE* fptr, ASTNode* parent)
{
	int status;

    /* For all of these pass parent to AsgnExpr so can assign the correct operator */
	if((status = AsgnExpr(fptr, parent)) != VALID)
		return status;

	return VALID;
}

int AsgnExpr(FILE* fptr, ASTNode* parent)
{
    /* TODO: Need to differentiate between Operators and Operands, only the child knows
       whether or not it contains an operator or it is passing an operand higher up the tree.
       Therefore, in this case we would need to get the tokentype from the child
    */
	int status;

    ASTNode* lhs = InitASTNode();
    if((status = OrlExpr(fptr, lhs)) != VALID)
    {
        ASTFreeNode(lhs);
        return status;
    }
    
    Token t = GetNextTokenP(fptr);
    if((status = ValidTokType(ASSIGNS, ASSIGNS_COUNT, t.type)) == VALID)
    {
        ASTNode* operatorNode = InitASTNode();
        ASTMakeTokNode(operatorNode, t);
        ASTPushChildNode(operatorNode, lhs, OPERATOR_NODE);
        
        ASTNode* rhs = InitASTNode();
        if((status = AsgnExpr(fptr, rhs)) != VALID)
        {
            printf("ERROR: improper AsgnExpr after operator in AsgnExpr\n");
            ASTFree(operatorNode);
            ASTFreeNode(rhs);
            return status;
        }

        ASTPushChildNode(operatorNode, rhs, OPERATOR_NODE);
        ASTPushChildNode(parent, operatorNode, OPERATOR_NODE);
    }
    else    
    {
        ASTPushChildNode(parent, lhs, OPERATOR_NODE);
        PutTokenBack(&t);
    }

    return VALID;
}

int OrlExpr(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* lhs = InitASTNode();
    if((status = AndlExpr(fptr, lhs)) != VALID)
    {
        ASTFreeNode(lhs);
        return status;
    }

    while(true)
    {
        ASTNode* operatorNode = NULL;
        if(!ParseOperatorToken(fptr, ORL, &operatorNode))
            break;

        ASTNode* rhs = InitASTNode();
        if(AndlExpr(fptr, rhs) != VALID)
        {
            printf("ERROR: improper AndlExpr after || in OrlExpr\n");
            ASTFreeNode(rhs);
            ASTFreeNode(operatorNode);
            return ERRP;
        }

        ASTPushChildNode(operatorNode, lhs, OPERATOR_NODE);
        ASTPushChildNode(operatorNode, rhs, OPERATOR_NODE);

        lhs = operatorNode;
    }

    ASTPushChildNode(parent, lhs, OPERATOR_NODE); 
    return VALID;
}

int AndlExpr(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* lhs = InitASTNode();
    if((status = OrExpr(fptr, lhs)) != VALID)
    {
        ASTFreeNode(lhs);
        return status;
    }

    while(true)
    {
        ASTNode* operatorNode = NULL;
        if(!ParseOperatorToken(fptr, operatorNode, ANDL, operatorNode))
        {
            ASTPushChildNode(parent, lhs, OPERATOR_NODE);
            break;
        }
        
        ASTNode* rhs = InitASTNode(); 
        if(OrExpr(fptr, orNode) != VALID)
        {
            printf("ERROR: improper OrExpr after && in AndlExpr\n");
            ASTFreeNode(orNode);
            return ERRP;
        }
        ASTPushChildNode(parent, orNode);
    }

    return VALID;
}

int OrExpr(FILE* fptr, ASTNode* parent)
{
    int status;
    
    ASTNode* xorNode = InitASTNode();
    if((status = XorExpr(fptr, xorNode)) != VALID)
    {
        ASTFreeNode(xorNode);
        return status;
    }
    ASTPushChildNode(parent, xorNode);

    Token t;
    while(true)
    {
        if(CompareToken(fptr, OR, "", NAP) != VALID)
            break;
    
        xorNode = InitASTNode();
        if(XorExpr(fptr, xorNode) != VALID)
        {
            printf("ERROR: improper XorExpr after | in OrExpr\n");
            ASTFreeNode(xorNode);
            return ERRP;
        }
        ASTPushChildNode(parent, xorNode);
    }

    return VALID;
}

int XorExpr(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* andNode = InitASTNode();
    if((status = AndExpr(fptr, andNode)) != VALID)
    {
        ASTFreeNode(andNode);
        return status;
    }
    ASTPushChildNode(parent, andNode);

    Token t;
    while(true)
    {
        if(CompareToken(fptr, XOR, "", NAP) != VALID)
            break;
    
        andNode = InitASTNode();
        if((status = AndExpr(fptr, andNode)) != VALID)
        {
            printf("ERROR: improper AndExpr after ^ in OrExpr\n");
            ASTFreeNode(andNode);
            return ERRP;
        }
        ASTPushChildNode(parent, andNode);
    }

    return VALID;
}

int AndExpr(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* eqqNode = InitASTNode();
    if((status = EqqExpr(fptr, eqqNode)) != VALID)
    {
        ASTFreeNode(eqqNode);
        return status;
    }
    ASTPushChildNode(parent, eqqNode);

    Token t;
    while(true)
    {
        if(CompareToken(fptr, AND, "", NAP) != VALID)
            break;

        eqqNode = InitASTNode();
        if(EqqExpr(fptr, eqqNode) != VALID)
        {
            printf("ERROR: improper EqqExpr after & in OrExpr\n");
            ASTFreeNode(eqqNode);
            return ERRP;
        }
        ASTPushChildNode(parent, eqqNode);
    }

    return VALID;
}

int EqqExpr(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* relationNode = InitASTNode();
    if((status = RelationExpr(fptr, relationNode)) != VALID)
    {
        ASTFreeNode(relationNode);
        return status;
    }
    ASTPushChildNode(parent, relationNode);

    Token t = GetNextTokenP(fptr);
    if(t.type == EQQ || t.type == NEQQ)
    {
        relationNode = InitASTNode();
        if(RelationExpr(fptr, relationNode) != VALID)
        {
            printf("ERROR: improper RelationExpr after == or != in EqqExpr\n");
            ASTFreeNode(relationNode);
            return ERRP;
        }
        ASTPushChildNode(parent, relationNode);
    }
    else
        PutTokenBack(&t);

    return VALID;
}

int RelationExpr(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* shiftNode = InitASTNode();
    if((status = ShiftExpr(fptr, shiftNode)) != VALID)
    {
        ASTFreeNode(shiftNode);
        return status;
    }
    ASTPushChildNode(parent, shiftNode);

    Token t = GetNextTokenP(fptr);
    if((status = ValidTokType(RELATIONAL, RELATIONAL_COUNT, t.type)) == VALID)
    {
        shiftNode = InitASTNode();
        if(ShiftExpr(fptr, shiftNode) != VALID)
        {
            printf("ERROR: improper ShiftExpr after operator in RelationExpr\n");
            ASTFreeNode(shiftNode);
            return ERRP;
        }
        ASTPushChildNode(parent, shiftNode);
    }
    else 
        PutTokenBack(&t);

    return VALID;
}

int ShiftExpr(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* addNode = InitASTNode();
    if((status = AddExpr(fptr, addNode)) != VALID)
    {
        ASTFreeNode(addNode);
        return status;
    }
    ASTPushChildNode(parent, addNode);

    Token t;
    while(true)
    {
        t = GetNextTokenP(fptr);
        if(t.type != LSHIFT && t.type != RSHIFT)
        {
            PutTokenBack(&t);
            break;
        }

        addNode = InitASTNode();
        if(AddExpr(fptr, addNode) != VALID)
        {
            printf("ERROR: improper AddExpr after << or >> in ShiftExpr\n");
            ASTFreeNode(addNode);
            return ERRP;
        }
        ASTPushChildNode(parent, addNode);
    }

    return VALID;
}

int AddExpr(FILE* fptr, ASTNode* parent)
{
	int status;

    ASTNode* multNode = InitASTNode();
    if((status = MultExpr(fptr, multNode)) != VALID)
    {
        ASTFreeNode(multNode);
        return status;
    }
    ASTPushChildNode(parent, multNode);
    
	Token t;
    while(true)
    {
        t = GetNextTokenP(fptr);
        if((status = ValidTokType(ADDS, ADDS_COUNT, t.type)) != VALID)
        {
            PutTokenBack(&t);
            break;
        }
    
        multNode = InitASTNode();
        if(MultExpr(fptr, multNode) != VALID)
        {
            printf("ERROR: improper MultExpr after + or - in AddExpr\n");
            ASTFreeNode(multNode);
            return ERRP;
        }
        ASTPushChildNode(parent, multNode);
    }

    return VALID;
}

int MultExpr(FILE* fptr, ASTNode* parent)
{
    int status;
    
    ASTNode* powNode = InitASTNode();
    if ((status = PowExpr(fptr, powNode)) != VALID)
    {
        ASTFreeNode(powNode);
        return status;
    }
    ASTPushChildNode(parent, powNode);

    Token t;
    while (true)
    {
        t = GetNextTokenP(fptr);
        if (t.type != MULT && t.type != DIV && t.type != MOD)
        {
            PutTokenBack(&t);
            break;
        }

        powNode = InitASTNode();
        if (PowExpr(fptr, powNode) != VALID)
        {
            printf("ERROR: improper PowExpr after *, / or %% in MultExpr\n");
            ASTFreeNode(powNode);
            return ERRP;
        }
        ASTPushChildNode(parent, powNode);
    }

    return VALID;
}

int PowExpr(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* prefixNode = InitASTNode();
    if ((status = Prefix(fptr, prefixNode)) != VALID)
    {
        ASTFreeNode(prefixNode);
        return status;
    }
    ASTPushChildNode(parent, prefixNode);

    if(CompareToken(fptr, POW, "", NAP) == VALID)
    {
        prefixNode = InitASTNode();
        if((status = PowExpr(fptr, prefixNode)) != VALID)
        {
            printf("ERROR: improper PowExpr after ** in PowExpr\n");
            ASTFreeNode(prefixNode);
            return ERRP;
        }
        ASTPushChildNode(parent, prefixNode);
    }

    return VALID;
}

int Prefix(FILE* fptr, ASTNode* parent)
{
    int status;
    Token t = GetNextTokenP(fptr);
    if(ValidTokType(PREFIXS, PREFIXS_COUNT, t.type) != VALID)
    {
        /* TODO: include cast here */ 
        PutTokenBack(&t);

        ASTNode* postfixNode = InitASTNode();
        if((status = Postfix(fptr, postfixNode)) == ERRP)
        {
            printf("ERROR: postfix node invalid\n");
            ASTFreeNode(postfixNode);
            return ERRP;
        }
        else if (status == VALID)
            ASTPushChildNode(parent, postfixNode);

        return status;
    }
    
    ASTNode* prefixNode = InitASTNode();
    if((status = Prefix(fptr, prefixNode)) != VALID)
    {
        ASTFreeNode(prefixNode);
        return status;
    }
    ASTPushChildNode(parent, prefixNode);

    return VALID;
}

int Postfix(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* primaryNode = InitASTNode();
    if((status = Primary(fptr, primaryNode)) != VALID)
    {
        ASTFreeNode(primaryNode);
        return status;
    }
    ASTPushChildNode(parent, primaryNode);

    Token t;
    while(true)
    {
        t = GetNextTokenP(fptr);
        if(ValidTokType(POSTFIXS, POSTFIXS_COUNT, t.type) == VALID)
            continue;

        /* [] */
        if(t.type == LBRACK)
        {
            ASTNode* exprNode = InitASTNode();
            if(Expr(fptr, exprNode) != VALID)
            {
                printf("ERROR: Invalid Expr while indexing an array\n");
                ASTFreeNode(exprNode);
                return ERRP;
            }
            ASTPushChildNode(parent, exprNode);

            if(CompareToken(fptr, RBRACK, "Missing RBRACK in while indexing an array", ERRP) != VALID)
                return ERRP;
            continue;
        }
        /* ( function ) */
        else if(t.type == LPAREN)
        {
            ASTNode* argListNode = InitASTNode();
            if(ArgList(fptr, argListNode) == ERRP)
            {
               printf("ERROR: Invalid ArgList in function calling\n");
               ASTFreeNode(argListNode);
               return ERRP;
            }
            ASTPushChildNode(parent, argListNode, ARG_LIST_NODE);

            if(CompareToken(fptr, RPAREN, "Missing RPAREN in while calling a function", ERRP) != VALID)
                return ERRP;

            continue;
        }

        PutTokenBack(&t);
        break;
    }

    return VALID;
}

int Primary(FILE* fptr, ASTNode* parent)
{
    int status;
    Token t = GetNextTokenP(fptr);

    /* TODO: have tokens be stored */
    if(t.type == IDENT || t.type == SLITERAL || t.type == CLITERAL || t.type == DECIMAL || t.type == INTEGRAL)
        return VALID;
    else if (t.type == LPAREN)
    { 
        ASTNode* exprNode = InitASTNode();
        if(Expr(fptr, exprNode) != VALID)
        {
            printf("ERROR: improper Expr after ( in Primary\n");
            ASTFreeNode(exprNode);
            return ERRP;
        }
        ASTPushChildNode(parent, exprNode, EXPR_NODE);
    
        if(CompareToken(fptr, RPAREN, "Missing RPAREN in Primary", ERRP) != VALID)
            return ERRP;
        return VALID;
    }

    PutTokenBack(&t);
    return NAP;
}

 

/* ---------- Etc ---------- */

int Type(FILE* fptr, ASTNode* parent)
{
    Token t = GetNextTokenP(fptr);
    if(ValidTokType(TYPES, TYPES_COUNT, t.type) != VALID)
    {
        PutTokenBack(&t);
        return NAP;
    }
     
    return VALID;
}

int ArgList(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* exprNode = InitASTNode();
    if((status = Expr(fptr, exprNode)) != VALID)
    {
        ASTFreeNode(exprNode);
        return status;
    }
    ASTPushChildNode(parent, exprNode, EXPR_NODE);

    while(true)
    {
        if(CompareToken(fptr, COMMA, "", NAP) != VALID)
            break;

        exprNode = InitASTNode();
        if(Expr(fptr, exprNode) != VALID)
        {
            printf("ERROR: Invalid ArgList\n");
            ASTFreeNode(exprNode);
            return ERRP;
        }
        ASTPushChildNode(parent, exprNode, EXPR_NODE);
    }
    
    return VALID;
}

int VarList(FILE* fptr, ASTNode* parent)
{
    int status;

    ASTNode* varNode = InitASTNode();
    if((status = Var(fptr, varNode)) != VALID)
    {
        ASTFreeNode(varNode);
        return status;
    }
    ASTPushChildNode(parent, varNode, VAR_NODE);

    while(true)
    {
        if(CompareToken(fptr, COMMA, "", NAP) != VALID)
            break;

        varNode = InitASTNode();
        if(Var(fptr, varNode) != VALID)
        {
            printf("ERROR: improper Var after , in VarList\n");
            ASTFreeNode(varNode);
            return ERRP;
        }
        ASTPushChildNode(parent, varNode, VAR_NODE);
    }

    return VALID;
}

int Var(FILE* fptr, ASTNode* parent)
{
    if(CompareToken(fptr, IDENT, "", NAP) != VALID)
        return NAP;

    if(CompareToken(fptr, EQ, "", NAP) != VALID)
        return VALID;

    ASTNode* exprNode = InitASTNode();
    if(Expr(fptr, exprNode) != VALID)
    {
        printf("ERROR: Invalid Expr equivalent to var\n");
        ASTFreeNode(exprNode);
        return ERRP;
    }
    ASTPushChildNode(parent, exprNode, EXPR_NODE);

    return VALID;
}
