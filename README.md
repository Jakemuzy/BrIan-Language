# Introducing BrIan

A Compiled Language built with **concurrency** in mind. Build mainly for embedded systems, compiles to the **RISC-V** ISA.

---

## BUILDING
    1.) Run the compilation script (unix only) ./compile.sh name.exe
    1.5.) Include path to your source code
    2.) Run the executable you created ./builds/name.exe

## GOALS:
    Concurrency, low memory utilization, compiled, safe memory

---

EBNF:  
    ImportList ::= { "#import" "<" IDENT ">" }

	Prog ::= ImportList START Body UPDATE Body
	Thread ::= IDENT Body

	Body ::= '{' StmtLst '}' | Stmt
	StmtList ::= { Stmt }
	Stmt ::= DeclStmt | CtrlStmt | CompStmt | Expr
	
	DeclStmt ::= Type VarList;
	CtrlStmt ::= IfStmt | SwitchStmt | WhileStmt | DoWhileStmt | ForStmt 
	CompStmt ::= '(' Expr ')'

    IfStmt ::= "if" CompStmt Body { "elif" Body } { "else" Body }
    SwitchStmt ::= "switch" '(' Var ')' '{' {"case" Var ':' StmtList '}'
    WhileStmt ::= "while" CompStmt Body
    DoWhileStmt ::= "do" Body "while" CompStmt ';'
    ForStmt ::= "for" '(' Expr ';' Expr ';' Expr ')'

	Expr ::= LasgnExpr
    LasgnExpr ::= BasgnExpr { ( '||=' | &&= ) BasgnExpr }
    BasgnExpr ::= SasgnExpr { ( &= | '|=' | ^= ) SasgnExpr }
    SasgnExpr ::= AsgnExpr { ( <<= | >>= ) AsgnExpr }
    AsgnExpr ::= TernExpr { ( += | -= | *= | /= | %= ) TernExpr }
    TernExpr ::= LandExpr ? LandExpr : LandExpr | LandExpr
    LandExpr ::= LorExpr { && LorExpr }
    LorExpr ::= BorExpr { '||' BorExpr }
    BorExpr ::= XorExpr { '|' XorExpr }
    XorExpr ::= BandExpr { '^' BandExpr }
    BandExpr ::= EqqExpr { '&' EqExpr }
    EqqExpr ::= CompExpr { ( == | != ) CompExpr }
    CompExpr ::= ShiftExpr { ( < | <= | > | >= ) ShiftExpr }
    ShiftExpr ::= AddExpr { ( "<<" | ">>" ) AddExpr }
	AddExpr ::= MultEpxr { ( + | - ) MultExpr }
	MultExpr ::= UnaryExpr { ( * | / | % ) UnaryExpr }
    UnaryExpr ::= ( ++ | -- | ! | ~ | '(' Type ')' | * | & ) ImmExpr | ImmExpr ( ++ | -- | ** | ! ) | ImmExpr
    ImmExpr ::= '(' Primary ')' | '[' Primary ']' | Primary ( . | -> ) Primary | Primary
    Primary ::= IDENT | LITERAL | '(' Expr ')'

    Type = ( char | bool | int | long | double | float | void | string )

	VarList ::= Var ; | Var , Var;
    Var = IDENT | IDENT Expr | VarList
	...	
    
---
## Presedence

1.)  () [] . ->
2.)  ++ -- ** ! ~ (type) * &        **Unary**
3.)  * / %                          **Arith**
4.)  + -  
5.)  << >>                          **Shift**
6.)  < <= > >=                      **Logic**
7.)  == !=
8.)  &                              **Bit**
9.)  ^
10.) |
11.) &&                     
12.) || 
13.) ?:                             **Tern**
14.) += -= *= /= %= 
15.) <<= >>=
16.) &= ^= |= 
17.) &&= ||= 

### NOTES
    BrIan does NOT short circuit
    BrIan is type safe
    BrIan allows for easy bit manipulation
    BrIan has 

---

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

