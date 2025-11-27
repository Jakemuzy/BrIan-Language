#include "Parser.h"

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
        return NAP;
    }

    return VALID;
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
    
    /* TODO: while(retCode != ERRP && retCode != NAP) */
    while (true)
        retCode = Stmt(fptr, ast);
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
	if((status = Type(fptr, ast) != VALID))
		return status;

	if((status = VarList(fptr, ast) != VALID))
		return status;

	return VALID;
}

int CtrlStmt(FILE* fptr, AST* ast)
{
	int status;
	if(!(status = IfStmt(fptr, ast)))
		return status;
	else if ((status = SwitchStmt(fptr, ast) != VALID))
		return status;
	else if((status = WhileStmt(fptr, ast) != VALID))
		return status;
	else if((status = DoWhileStmt(fptr, ast) != VALID))
		return status;
	else if((status = ForStmt(fptr, ast) != VALID))
		return status;

	return VALID;
}

int ReturnStmt(FILE* fptr, AST* ast)
{
	Token t = GetNextToken(fptr);

	if(!strcmp(t.lex.word, "return"))
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

	if((status = Expr(fptr, ast) != VALID))
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

	return VALID;
}


int Type(FILE* fptr, AST* ast)
{
}
int VarList(FILE* fptr, AST* ast)
{
}
