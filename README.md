Introducing BrIan

A Compiled Language with build with concurrency in mind

EBNF:  
    ImportList ::= ImportList | Import
    Import ::= '<'IDENT'>'

	Prog ::= START CompStmt UPDATE 
	Thread ::= IDENT CompStmt

	Body ::= '{' StmtLst '}'
	StmtList ::= Stmtist | Stmt
	Stmt ::= DeclStmt | CtrlStmt | CompStmt
	
	DeclStmt ::= ( char | bool | int | long | double | float | void | string ) VarList
	CtrlStmt ::= ( for | while | if ) CompStmt Body | switch Var CompStmt case ':' CompStmt | do Body while 
	CompStmt ::= ( IDENT | Expr ) ( == | != | <= | >= | < | > ) (IDENT | Expr)
	 
	VarList ::= Var ',' Varlist | Var 
	Var = IDENT 

	Expr ::= UnaryExpr | MultExpr | AddExpr | AssgnExpr | BaseExpr
	UnaryExpr ::= ( ! | + | - ) ExpExpr | ExpExpr ! 
    ExpExpr ::= MultExpr ** 
	MultExpr ::= ( * | / | % ) AddExpr 
	AddExpr ::= ( + | - )
	AssgnExpr ::= Var ( = | += | -= | /= | *= | %= ) | Expr
    BaseExpr ::= Var | Expr


	...	
	
