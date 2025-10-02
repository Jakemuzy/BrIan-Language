Introducing BrIan

A Compiled Language built with concurrency in mind.

GOALS:
    Concurrency, low memory utilization, compiled, safe memory

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
REGULAR EXPRESSIONS:
    


DATA TYPES:
    Integral:
        char(1), bool(1), short(2), int(4), long(8)
    Decimal:
        float(4), double(8)
    Special:
        binary(1), hexadecimal(8), string(inf), safeptr(inf), struct(inf), enum(inf), thread(inf)

TYPE QUALIFIERS:
    unsigned, signed, const, new

CONTROL STATEMENTS:
    if, else, do, while, switch, case, break, continue, goto, return

