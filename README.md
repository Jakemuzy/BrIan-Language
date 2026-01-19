# Introducing BrIan

A Compiled Language built with **concurrency** in mind. Built mainly for embedded systems, compiles to the **RISC-V** ISA

---

## PRINCIPLES:
    Brian's main goals are 
        - Compiled 
        - Concurrency 
        - Low memory utilization 
        - Type and memory safety
    BrIan strives to be simple but extremely powerful. By allowing programmers to interface directly with hardware,
    BrIan is extremely powerful, hence any error can be catastrophic. 
    Because of this BrIan is designed to be idiomatic and straight forward; 
        avoiding confusing and hard to catch bugs that C++ is known for.
    Built upon the principles of C, BrIan is essentially my attempt at a C++ without the verbosity, depreciation, and confusion. 
        In BrIan, you won't need to constantly learn the latest and greatest functions in the standard library like C++; 
        nor will you have to create everything by yourself like in C. 
        BrIan takes the middle ground of these two, providing simple algorithms, data structures and everything that is required 
        to build complex standalone projects, without the technical debt.
    There are many aspects of C and C++ that BrIan aims to fix:
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
        * Especially regarding deprecated functions, 
          best practices and obtuse implementations
    - Safety concerns 
        * BrIan will have safe pointers and safe dereferencing
    - Header files 
    - Macros


## BUILDING:
    1.) Make sure to have gcc installed
    2.) ...

## DOCUMENTATION:

---

## TODO
    -   Allow preprocessor directives anywhere
    -   

## GRAMMAR

```
	Program ::=  { Function | DeclStmt }
    Function ::= Type IDENT '(' [ ParamList ] ')' Body
    ParamList ::= Param { ',' Param }
    Param ::= Type IDENT

	Body ::= '{' StmtList '}'
	StmtList ::= { Stmt }  
	Stmt ::= CtrlStmt | DeclStmt | ExprStmt | ReturnStmt 

    ExprStmt ::= ';' | Expr ';'  
	DeclStmt ::= Type Varlist ';'  
	CtrlStmt ::= IfStmt | SwitchStmt | WhileStmt | DoWhileStmt | ForStmt  
    ReturnStmt ::= "return" [Expr] ';'  

    IfStmt ::= If { Elif } [ Else ] 
        If ::= "if" '(' Expr ')' Body
        Elif ::= "elif" '(' Expr ')' Body 
        Else ::= "else" Body 
    SwitchStmt ::= "switch" '(' Expr ')' '{' {CaseStmt} [DefaultStmt] '}'
        Case ::= "case" Expr Body
        Default ::= "default" Body
    WhileStmt ::= "while" '(' Expr ')' Body
    DoWhileStmt ::= "do" ( Body ) 
                    "while" '(' Expr ')' ';'  
    ForStmt ::= "for" '(' [ ExprList ]';' [ Expr ] ';' [ ExprList ] ')' Body

    ExprList ::= Expr { ',' Expr }
    Expr ::= AsgnExpr  
    AsgnExpr ::= OrlExpr [ ( '=' | '+=' | '-=' | '*=' | '/=' | '%=' | '<<=' | '>>=' | '&=' | '^=' | '|=' | '&&=' | '||=') AsgnExpr ]
    OrlExpr ::= AndlExpr { '||' AndlExpr }  
    AndlExpr ::= OrExpr { '&&' OrExpr } 
    OrExpr ::= XorExpr { '|' XorExpr }
    XorExpr ::= AndExpr { '^' AndExpr }
    AndExpr ::= EqqExpr { '&' EqqExpr }
    EqqExpr ::= RelationExpr [ ('==' | '!=') RelationExpr ]
    RelationExpr ::= ShiftExpr [ ('>' | '<' | '<=' | '>=') ShiftExpr ]  
    ShiftExpr ::= AddExpr { ('<<' | '>>') AddExpr }  
	AddExpr ::= MultExpr { ( '+' | '-' ) MultExpr }  
	MultExpr ::= PowExpr { ( '*' | '/' | '%' ) PowExpr }  
    PowExpr ::= Prefix [ '**' PowExpr ]
    Prefix ::= ( '++' | '--' | '+' | '-' | '!' | '~' | '*' | '&' | Cast ) Prefix | Postfix 
        Cast ::= '(' Type ')'
    Postfix ::= Primary { '++' | '--' | '$' | Index | CallFunc }
        Index ::= '[' Expr' ']'
        CallFunc ::= IDENT '(' [ ArgList ] ')'
    Primary ::= IDENT | DECIMAL | INTEGRAL | SLITERAL | CLITERAL | '(' Expr ')'

    Type = ( "char" | "bool" | "int" | "long" | "double" | "float" | "void" | "string" )
    ArgList = Expr { ',' Expr }

	VarList ::= Var { ',' Var }
    Var = IDENT [ '=' Expr ]

	...	
```

---
## Presedence And Associativity


### NOTES
    BrIan is type safe
    BrIan allows for easy bit manipulation

## PROGRESS

    - [x] Tokenizer
    - [ ] All Tokens implemented
    - [ ] Parser and AST
    - [ ] Conversion to RISC V

---

`
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
`
