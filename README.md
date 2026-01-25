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


## BUILDING [WIP]:
    1.) Make sure to have gcc and cmake installed
    2.) Compile the source code for your machine 
    3.) Run bcc on your .b file 
        ie.) ./bcc TestCases.b
    4.) ...

## DOCUMENTATION:

---

## TODO
    -   Allow preprocessor directives anywhere
    -   Global decl statements shouldn't cause ast to error
    -   Accidentally mixed up braces and brackets, fix later 

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
    Primary ::= Literal | '(' Expr ')'

    Type = ( "char" | "bool" | "int" | "long" | "double" | "float" | "void" | "string" )
    ArgList = Expr { ',' Expr }

	VarList ::= Var { ',' Var }
    Var ::= IDENT [ '=' Expr ] |
            IDENT '[' [ Expr ] ']' [ '=' ArrInitList ]
    ArrInitlist ::= '{' Literal { ',' Literal } '}' 
    Literal ::= IDENT | DECIMAL | INTEGRAL | SLITERAL | CLITERAL 
	...	
```

---

## Precedence and Associativity
Similar to C, but simpler.

| Precedence     | Operators / Constructs                                      | Associativity    |
|---------------:|------------------------------------------------------------|----------------|
| 1 (lowest)     | `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `<<=`, `>>=`, `&=`, `^=`, `\|=`, `&&=`, `\|\|=` | Right-to-left  |
| 2              | `\|\|`                                                    | Left-to-right  |
| 3              | `&&`                                                      | Left-to-right  |
| 4              | `\|`                                                      | Left-to-right  |
| 5              | `^`                                                       | Left-to-right  |
| 6              | `&`                                                       | Left-to-right  |
| 7              | `==`, `!=`                                                | Left-to-right  |
| 8              | `>`, `<`, `>=`, `<=`                                      | Left-to-right  |
| 9              | `<<`, `>>`                                                | Left-to-right  |
| 10             | `+`, `-`                                                  | Left-to-right  |
| 11             | `*`, `/`, `%`                                             | Left-to-right  |
| 12             | `**`                                                      | Right-to-left  |
| 13             | Prefix: `++`, `--`, `+`, `-`, `!`, `~`, `*`, `&`, Cast    | Right-to-left  |
| 14             | Postfix: `++`, `--`, `$`, Index, CallFunc                 | Left-to-right  |
| 15 (highest)   | Primary: `IDENT`, `DECIMAL`, `INTEGRAL`, `SLITERAL`, `CLITERAL`, `( Expr )` | N/A            |


### NOTES
    BrIan is type safe
    BrIan allows for easy bit manipulation

## PROGRESS

    - [x] Tokenizer
    - [ ] All Tokens implemented
        - [x] C-Like tokens 
        - [x] Register tokens 
        - [x] Safe memory tokens
        - [ ] Bit manipulation tokens
    - [ ] Proper Preprocessor 
    - [x] Parser and AST
    - [ ] Semanitc Analysis
        - [ ] Name Resolution
            - [x] Variable Shadowing
            - [ ] Symbol Table
            - [ ] Name Spaces
            - [ ] Overloading
        - [ ] Type Checking
        - [ ] Desugaring
        - [ ] Concurrency Checking
    - [ ] Conversion to LLVM IR
        - [ ] Assemble to RISC V
---

## PIPELINE DESIGN

    1. Raw input file
    2. Preprocesser  -> 
            TBD...
    3. Tokenizer     -> 
            Tokenizes input file one at a time
    4. Parser        -> 
            Calls the tokenizer one token at a time, builds an AST 
            based off the input file. Returns a syntax error if code is invalid.
    5. Name Resolver -> 
            Determines scope and availability of names by generating
            a symbol table, goal is to ensure names are consistent and available given scope. Also stores type information for later steps.
    6. Type Checker  -> 
            Checks the symbol table for any invalid typings
    7. Desugarizer   ->
            Removes syntactic "sugar" converting the ast to a form that is easily translated to BrIRan (BrIan Intermediate Language)
    7. IR Generation ->
            Generates LLVM
    8. RISC Assembly ->
            TBD...


`
DATA TYPES [WIP]:
    Integral:
        char(1), bool(1), short(2), int(4), long(8)
    Decimal:
        float(4), double(8)
    Special:
        binary(1), hexadecimal(8), string(inf), safeptr(inf), struct(inf), enum(inf)

TYPE QUALIFIERS:
    unsigned, signed, const, new

CONTROL STATEMENTS:
    if, else, do, while, switch, case, break, continue, goto, return
`
