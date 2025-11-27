# Introducing BrIan

A Compiled Language built with **concurrency** in mind. Built mainly for embedded systems, compiles to the **RISC-V** ISA

---

## PRINCIPLES:
    Brian's main goals are 
        - Concurrency 
        - Low memory utilization 
        - Compiled 
        - Type and memory safety
    BrIan strives to be simple but extremely powerful. By allowing programmers to interface directly with hardware, BrIan is extremely powerful, hence any error can be catastrophic. 
    Because of this BrIan is designed to be idiomatic and straight forward; avoiding confusing and hard to catch bugs that C++ is known for.
    Built upon the principles of C, BrIan is essentially a my attempt at a C++ without the verbosity, depreciation, and confusion. In BrIan, you won't need to constantly learn the latest and greatest functions in the standard library like C++; nor will you have to create everything by yourself like in C. BrIan takes the middle ground of these two, providing simple datastructures, algorithms and all that is required to build complex standalone projects, without the technical debt.
    There are many aspects of C and C++ that BrIan aims to fix:
    - Confusing operator prescedence
    - Confusing function pointers
    - Confusing implicit casting rules
    - Confusing pointer declarations 
        * Think comparisons between unsigned and signed in C
    - Lack of namespaces
    - Lack of lambdas 
    - Lack of interfaces and struct methods
    - Unintuitive strings
    - Ambiguous indentation with conditionals and loops
    - Ambiguous standard library
        * Especially regarding deprecated functions, best practices and obtuse implementations
    - Safety concerns 
        * BrIan will have safe pointers and safe dereferencing
    - Switch fall throughs 
    - Header files 
    - Macros


## BUILDING:

## DOCUMENTATION:

---

## GRAMMAR

' 
	Progam ::= ImportList "START" Body "UPDATE" Body  
    ImportList ::= { "#import" "<" IDENT ">" }  

	Thread ::= "THREAD" IDENT Body  

	Body ::= '{' StmtList '}' | LineStmt  
	StmtList ::= { Stmt }  
	Stmt ::= CtrlStmt | LineStmt ';'  
    LineStmt ::= DeclStmt | ExprStmt | ReturnStmt  

    ExprStmt ::= Expr ';'  
	DeclStmt ::= Type VarList ';'  
	CtrlStmt ::= IfStmt | SwitchStmt | WhileStmt | DoWhileStmt | ForStmt  
    ReturnStmt ::= return ';'  

    IfStmt ::= "if" '(' Expr ')' Body { "elif" '(' Expr ')' Body } { "else" Body }  
    SwitchStmt ::= "switch" '(' Expr ')' '{' { "case" Expr ':' StmtList } [ "default" ':' StmtList ] '}'  
    WhileStmt ::= "while" '(' Expr ')' Body  
    DoWhileStmt ::= "do" Body "while" CompStmt ';'  
    ForStmt ::= "for" '(' [Expr { ',' Expr} ] ';' Expr ';' Expr ')'  

    Expr ::= AsgnExpr  
    AsgnExpr ::= LogicExpr { ( '*=' | '/=' | '%=' | '+=' | '-=' | '&=' | '|=' | '^=' | '>>=' | '<<=' | '||=' | '&&=' | '=' ) LogicExpr }  
    LogicExpr ::= BitExpr { ( '<' '<=' '>' '>=' '==' '!=' '&& '||') BitExpr }  
    BitExpr ::= AddExpr { ( '&' '^' '|' '>>' '<<') AddExpr }  
	AddExpr ::= MultExpr { ( '+' | '-' ) MultExpr }  
	MultExpr ::= PowExpr { ( '*' | '/' | '%' ) PowExpr }  
    PowExpr ::= Prefix [ '**' PowExpr ]  
    Prefix ::= ( '++' | '--' | '!' | '~' | '(' Type ')' | '*' | '&' ) Prefix | Postfix  
    Postfix ::= Primary { '++' | '--' | '$' }  
    Primary ::= IDENT | LITERAL | '(' EXPR ')'  

    Type = ( char | bool | int | long | double | float | void | string )

	VarList ::= Var { ',' Var }
    Var = IDENT [ '=' Expr ]
	...	
'

---
## Presedence

1.)  () [] . ->  
2.)  ++ -- ** ! ~ (type) * & $      **Unary**  
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

## PROGRESS

    - [x] Tokenizer
    - [ ] All Tokens implemented
    - [ ] Parser and AST
    - [ ] Conversion to RISC V

---

'
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
'
