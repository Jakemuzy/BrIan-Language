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

AST* ConstructAST(FILE* fptr)
{
    AST tree;
    tree.root = malloc(sizeof(ASTNode));


    return;
}

/* ---------- EBNF ---------- */

void Program(FILE* fptr, AST* ast)
{
    Token t;

    ImportList(fptr, ast);

    t = GetNextToken(fptr);
    if(!strcmp(t.lex.word, "START"))
    {
        /* TODO: output line num and col */
        perror("ERROR: Program must have START function\n");
        exit(1);
    }

    Body(fptr, ast);

    /* TODO: For now we will force an update function, fix this later */
    t = GetNextToken(fptr);

    if(!strcmp(t.lex.word, "START"))
    {
        /* TODO: output line num and col */
        perror("ERROR: Program must have UPDATE function\n");
        exit(1);
    }
}

int ImportList(FILE* fptr, AST* ast)
{
    Token t = GetNextToken(fptr);
    if(!strcmp(t.lex.word, "#import"))
    {
        PutTokenBack(&t);
        return VALID;
    }

    return NAP;
}

int Body(FILE* fptr, AST* ast)
{
    Token t = GetNextToken(fptr);
    if(t.type != LBRACK)
    {
        PutTokenBack(&t);
        return NAP;
    }

    StmtList(fptr, ast);

    t = GetNextToken(fptr);
    if(t.type != RBRACK)
    {
        PutTokenBack(&t);
        return ERRP;
    }

    return VALID;
}
int StmtList(FILE* fptr, AST* ast)
{
    int retCode = VALID;
    while((retCode = Stmt(fptr, ast)) == VALID)
        ;
    return retCode;
}
int Stmt(FILE* fptr, AST* ast)
{
    Token t;
    if(!CtrlStmt(fptr, ast) && !LineStmt(fptr, ast))
        return NAP;

    t = GetNextToken(fptr);
    if(t.type != SEMI)
    {
        perror("ERROR: Semicolon missing\n");
        PutTokenBack(&t);
        return ERRP;
    }
    return VALID;
}

int LineStmt(FILE* fptr, AST* ast)
{

	int status;

	if(status = DeclStmt(fptr, ast))
		return status;
	else if (status = ExprStmt(fptr, ast))
		return status;
	else if (status = ReturnStmt(fptr, ast))
		return status;

	return NAP;
}







int ExprStmt(FILE* fptr, AST* ast)
{
	Token t;
	if(!Expr(fptr, ast))
		return NAP;
	
	t = GetNextToken(fptr);
	if(t.type != SEMI)
	{
		perror("ERROR: Semicolon missing\n");
		PutTokenBack(&t);
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

	return VALID;
}

int CtrlStmt(FILE* fptr, AST* ast)
{
    int status;

    status = IfStmt(fptr, ast);
    if (status != NAP) 
        return status;

    status = SwitchStmt(fptr, ast);
    if (status != NAP) 
        return status;

    status = WhileStmt(fptr, ast);
    if (status != NAP) 
        return status;

    status = DoWhileStmt(fptr, ast);
    if (status != NAP) 
        return status;

    status = ForStmt(fptr, ast);
    if (status != NAP) 
        return status;

    return NAP;
}

int ReturnStmt(FILE* fptr, AST* ast)
{
	Token t = GetNextToken(fptr);

	if(strcmp(t.lex.word, "return") != 0)
	{
		PutTokenBack(&t);
		return NAP;
	}

	/* TODO: Put an optional ExprStmt check here */

	t = GetNextToken(fptr);
	if(t.type != SEMI)
	{
		PutTokenBack(&t);
		return ERRP;
	}
	return VALID;
}



int IfStmt(FILE* fptr, AST* ast)
{
	int status;
	Token t = GetNextToken(fptr);

	if(t.type != IF)
	{
		PutTokenBack(&t);
		return NAP;
	}

	t = GetNextToken(fptr);
	if(t.type != LPAREN)
	{
		perror("ERROR: Missing left parenthesis for IF statement\n");
		return ERRP;
	}

	if((status = Expr(fptr, ast)) != VALID)
		return status;

	t = GetNextToken(fptr);
	if(t.type != RPAREN)
	{
		perror("ERROR: Missing right parenthesis for IF statement\n");
		return ERRP;
	}

	if((status = Body(fptr, ast)) != VALID)
		return status;

	/* TODO option elif and else */
    /* TODO allow chaining */
    return VALID;
}

int SwitchStmt(FILE* fptr, AST* ast)
{
}
int WhileStmt(FILE* fptr, AST* ast)
{
}
int DoWhileStmt(FILE* fptr, AST* ast)
{
}
int ForStmt(FILE* fptr, AST* ast)
{
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

        t = GetNextToken(fptr);
        if((status = ValidTokType(ASSIGNS, ASSIGNS_COUNT, t.type)) != VALID)
        {
            PutTokenBack(&t);
            break;
        }
     
        if((status = LogicExpr(fptr, ast)) != VALID)
            return status;

        return VALID;
    }
}

int LogicExpr(FILE* fptr, AST* ast)
{
	int status;
	Token t;

    if((status = LogicExpr(fptr, ast)) != VALID)
        return status;
    
    while(true)
    {

        t = GetNextToken(fptr);
        if((status = ValidTokType(ASSIGNS, ASSIGNS_COUNT, t.type)) != VALID)
        {
            PutTokenBack(&t);
            break;
        }
     
        if((status = LogicExpr(fptr, ast)) != VALID)
            return status;

        return VALID;
    }
}

int BitExpr(FILE* fptr, AST* ast)
{
	int status;
	Token t;

    if((status = AddExpr(fptr, ast)) != VALID)
        return status;
    
    while(true)
    {

        t = GetNextToken(fptr);
        if((status = ValidTokType(ADDS, ADDS_COUNT, t.type)) != VALID)
        {
            PutTokenBack(&t);
            break;
        }
     
        if((status = AddExpr(fptr, ast)) != VALID)
            return status;

        return VALID;
    }
}

int AddExpr(FILE* fptr, AST* ast)
{
	int status;
	Token t;

    if((status = MultExpr(fptr, ast)) != VALID)
        return status;
    
    while(true)
    {

        t = GetNextToken(fptr);
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
        t = GetNextToken(fptr);
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
    t = GetNextToken(fptr);
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
    Token t;
    t = GetNextToken(fptr);
    if(ValidTokType(PREFIXS, PREFIXS_COUNT, t.type != VALID))
    {
        /* TODO: include cast here */ 
        PutTokenBack(&t);
        return NAP;
    }
    
    int status;
    if((status = Prefix(fptr, ast)) == VALID)
        return VALID;
    else if ((status = Postfix(fptr, ast)) != VALID)
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
        if(ValidTokType(POSTFIXS, POSTFIXS_COUNT, t.type != VALID))
        {
            /* TODO: include cast here */ 
            PutTokenBack(&t);
            break;
        }
    }
}

int Type(FILE* fptr, AST* ast)
{
}
int VarList(FILE* fptr, AST* ast)
{
}
