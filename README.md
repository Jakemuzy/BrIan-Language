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
    ImportList ::= #import Import | e
    Import ::= '<'IDENT'>' | ImportList

	Prog ::= ImportList START Body UPDATE Body
	Thread ::= IDENT Body

	Body ::= '{' StmtLst '}'
	StmtList ::= Stmtist | Stmt
	Stmt ::= DeclStmt | CtrlStmt | CompStmt | Expr
	
	DeclStmt ::= Type VarList;
	CtrlStmt ::= ( for | while | if ) CompStmt Body | switch Var CompStmt case ':' CompStmt | do Body while 
	CompStmt ::= '(' Expr ')'
	 
	VarList ::= , Var ; | Var ;
    Var = IDENT | IDENT Expr | VarList

	Expr ::= LasgnExpr
    LasgnExpr ::= ( '||=' | &&= ) BasgnExpr | BasgnExpr
    BasgnExpr ::= ( &= | '|=' | ^= ) SasgnExpr | SasgnExpr
    SasgnExpr ::= ( <<= | >>= ) AsgnExpr | AsgnExpr
    AsgnExpr ::= ( += | -= | *= | /= | %= ) TernExpr | TernExpr
    TernExpr ::= ? LorExpr : LorExpr | LorExpr
    LorExpr ::= '||' LandExpr | LandExpr
    LandExpr ::= && BorExpr | BorExpr
    BorExpr ::= '|' XorExpr | XorExpr
    XorExpr ::= ^ BandExpr | BandExpr
    BandExpr ::= & EqqExpr | EqqExpr
    EqqExpr ::= ( == | != ) CompExpr | CompExpr
    CompExpr ::= ( < | <= | > | >= ) ShiftExpr | ShiftExpr
    ShiftExpr ::= ( << | >> ) AddExpr | AddExpr
	AddExpr ::= ( + | - ) MultExpr | MultExpr
	MultExpr ::= ( * | / | % ) UnaryExpr | UnaryExpr
    UnaryExpr ::= ( ++ | -- | ** | ! | ~ | (type) | * | & ) ImmExpr | ImmExpr
    ImmExpr ::= '(' BaseExpr ')' | '[' BaseExpr ']' | ( . | -> ) BaseExpr | BaseExpr
    BaseExpr ::= Var | Expr

    Type = ( char | bool | int | long | double | float | void | string )
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

