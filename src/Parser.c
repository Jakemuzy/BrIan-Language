#include "Parser.h"

/* TODO:
 * 
*/

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

AST* ConstructAST(FILE* fptr)
{
    AST* ast = malloc(sizeof(AST));
    ast->root = NULL;

    return ast;
}

/* ---------- EBNF ---------- */

void Program(FILE* fptr, AST* ast)
{
    int status;
    /* TODO: Skipped for testing, in preprocess (gettokenp)
    if((status = ImportList(fptr, ast)) != ERRP)
    {
        printf("ERROR: Program has malformed imports\n");
        exit(1);
    }
    */

    Token t;
    t = GetNextTokenP(fptr);
    if(strcmp(t.lex.word, "START"))
    {
        /* TODO: output line num and col */
        printf("ERROR: Program must have START function\n");
        exit(1);
    }

    if((status = Body(fptr, ast)) != VALID)
    {
        printf("ERROR: Program has malformed body after START, %d\n", status); 
        exit(1);
    }

}


int Body(FILE* fptr, AST* ast)
{
    int status;
    Token t = GetNextTokenP(fptr);

    if(t.type != LBRACK)
    {
        PutTokenBack(&t);
        return NAP;
    }

    if((status = StmtList(fptr, ast)) == ERRP)
        return status;

    t = GetNextTokenP(fptr);
    if(t.type != RBRACK)
    {
        PutTokenBack(&t);
        printf("Missing RBRACK in BODY\n");
        return ERRP;
    }

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
	Token t;
    int status;
	if((status = Expr(fptr, ast)) != VALID)
		return status;
	
	t = GetNextTokenP(fptr);
	if(t.type != SEMI)
	{
		printf("ERROR: Semicolon missing in exprstmt\n");
		return ERRP;
	}	

	return VALID;
}

int DeclStmt(FILE* fptr, AST* ast)
{
	int status;
	if((status = Type(fptr, ast)) != VALID)
		return status;

	if((status = VarList(fptr, ast)) != VALID)
		return status;

    Token t;
    t = GetNextTokenP(fptr);
    if(t.type != SEMI)
    {
        printf("ERROR: Semicolon missing in declstmt\n");
        return ERRP;
    }
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
	Token t = GetNextTokenP(fptr);
	if(strcmp(t.lex.word, "return") != 0)
	{
		PutTokenBack(&t);
		return NAP;
	}

    int status;
    if((status = ExprStmt(fptr, ast)) == ERRP)
    {
        printf("Return Stmt has invalid ExprStmt\n");
        return status;
    }

    t = GetNextTokenP(fptr);
    if(t.type != SEMI)
    {
        printf("ERROR: Semicolon missing in returnstmt\n");
        return ERRP;
    }

	return VALID;
}



int IfStmt(FILE* fptr, AST* ast)
{
	int status;
	Token t = GetNextTokenP(fptr);

	if(t.type != IF)
	{
		PutTokenBack(&t);
		return NAP;
	}

	/* Infinite elif and final else */
    bool chain = true;
    while(chain)
    {
        t = GetNextTokenP(fptr);
        if(t.type != LPAREN)
        {
            printf("ERROR: Missing left parenthesis for IF statement\n");
            return ERRP;
        }

        if((status = Expr(fptr, ast)) != VALID)
            return status;

        t = GetNextTokenP(fptr);
        if(t.type != RPAREN)
        {
            printf("ERROR: Missing right parenthesis for IF statement\n");
            return ERRP;
        }

        if((status = Body(fptr, ast)) != VALID)
            return status; 

        t = GetNextTokenP(fptr);
        if(t.type == ELIF)
        {
            continue;
        }
        else if(t.type != ELSE)
        {
            chain = false;
            continue;
        }
        
        PutTokenBack(&t);
        chain = false;
    }

    return VALID;
}

int SwitchStmt(FILE* fptr, AST* ast)
{
	int status;
	Token t = GetNextTokenP(fptr);

	if(t.type != SWITCH)
	{
		PutTokenBack(&t);
		return NAP;
	}

    t = GetNextTokenP(fptr);
    if(t.type != LPAREN)
    {
        printf("ERROR: no lparen in switch stmt\n");
        return ERRP; 
    }

    if((status = Expr(fptr, ast)) != VALID)
    {

        printf("ERROR: no bad expr in switch stmt\n");
        return ERRP;
    }

    t = GetNextTokenP(fptr);
    if(t.type != RPAREN)
    {
        printf("ERROR: no rparen in switch stmt\n");
        return ERRP; 
    }

    t = GetNextTokenP(fptr);
    if(t.type != LBRACE)
    {
        printf("ERROR: no lbrace in switch stmt\n");
        return ERRP; 
    }

    /* TODO: add cases */
    int cases = 1;
    while(cases)
    {
        t = GetNextTokenP(fptr);
        if(t.type != CASE)
        {
            PutTokenBack(&t);
            printf("ERROR: no case in switch stmt\n");
        }

        t = GetNextTokenP(fptr);
        if(t.type != COLON)
        {
            PutTokenBack(&t);
            printf("ERROR: no colon for case in switch stmt\n");
        }

        if((status = StmtList(fptr, ast)) == ERRP)
        {
            printf("ERROR: Invalid StmtList in body of case in switch stmt\n");
            return status;
        }

    }

    t = GetNextTokenP(fptr);
    if(t.type != DEFAULT)
    {
        PutTokenBack(&t);
        printf("ERROR: no case in switch stmt\n");
    }

    t = GetNextTokenP(fptr);
    if(t.type != COLON)
    {
        PutTokenBack(&t);
        printf("ERROR: no colon for case in switch stmt\n");
    }

    if((status = StmtList(fptr, ast)) == ERRP)
    {
        printf("ERROR: Invalid StmtList in body of default in switch stmt\n");
        return status;
    }

    t = GetNextTokenP(fptr);
    if(t.type != RBRACE)
    {
        printf("ERROR: no lbrace in switch stmt\n");
        return ERRP; 
    }

    return VALID;
}
int WhileStmt(FILE* fptr, AST* ast)
{
    int status;
    Token t = GetNextTokenP(fptr);
    if(t.type != WHILE)
    {
        PutTokenBack(&t);
        return NAP;
    }

    t = GetNextTokenP(fptr);
    if(t.type != LPAREN)
    {
        printf("ERROR: no LPAREN in while stmt\n");
        return ERRP;
    }

    if((status = Expr(fptr, ast)) != VALID)
        return status;

    t = GetNextTokenP(fptr);
    if(t.type != RPAREN)
    {
        printf("ERROR: no RPAREN in while stmt\n");
        return ERRP;
    }

    if((status = Body(fptr, ast)) != VALID)
        return status;

    return VALID;
}
int DoWhileStmt(FILE* fptr, AST* ast)
{
    int status;
    Token t = GetNextTokenP(fptr);
    if(t.type != DO)
    {
        PutTokenBack(&t);
        return NAP;
    }

    if((status = Body(fptr, ast)) != VALID)
        return status;

    if(t.type != WHILE)
    {
        PutTokenBack(&t);
        return NAP;
    }

    t = GetNextTokenP(fptr);
    if(t.type != LPAREN)
    {
        printf("ERROR: no LPAREN in do while stmt\n");
        return ERRP;
    }

    if((status = Expr(fptr, ast)) != VALID)
        return status;

    t = GetNextTokenP(fptr);
    if(t.type != RPAREN)
    {
        printf("ERROR: no RPAREN in do while stmt\n");
        return ERRP;
    }

	t = GetNextTokenP(fptr);
	if(t.type != SEMI)
	{
		printf("ERROR: Semicolon missing in do while\n");
		return ERRP;
	}	

    return VALID;
}
int ForStmt(FILE* fptr, AST* ast)
{
    Token t = GetNextTokenP(fptr);
    if(t.type != FOR)
    {
        PutTokenBack(&t);
        return NAP;
    }
    
    t = GetNextTokenP(fptr);
    if(t.type != LPAREN)
    {
        printf("ERROR: no LPAREN in for stmt\n");
        return ERRP;
    }

    int status;
    if((status = ExprList(fptr, ast)) == ERRP)
    {
        printf("ERROR: invalid Inital Expr in for stmt\n");
        return ERRP;
    }

    t = GetNextTokenP(fptr);
    if(t.type != SEMI)
    {
        printf("ERROR: no SEMICOLON in do for stmt\n");
        return ERRP;
    }

    if((status = Expr(fptr, ast)) == ERRP)
    {
        printf("ERROR: invalid Comparison Expr in for stmt\n");
        return ERRP;
    }

    t = GetNextTokenP(fptr);
    if(t.type != SEMI)
    {
        printf("ERROR: no SEMICOLON in do for stmt\n");
        return ERRP;
    }

    if((status = ExprList(fptr, ast)) == ERRP)
    {
        printf("ERROR: invalid Increment Expr in for stmt\n");
        return ERRP;
    }

    t = GetNextTokenP(fptr);
    if(t.type != RPAREN)
    {
        printf("ERROR: no RPAREN in for stmt\n");
        return ERRP;
    }


    return VALID;
}





int ExprList(FILE* fptr, AST* ast)
{
    int status;
    if((status = Expr(fptr, ast)) != VALID)
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

        if((status = Expr(fptr, ast)) != ERRP)
            return VALID;
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
    if((status = ValidTokType(ASSIGNS, ASSIGNS_COUNT, t.type)) != VALID)
    {
        if((status = AsgnExpr(fptr, ast)) != VALID)
            return status;
    }
    else    
        PutTokenBack(&t);

    return VALID;
}

int OrlExpr(FILE* fptr, AST* ast)
{
    int status;
    Token t;
    if((status = AndlExpr(fptr, ast)) != VALID)
        return status;

    while(true)
    {
        t = GetNextTokenP(fptr);
        if(t.type != ORL)
        {
            PutTokenBack(&t);
            break;
        }
        if((status = AndlExpr(fptr, ast)) != VALID)
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
    Token t;
    if((status = OrExpr(fptr, ast)) != VALID)
        return status;

    while(true)
    {
        t = GetNextTokenP(fptr);
        if(t.type != ANDL)
        {
            PutTokenBack(&t);
            break;
        }
        if((status = OrExpr(fptr, ast)) != VALID)
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
    Token t;
    if((status = XorExpr(fptr, ast)) != VALID)
        return status;

    while(true)
    {
        {
            PutTokenBack(&t);
            break;
        }
        if((status = XorExpr(fptr, ast)) != VALID)
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
    Token t;
    if((status = AndExpr(fptr, ast)) != VALID)
        return status;

    while(true)
    {
        t = GetNextTokenP(fptr);
        if(t.type != XOR)
        {
            PutTokenBack(&t);
            break;
        }
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
    Token t;
    if((status = EqqExpr(fptr, ast)) != VALID)
        return status;

    while(true)
    {
        t = GetNextTokenP(fptr);
        if(t.type != AND)
        {
            PutTokenBack(&t);
            break;
        }
        if((status = EqqExpr(fptr, ast)) != VALID)
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
    Token t;
    if((status = RelationExpr(fptr, ast)) != VALID)
        return status;

    t = GetNextTokenP(fptr);
    if(t.type != EQQ && t.type != NEQQ)
    {
        PutTokenBack(&t);
        return NAP;
    }
    if((status = RelationExpr(fptr, ast)) != VALID)
    {
        printf("ERROR: improper RelationExpr after == or != in EqqExpr\n");
        return ERRP;
    }

    return VALID;
}

int RelationExpr(FILE* fptr, AST* ast)
{
    int status;
    Token t;
    if((status = ShiftExpr(fptr, ast)) != VALID)
        return status;

    if(true)
    {
        t = GetNextTokenP(fptr);
        if((status = ValidTokType(RELATIONAL, RELATIONAL_COUNT, t.type)) != VALID)
        {
            PutTokenBack(&t);
            return status;
        }
        if((status = ShiftExpr(fptr, ast)) != VALID)
        {
            printf("ERROR: improper ShiftExpr after operator in RelationExpr\n");
            return ERRP;
        }
    }

    return VALID;
}

int ShiftExpr(FILE* fptr, AST* ast)
{
    int status;
    Token t;
    if((status = AddExpr(fptr, ast)) != VALID)
        return status;

    while(true)
    {
        t = GetNextTokenP(fptr);
        if(t.type != LSHIFT && t.type != RSHIFT)
        {
            PutTokenBack(&t);
            break;
        }
        if((status = AddExpr(fptr, ast)) != VALID)
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
	Token t;

    if((status = MultExpr(fptr, ast)) != VALID)
        return status;
    
    while(true)
    {

        t = GetNextTokenP(fptr);
        if((status = ValidTokType(ADDS, ADDS_COUNT, t.type)) != VALID)
        {
            PutTokenBack(&t);
            break;
        }
     
        if((status = MultExpr(fptr, ast)) != VALID)
            return status;
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

        if ((status = PowExpr(fptr, ast)) != VALID)
            return status;
    }

    return VALID;
}

int PowExpr(FILE* fptr, AST* ast)
{
    int status;
    if ((status = Prefix(fptr, ast)) != VALID)
        return status;

    Token t;
    t = GetNextTokenP(fptr);
    if(t.type == POW)
    {
        if((status = PowExpr(fptr, ast)) != VALID)
            return status;
    }
    else 
        PutTokenBack(&t);

    return VALID;
}

int Prefix(FILE* fptr, AST* ast)
{
    int status;
    Token t = GetNextTokenP(fptr);
    if(ValidTokType(PREFIXS, PREFIXS_COUNT, t.type) != VALID)
    {
        /* TODO: include cast here */ 
        PutTokenBack(&t);
        return Postfix(fptr, ast);
    }
    
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
        if(ValidTokType(POSTFIXS, POSTFIXS_COUNT, t.type) != VALID)
        {
            PutTokenBack(&t);
            break;
        }
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
        if((status = Expr(fptr, ast)) != VALID)
            return status;
    
        t = GetNextTokenP(fptr);
        if(t.type != RPAREN)
        {
            printf("Missing RPAREN in primary\n");
            return ERRP;
        }
    }

    return VALID;
}


int Type(FILE* fptr, AST* ast)
{
    Token t = GetNextTokenP(fptr);
    if(ValidTokType(TYPES, TYPES_COUNT, t.type) != VALID)
    {
        printf("ERROR: Not a valid type: %s \n", t.lex.word);
        return ERRP;
    }

     
    return VALID;
}
int VarList(FILE* fptr, AST* ast)
{
}
