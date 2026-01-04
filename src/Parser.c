#include "Parser.h"

/* TODO:
 * 
*/

/* ---------- AST ----------- */

AST* ASTInit(FILE* fptr)
{
    AST* ast = malloc(sizeof(AST));
    
    ast->root = malloc(sizeof(ASTNode));
    ast->root->children = NULL;
    ast->root->parent = NULL;
    ast->root->childCount = 0;

    ast->leaf = ast->root;
    return ast;
}

void ASTAddChild(AST* ast, Token t)
{
    ASTNode* leaf = ast->leaf;

    int childIndex = leaf->childCount;

    leaf->children = realloc(leaf->children, (childIndex + 1) * sizeof(ASTNode*));

    ASTNode* child = malloc(sizeof(ASTNode));
    child->children = NULL;
    child->parent = leaf;
    child->childCount = 0;
    child->token = t;

    (leaf->children)[childIndex] = child;
    leaf->childCount++;

    ast->leaf = child;
}

void TraverseUpAST(AST* ast)
{
    if(ast->leaf->parent)
        ast->leaf = ast->leaf->parent;
}

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
    Token current  = GetNextToken(fptr);
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

/* ---------- EBNF ---------- */

void Program(FILE* fptr, AST* ast)
{
    Token t; 
    int status; 
    while(true)
    {
        t = GetNextTokenP(fptr);
        if(t.type == END)
            exit(0);
        PutTokenBack(&t);

        if((status = Function(fptr, ast)) == VALID)
            continue;
        else if(status == ERRP)
        {
            printf("ERROR: Invalid Function in Program\n");
            exit(1);
        }

        if((status = DeclStmt(fptr, ast)) == VALID)
            continue;
        else if(status == ERRP)
        {
            printf("ERROR: Invalid global DeclStmt in Program\n");
            exit(1);
        }
        
        printf("ERROR: Unexpected token in global scope\n"); 
        break;
    }

    exit(1);

}

int Function(FILE* fptr, AST* ast)
{
    /* TODO: Would also have to have global vars */
    int status;
    if((status = Type(fptr, ast)) != VALID)
       return status; 

    if(CompareToken(fptr, IDENT, "Function does not have a name", ERRP) != VALID)
        return ERRP;

    if(CompareToken(fptr, LPAREN, "Missing left parenthesis in function", ERRP) != VALID)
        return ERRP;

    if((status = ParamList(fptr, ast)) == ERRP)
    {
        printf("ERROR: Invalid ParamList in function\n");
        return ERRP;
    }

    if(CompareToken(fptr, RPAREN, "Missing right parenthesis in function", ERRP) != VALID)
        return ERRP;

    if((status = Body(fptr, ast)) != VALID)
    {
        printf("ERROR: Invalid body in function");
        return status;
    }

    return VALID;
}

int ParamList(FILE* fptr, AST* ast)
{
    int status;
    if((status = Param(fptr, ast)) != VALID)
        return status;

    Token t;
    while(true)
    {
        t = GetNextToken(fptr);
        if(t.type != COMMA)
        {
            PutTokenBack(&t);
            break;
        }

        if((status = Param(fptr, ast)) != VALID)
            return ERRP;
    }

    return VALID;
}

int Param(FILE* fptr, AST* ast)
{
    int status;
    if((status = Type(fptr, ast)) != VALID)
       return status; 

    if(CompareToken(fptr, IDENT, "param doesn't have a name", ERRP) != VALID)
        return ERRP;

    return VALID;
}



/* ---------- Statements ---------- */

int Body(FILE* fptr, AST* ast)
{
    if(CompareToken(fptr, LBRACK, "", NAP) != VALID)
        return NAP;

    if(StmtList(fptr, ast) == ERRP)
    {
        printf("ERROR: Invalid StmtList in Body\n");
        return ERRP;
    }

    if(CompareToken(fptr, RBRACK, "Missing RBRACK in Body", ERRP) != VALID)
        return ERRP;

    return VALID;
}

int StmtList(FILE* fptr, AST* ast)
{
    int status;
    while(true)
    {
        if((status = Stmt(fptr,ast)) == VALID)
            continue;
        if(status == NAP)
            return VALID;

        return ERRP;
    }
    return VALID;
}

int Stmt(FILE* fptr, AST* ast)
{
    int status;
    if((status = CtrlStmt(fptr, ast)) != NAP)
        return status;
	if((status = DeclStmt(fptr, ast)) != NAP)
		return status;
	else if ((status = ExprStmt(fptr, ast)) != NAP) 
		return status;
	else if ((status = ReturnStmt(fptr, ast)) != NAP)
		return status;

    return NAP;
}

int ExprStmt(FILE* fptr, AST* ast)
{
	Token t = GetNextTokenP(fptr);
    if(t.type == SEMI)
        return VALID;
    PutTokenBack(&t);

    int status;
	if((status = Expr(fptr, ast)) != VALID)
		return status;

    if(CompareToken(fptr, SEMI, "Semicolon missing in ExprStmt", ERRP) != VALID)
        return ERRP;

	return VALID;
}

int DeclStmt(FILE* fptr, AST* ast)
{
	int status;
	if((status = Type(fptr, ast)) != VALID)
		return status;

	if((status = VarList(fptr, ast)) != VALID)
    {
        printf("ERROR: Invalid VarList in DeclStmt\n");
		return ERRP;
    }

    if(CompareToken(fptr, SEMI, "Semicolon missing in DeclStmtt", ERRP) != VALID)
        return ERRP;

    return VALID;
}

int CtrlStmt(FILE* fptr, AST* ast)
{
    int status;
    if ((status = IfStmt(fptr, ast)) != NAP) 
        return status;
    if ((status = SwitchStmt(fptr, ast)) != NAP) 
        return status;
    if ((status = WhileStmt(fptr, ast)) != NAP) 
        return status;
    if ((status = DoWhileStmt(fptr, ast)) != NAP) 
        return status;
    if ((status = ForStmt(fptr, ast)) != NAP) 
        return status;

    return NAP;
}

int ReturnStmt(FILE* fptr, AST* ast)
{
    if(CompareToken(fptr, RET, "", NAP) != VALID)
        return NAP;

    int status;
    if((status = Expr(fptr, ast)) == ERRP)
    {
        printf("Return Stmt has invalid ExprStmt\n");
        return status;
    }

    if(CompareToken(fptr, SEMI, "Semicolon missing in ReturnStmt", ERRP) != VALID)
        return ERRP;

	return VALID;
}


int IfStmt(FILE* fptr, AST* ast)
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

        if((status = Expr(fptr, ast)) != VALID)
        {
            printf("ERROR: Invalid Expr in IfStmt\n");
            return status;
        }

        if(CompareToken(fptr, RPAREN, "Missing right parenthesis for IfStmt", ERRP) != VALID) 
            return ERRP;

        if((status = Body(fptr, ast)) != VALID)
            return status; 

        t = GetNextTokenP(fptr);
        if(t.type == ELIF)
            continue;
        else if(t.type == ELSE)
        {
            if((status = Body(fptr, ast)) != VALID)
                return status;
            break;
        }

        PutTokenBack(&t);
        break;
    }

    return VALID;
}

int SwitchStmt(FILE* fptr, AST* ast)
{
    if(CompareToken(fptr, SWITCH, "", NAP) != VALID)
        return NAP;

    if(CompareToken(fptr, LPAREN, "No LPAREN in SwitchStmt", ERRP) != VALID)
        return ERRP;

	int status;
    if((status = Expr(fptr, ast)) != VALID)
    {

        printf("ERROR: no bad expr in switch stmt\n");
        return ERRP;
    }

    if(CompareToken(fptr, RPAREN, "No LPAREN in SwitchStmt", ERRP) != VALID)
        return ERRP;

    if(CompareToken(fptr, LBRACK, "No LBRACK in SwitchStmt", ERRP) != VALID)
        return ERRP;

    /* TODO: add cases */
    while(true)
    {
        if(CompareToken(fptr, CASE, "", NAP) != VALID)
            break;

        if((status = Expr(fptr, ast)) != VALID)
        {
            printf("ERROR: Invalid case in switch stmt\n");
            return ERRP;
        }

        if(CompareToken(fptr, COLON, "No colon found for case in SwitchStmt", ERRP) != VALID)
            return ERRP;

        if((status = StmtList(fptr, ast)) != VALID)
        {
            printf("ERROR: Invalid StmtList in body of case in switch stmt\n");
            return ERRP;
        }

    }

    if(CompareToken(fptr, DEFAULT, "", NAP) == VALID)
    {
        if(CompareToken(fptr, COLON, "No colon found for default in SwitchStmt", ERRP) != VALID)
            return ERRP;

        if((status = StmtList(fptr, ast)) != VALID)
        {
            printf("ERROR: Invalid StmtList in body of default in switch stmt\n");
            return ERRP;
        }
    }

    if(CompareToken(fptr, RBRACK, "No RBRACK in SwitchStmt", ERRP) != VALID)
        return ERRP;

    return VALID;
}

int WhileStmt(FILE* fptr, AST* ast)
{
    if(CompareToken(fptr, WHILE, "", NAP) != VALID)
        return NAP;

    if(CompareToken(fptr, LPAREN, "No LPAREN in WhileStmt", ERRP) != VALID)
        return ERRP;

    if(Expr(fptr, ast) != VALID)
    {
        printf("ERROR: Invalid Expr in WhileStmt\n");
        return ERRP;
    }

    if(CompareToken(fptr, RPAREN, "No RPAREN in WhileStmt", ERRP) != VALID)
        return ERRP;

    if(Body(fptr, ast) != VALID)
    {
        if(ExprStmt(fptr, ast) != VALID )
        {
            printf("ERROR: Invalid Body in WhileStmt\n");
            return ERRP;
        }
    }

    return VALID;
}

int DoWhileStmt(FILE* fptr, AST* ast)
{
    if(CompareToken(fptr, DO, "", NAP) != VALID)
        return NAP;

    if(Body(fptr, ast) != VALID)
    {
        if(ExprStmt(fptr, ast) != VALID )
        {
            printf("ERROR: Invalid Body after Do in DoWhileStmt\n");
            return ERRP;
        }
    }

    if(CompareToken(fptr, WHILE, "No while after in DoWhileStmt", ERRP) != VALID)
        return ERRP;

    if(CompareToken(fptr, LPAREN, "No LPAREN in DoWhileStmt", ERRP) != VALID)
        return ERRP;

    if(Expr(fptr, ast) != VALID)
    {
        printf("ERROR: Invalid Expr in while of DoWhileStmt\n");
        return ERRP;
    }

    if(CompareToken(fptr, RPAREN, "No RPAREN in DoWhileStmt", ERRP) != VALID)
        return ERRP;

    if(CompareToken(fptr, SEMI, "No SEMI in DoWhileStmt", ERRP) != VALID)
        return ERRP;

    return VALID;
}

int ForStmt(FILE* fptr, AST* ast)
{
    if(CompareToken(fptr, FOR, "", NAP) != VALID)
        return NAP;
    
    if(CompareToken(fptr, LPAREN, "No LPAREN in ForStmt", ERRP) != VALID)
        return ERRP;

    if(ExprList(fptr, ast) == ERRP)
    {
        printf("ERROR: invalid Inital Expr in for stmt\n");
        return ERRP;
    }

    if(CompareToken(fptr, SEMI, "No SEMI in ForStmt", ERRP) != VALID)
        return ERRP;

    if(Expr(fptr, ast) == ERRP)
    {
        printf("ERROR: invalid Comparison Expr in for stmt\n");
        return ERRP;
    }

    if(CompareToken(fptr, SEMI, "No SEMI in ForStmt", ERRP) != VALID)
        return ERRP;

    if(ExprList(fptr, ast) == ERRP)
    {
        printf("ERROR: invalid Increment Expr in for stmt\n");
        return ERRP;
    }

    if(CompareToken(fptr, RPAREN, "No LPAREN in ForStmt", ERRP) != VALID)
        return ERRP;

    if(Body(fptr, ast) != VALID)
    {
        if(ExprStmt(fptr, ast) != VALID )
        {
            printf("ERROR: Invalid Body in ForStmt\n");
            return ERRP;
        }
    }

    return VALID;
}



/* ---------- Expressions ---------- */

int ExprList(FILE* fptr, AST* ast)
{
    int status;
    if((status = Expr(fptr, ast)) != VALID)
        return status;

    Token t;
    while(true)
    {
        if(CompareToken(fptr, COMMA, "", NAP) != VALID)
            break;

        if(Expr(fptr, ast) != VALID)
        {
            printf("ERROR: Expr in ExprList is invalid\n");
            return ERRP;
        }
    }
    
    return VALID;
}

int Expr(FILE* fptr, AST* ast)
{
	int status;
	if((status = AsgnExpr(fptr, ast)) != VALID)
		return status;

	return VALID;
}

int AsgnExpr(FILE* fptr, AST* ast)
{
	int status;
    if((status = OrlExpr(fptr, ast)) != VALID)
        return status;
    
    Token t = GetNextTokenP(fptr);
    if((status = ValidTokType(ASSIGNS, ASSIGNS_COUNT, t.type)) == VALID)
    {
        if((status = AsgnExpr(fptr, ast)) != VALID)
        {
            printf("ERROR: improper AsgnExpr after operator in AsgnExpr\n");
            return status;
        }
    }
    else    
        PutTokenBack(&t);

    return VALID;
}

int OrlExpr(FILE* fptr, AST* ast)
{
    int status;
    if((status = AndlExpr(fptr, ast)) != VALID)
        return status;

    Token t;
    while(true)
    {
        if(CompareToken(fptr, ORL, "", NAP) != VALID)
            break;

        if(AndlExpr(fptr, ast) != VALID)
        {
            printf("ERROR: improper AndlExpr after || in OrlExpr\n");
            return ERRP;
        }
    }

    return VALID;
}

int AndlExpr(FILE* fptr, AST* ast)
{
    int status;
    if((status = OrExpr(fptr, ast)) != VALID)
        return status;

    Token t;
    while(true)
    {
        if(CompareToken(fptr, ANDL, "", NAP) != VALID)
            break;

        if(OrExpr(fptr, ast) != VALID)
        {
            printf("ERROR: improper OrExpr after && in AndlExpr\n");
            return ERRP;
        }
    }

    return VALID;
}

int OrExpr(FILE* fptr, AST* ast)
{
    int status;
    if((status = XorExpr(fptr, ast)) != VALID)
        return status;

    Token t;
    while(true)
    {
        if(CompareToken(fptr, OR, "", NAP) != VALID)
            break;

        if(XorExpr(fptr, ast) != VALID)
        {
            printf("ERROR: improper XorExpr after | in OrExpr\n");
            return ERRP;
        }
    }

    return VALID;
}

int XorExpr(FILE* fptr, AST* ast)
{
    int status;
    if((status = AndExpr(fptr, ast)) != VALID)
        return status;

    Token t;
    while(true)
    {
        if(CompareToken(fptr, XOR, "", NAP) != VALID)
            break;

        if((status = AndExpr(fptr, ast)) != VALID)
        {
            printf("ERROR: improper AndExpr after ^ in OrExpr\n");
            return ERRP;
        }
    }

    return VALID;
}

int AndExpr(FILE* fptr, AST* ast)
{
    int status;
    if((status = EqqExpr(fptr, ast)) != VALID)
        return status;

    Token t;
    while(true)
    {
        if(CompareToken(fptr, AND, "", NAP) != VALID)
            break;

        if(EqqExpr(fptr, ast) != VALID)
        {
            printf("ERROR: improper EqqExpr after & in OrExpr\n");
            return ERRP;
        }
    }

    return VALID;
}

int EqqExpr(FILE* fptr, AST* ast)
{
    int status;
    if((status = RelationExpr(fptr, ast)) != VALID)
        return status;

    Token t = GetNextTokenP(fptr);
    if(t.type == EQQ || t.type == NEQQ)
    {
        if(RelationExpr(fptr, ast) != VALID)
        {
            printf("ERROR: improper RelationExpr after == or != in EqqExpr\n");
            return ERRP;
        }
    }
    else
        PutTokenBack(&t);

    return VALID;
}

int RelationExpr(FILE* fptr, AST* ast)
{
    int status;
    if((status = ShiftExpr(fptr, ast)) != VALID)
        return status;

    Token t = GetNextTokenP(fptr);
    if((status = ValidTokType(RELATIONAL, RELATIONAL_COUNT, t.type)) == VALID)
    {
        if(ShiftExpr(fptr, ast) != VALID)
        {
            printf("ERROR: improper ShiftExpr after operator in RelationExpr\n");
            return ERRP;
        }
    }
    else 
        PutTokenBack(&t);

    return VALID;
}

int ShiftExpr(FILE* fptr, AST* ast)
{
    int status;
    if((status = AddExpr(fptr, ast)) != VALID)
        return status;

    Token t;
    while(true)
    {
        t = GetNextTokenP(fptr);
        if(t.type != LSHIFT && t.type != RSHIFT)
        {
            PutTokenBack(&t);
            break;
        }
        if(AddExpr(fptr, ast) != VALID)
        {
            printf("ERROR: improper AddExpr after << or >> in ShiftExpr\n");
            return ERRP;
        }
    }

    return VALID;
}

int AddExpr(FILE* fptr, AST* ast)
{
	int status;
    if((status = MultExpr(fptr, ast)) != VALID)
        return status;
    
	Token t;
    while(true)
    {
        t = GetNextTokenP(fptr);
        if((status = ValidTokType(ADDS, ADDS_COUNT, t.type)) != VALID)
        {
            PutTokenBack(&t);
            break;
        }
     
        if(MultExpr(fptr, ast) != VALID)
        {
            printf("ERROR: improper MultExpr after + or - in AddExpr\n");
            return ERRP;
        }
    }

    return VALID;
}

int MultExpr(FILE* fptr, AST* ast)
{
    int status;
    if ((status = PowExpr(fptr, ast)) != VALID)
        return status;

    Token t;
    while (true)
    {
        t = GetNextTokenP(fptr);
        if (t.type != MULT && t.type != DIV && t.type != MOD)
        {
            PutTokenBack(&t);
            break;
        }

        if (PowExpr(fptr, ast) != VALID)
        {
            printf("ERROR: improper PowExpr after *, / or %% in MultExpr\n");
            return ERRP;
        }
    }

    return VALID;
}

int PowExpr(FILE* fptr, AST* ast)
{
    int status;
    if ((status = Prefix(fptr, ast)) != VALID)
        return status;

    if(CompareToken(fptr, POW, "", NAP) == VALID)
    {
        if((status = PowExpr(fptr, ast)) != VALID)
        {
            printf("ERROR: improper PowExpr after ** in PowExpr\n");
            return ERRP;
        }
    }

    return VALID;
}

int Prefix(FILE* fptr, AST* ast)
{
    Token t = GetNextTokenP(fptr);
    if(ValidTokType(PREFIXS, PREFIXS_COUNT, t.type) != VALID)
    {
        /* TODO: include cast here */ 
        PutTokenBack(&t);
        return Postfix(fptr, ast);
    }
    
    int status;
    if((status = Prefix(fptr, ast)) != VALID)
        return status;

    return VALID;
}

int Postfix(FILE* fptr, AST* ast)
{
    int status;
    if((status = Primary(fptr, ast)) != VALID)
        return status;

    Token t;
    while(true)
    {
        t = GetNextTokenP(fptr);
        if(ValidTokType(POSTFIXS, POSTFIXS_COUNT, t.type) == VALID)
            continue;

        /* [] */
        if(t.type == LBRACK)
        {
            if(Expr(fptr, ast) != VALID)
            {
                printf("ERROR: Invalid Expr while indexing an array\n");
                return ERRP;
            }

            if(CompareToken(fptr, RBRACK, "Missing RBRACK in while indexing an array", ERRP) != VALID)
                return ERRP;
            continue;
        }
        /* ( function ) */
        else if(t.type == LPAREN)
        {
            if(ArgList(fptr, ast) == ERRP)
            {
               printf("ERROR: Invalid ArgList in function calling\n");
               return ERRP;
            }

            if(CompareToken(fptr, RPAREN, "Missing RPAREN in while calling a function", ERRP) != VALID)
                return ERRP;

            continue;
        }

        PutTokenBack(&t);
        break;
    }

    return VALID;
}

int Primary(FILE* fptr, AST* ast)
{
    int status;
    Token t = GetNextTokenP(fptr);
    if(t.type == IDENT || t.type == SLITERAL || t.type == CLITERAL || t.type == DECIMAL || t.type == INTEGRAL)
        return VALID;
    else if (t.type == LPAREN)
    { 
        if(Expr(fptr, ast) != VALID)
        {
            printf("ERROR: improper Expr after ( in Primary\n");
            return ERRP;
        }
    
        if(CompareToken(fptr, RPAREN, "Missing RPAREN in Primary", ERRP) != VALID)
            return ERRP;
        return VALID;
    }

    PutTokenBack(&t);
    return NAP;
}

 

/* ---------- Etc ---------- */

int Type(FILE* fptr, AST* ast)
{
    Token t = GetNextTokenP(fptr);
    if(ValidTokType(TYPES, TYPES_COUNT, t.type) != VALID)
    {
        PutTokenBack(&t);
        return NAP;
    }
     
    return VALID;
}

int ArgList(FILE* fptr, AST* ast)
{
    int status;
    if((status = Primary(fptr, ast)) != VALID)
        return status;

    while(true)
    {
        if(CompareToken(fptr, COMMA, "", NAP) != VALID)
            break;

        if(Primary(fptr, ast) != VALID)
        {
            printf("ERROR: Invalid ArgList\n");
            return ERRP;
        }
    }
    
    return VALID;
}

int VarList(FILE* fptr, AST* ast)
{
    int status;
    if((status = Var(fptr, ast)) != VALID)
        return status;

    while(true)
    {
        if(CompareToken(fptr, COMMA, "", NAP) != VALID)
            break;

        if(Var(fptr, ast) != VALID)
        {
            printf("ERROR: improper Var after , in VarList\n");
            return ERRP;
        }
    }

    return VALID;
}

int Var(FILE* fptr, AST* ast)
{
    if(CompareToken(fptr, IDENT, "", NAP) != VALID)
        return NAP;

    if(CompareToken(fptr, EQ, "", NAP) != VALID)
        return VALID;

    if(Expr(fptr, ast) != VALID)
    {
        printf("ERROR: Invalid Expr equivalent to var\n");
        return ERRP;
    }

    return VALID;
}
