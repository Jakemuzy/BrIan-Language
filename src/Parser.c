#include "Parser.h"

/* TODO:
 * Switch out peror with my own error alter system
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
        perror("ERROR: Program has malformed imports\n");
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
    int status = VALID;
    while(true)
    {
        if((status = Stmt(fptr, ast)) != VALID)
            break;
    }

    return status;
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
		perror("ERROR: Semicolon missing in exprstmt\n");
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
        perror("ERROR: Semicolon missing in declstmt\n");
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
        perror("ERROR: Semicolon missing in returnstmt\n");
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
            perror("ERROR: Missing left parenthesis for IF statement\n");
            return ERRP;
        }

        if((status = Expr(fptr, ast)) != VALID)
            return status;

        t = GetNextTokenP(fptr);
        if(t.type != RPAREN)
        {
            perror("ERROR: Missing right parenthesis for IF statement\n");
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

    t = GetNextTokenP(fptr);
    if(t.type != RBRACE)
    {
        printf("ERROR: no lbrace in switch stmt\n");
        return ERRP; 
    }

    return NAP;
}
int WhileStmt(FILE* fptr, AST* ast)
{

    return NAP;
}
int DoWhileStmt(FILE* fptr, AST* ast)
{

    return NAP;
}
int ForStmt(FILE* fptr, AST* ast)
{

    return NAP;
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
	Token t;

    if((status = LogicExpr(fptr, ast)) != VALID)
        return status;
    
    while(true)
    {

        t = GetNextTokenP(fptr);
        if((status = ValidTokType(ASSIGNS, ASSIGNS_COUNT, t.type)) != VALID)
        {
            PutTokenBack(&t);
            break;
        }
     
        if((status = LogicExpr(fptr, ast)) != VALID)
            return status;

    }

    return VALID;
}

int LogicExpr(FILE* fptr, AST* ast)
{
	int status;
	Token t;

    if((status = BitExpr(fptr, ast)) != VALID)
        return status;
    
    while(true)
    {

        t = GetNextTokenP(fptr);
        if((status = ValidTokType(ASSIGNS, ASSIGNS_COUNT, t.type)) != VALID)
        {
            PutTokenBack(&t);
            break;
        }
     
        if((status = BitExpr(fptr, ast)) != VALID)
            return status;

    }

    return VALID;
}

int BitExpr(FILE* fptr, AST* ast)
{
	int status;
	Token t;

    if((status = AddExpr(fptr, ast)) != VALID)
        return status;
    
    while(true)
    {

        t = GetNextTokenP(fptr);
        if((status = ValidTokType(ADDS, ADDS_COUNT, t.type)) != VALID)
        {
            PutTokenBack(&t);
            break;
        }
     
        if((status = AddExpr(fptr, ast)) != VALID)
            return status;
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
        if(ValidTokType(POSTFIXS, POSTFIXS_COUNT, t.type != VALID))
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

    return NAP;
}


int Type(FILE* fptr, AST* ast)
{
    Token t = GetNextTokenP(fptr);
    if(ValidTokType(TYPES, TYPES_COUNT, t.type != VALID))
    {
        perror("ERROR: Not a valid type");
        return ERRP;
    }

     
    return VALID;
}
int VarList(FILE* fptr, AST* ast)
{
}
