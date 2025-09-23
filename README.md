Introducing BrIan

A Compiled Language with build with concurrency in mind

EBNF:  
	Prog ::= START CompStmt UPDATE Compstmt
	Thread ::= IDENT CompStmt

	CompStmt ::= '{' StmtLst '}'
	StmtList ::= Stmt*
	Stmt ::= DeclStmt | CtrlStmt | CompStmt
	
	DeclStmt ::= (char | bool | int | long | double | float | void | string ) VarList
	CtrlStmt ::= (for | while | do | switch ) ... 
	CompStmt ::= (IDENT | Expr) ( == | != | <= | >= | < | > ) (IDENT | Expr)
	 
	VarList ::= IDENT AsgnExpr ',' IDENT AssgnExpr ';'
	
	Expr
	UnaryExpr
	MultExpr
	AddExpr
	AssgnExpr
	...	
	
