### Comments are discarded by tokenizer 
### '%' Means a register, allowed only during decl (similar to: int* var; )
### Strings are immutable
### Lambdas capture specified variables
### malloc, calloc, realloc, inverse and transpose are builtin functions, while sizeof is a keyword
### Vector and Matrix initalizers are just 1D and 2D arrays, this is already valid syntax in the language
### enum underlying type is i32
### Operator overloading is restricted to non assignment operators to prevent weird move semantics
### Operator overloading is also restriced to user defined types, to prevent compiler overhead and unintended hidden behavior
### An interesting thing about generics, they require <> surrounding the generic to signifiy a generic return type
### <- as a prefix indicates a message will receive, where as a posfix indicates it will send
### Restrictions on type qualifiers will be analyzed during later semantic analysis steps (ie paramaters, function pointers, etc )

```
	Program ::=  { Import | Directive } { Function | DeclStmt | InterfaceDecl }

    Import ::= '#' "import" [ "device" ] SLITERAL ';'
    Directive ::= '#' ( Ifdir | Elifdir | Elsedir | Endifdir | Pragmadir | Errdir ) ';'
        Ifdir ::= "if" CondExpr 
        ElifDir ::= "elif" CondExpr
        Elsedir ::= "else" 
        Endifdir ::= "endif"
        Pragmadir ::= "pragma" IDENT { IDENT | INTEGRAL | SLITERAL }
        Errdir ::= "error" SLITERAL 

    CondExpr ::= IDENT | INTEGRAL
           | CondExpr ( '&&' | '||' ) CondExpr
           | CondExpr ( '==' | '!=' | '<' | '>' | '<=' | '>=' ) CondExpr
           | '!' CondExpr
           | '(' CondExpr ')'

    Function ::= FuncDecl | FuncDef 
    FuncDecl ::= FuncSignature ';'
    FuncDef ::= FuncSignature Body

    FuncSignature ::= GenericFunc | RegularFunc
    GenericFunc ::= "fn" LinkageSpecifier TypeQualifier Generic IDENT GenericList '(' [ ParamList ] ')'
    RegularFunc ::= "fn" LinkageSpecifier TypeQualifier ( Type | IDENT ) [ DeclPrefix ] IDENT '(' [ ParamList ] ')'

    ParamList ::= Param  { ',' Param }
    Param ::= TypeQualifier ( Type | IDENT ) [ DeclPrefix ] IDENT
    AnonParamList ::= ( Type | IDENT ) { ',' ( Type | Ident ) }   

    Lambda ::= "lambda" ( Type | IDENT ) '(' [ ParamList ] ')' Captures Body     
        Captures ::= "captures" IDENT { IDENT { ',' IDENT } }
	Body ::= '{' StmtList '}'
    StmtList ::= { Stmt }
	Stmt ::= CtrlStmt | DeclStmt | ExprStmt | ReturnStmt | JumpStmt | ConcurrencyStmt

    ExprStmt ::= ';' | Expr ';'  
	DeclStmt ::= ( VarDecl  | EnumDecl | TypedefDecl ) ';'
                 | StructDecl        // Semicolon after struct is annoying

    VarDecl ::= "let" LinkageSpecifier TypeQualifier ( Type | IDENT ) VarList 
    GenDecl ::= "let" LinkageSpecifier TypeQualifier Generic VarList
    StructDecl ::= GenericStruct | RegularStruct
    GenericStruct ::= "struct" IDENT GenericList '{' GenStructBody '}'
        GenStructBody ::= { GenDecl | GenericFunc | OperatorOverload }
    RegularStruct ::= "struct" IDENT [ Implements ] '{' StructBody '}' 
        StructBody ::= { DeclStmt | Function | OperatorOverload }
        Implements ::= ':' IDENT { IDENT { ',' IDENT } }
        OperatorOverload ::= "operator" OverloadableOp '(' Param [ ',' Param ] ')' Body
        OverloadableOp   ::= '+' | '-' | '*' | '/' | '%' | '@'
                        | '==' | '!=' | '<' | '>' | '<=' | '>='
                        | '<<' | '>>' | '&' | '|' | '^' | '~'
                        | "[]"
    InterfaceDecl ::= "interface" IDENT '{' InterfaceBody '}'
        InterfaceBody ::= { ( VarDecl | FuncDecl ) ';' }

    EnumDecl ::= "enum" IDENT EnumBody 
        EnumBody ::= '{' IDENT [ = INTEGRAL ] { ',' IDENT [ = INTEGRAL ] } '}'
    TypedefDecl ::= "typedef" TypeSpec IDENT
        TypeSpec ::= ( Type | IDENT ) { TypedefPostfix }
        TypedefPostfix ::= ( '*' | '[' [ INTEGRAL ] ']' )

    ConcurrencyStmt ::= LockStmt | CriticalStmt 
        LockStmt     ::= "lock" '(' Expr ')' Body
        CriticalStmt ::= "critical" Body


	CtrlStmt ::= IfStmt | SwitchStmt | WhileStmt | DoWhileStmt | ForStmt  
    ReturnStmt ::= "return" [Expr] ';'  
    JumpStmt ::= ( "break" || "continue" ) ';'

    IfStmt ::= "if" '(' Expr ')' Body { "elif" '(' Expr ')' Body } [ "else" Body ] 
    SwitchStmt ::= "switch" '(' Expr ')' '{' {CaseStmt} [DefaultStmt] '}'
        Case ::= "case" Literal Body       
        Default ::= "default" Body
    WhileStmt ::= "while" '(' Expr ')' Body
    DoWhileStmt ::= "do" ( Body ) "while" '(' Expr ')' ';'  
    ForStmt ::= "for" '(' [ VarDecl | ExprList ]';' [ Expr ] ';' [ ExprList ] ')' Body
        ExprList ::= Expr { ',' Expr }

    Expr ::= TernaryExpr  
    TernaryExpr ::= AsgnExpr [ '?' Expr ':' TernaryExpr ]
    AsgnExpr ::= OrlExpr [ ( '=' | '+=' | '-=' | '*=' | '/=' | '%=' | '<<=' | '>>=' | '&=' | '^=' | '|=' | '&&=' | '||=' | '<-' ) AsgnExpr ]
    OrlExpr ::= AndlExpr { '||' AndlExpr }  
    AndlExpr ::= OrExpr { '&&' OrExpr } 
    OrExpr ::= XorExpr { '|' XorExpr }
    XorExpr ::= AndExpr { '^' AndExpr }
    AndExpr ::= EqqExpr { '&' EqqExpr }
    EqqExpr ::= CompExpr [ ('==' | '!=') CompExpr ]
    CompExpr ::= ShiftExpr [ ('>' | '<' | '<=' | '>=') ShiftExpr ]  
    ShiftExpr ::= AddExpr { ('<<' | '>>') AddExpr }  
	AddExpr ::= MultExpr { ( '+' | '-' ) MultExpr }  
	MultExpr ::= PowExpr { ( '*' | '/' | '%' | '@' ) PowExpr }  
    PowExpr ::= Prefix [ '**' PowExpr ]
    Prefix ::= ( '++' | '--' | '+' | '-' | '!' | '~' | '*' | '&' | "spawn" | "await" | '<-' ) Prefix | Postfix 
    Postfix ::= Primary { '++' | '--' | Cast | Index | CallFunc | Member | Ref | SafeMem | SafeRef }
        Cast ::= "as" ( Type | IDENT )
        Index ::= '[' Expr' ']'
        CallFunc ::= '(' [ ArgList ] ')'    
        Member ::= '.' IDENT
        Ref ::= '->' IDENT
        SafeMem ::= '.?' IDENT
        SafeRef ::= '->?' IDENT
    Primary ::= IDENT | Literal | PredefVars | SizeOf | '(' Expr ')' | Lambda

    Type ::= ( "char" | "bool" | "int" | "long" | "double" | "float" | "void" | "string" | "I8" | "I16" | "I32" | "I64" | "U8" | "U16" | "U32" | "U64" | Matrix | Vector | "mutex" | "semaphore" | "task" | Channel | FuncPointer | Closure ) 
        Channel ::= "chan" '<' ( Type | IDENT ) '>'
        Matrix ::= "mat" '<' {1-9} ',' {1-9} '>'
        Vector ::= "vec" '<' {1-9} '>'
        FuncPointer ::= "fp" [ TypeQualifier ] FuncPointerSignature
        Closure ::= "closure" [ TypeQualifier ] FuncPointerSignature
        FuncPointerSignature ::= ( Type | IDENT ) '(' [ AnonParamList ] ')' 
    DeclPrefix ::= ( '*' | '%' )          
    GenericList ::= '<' IDENT { ',' IDENT } '>'
        Generic ::= '<' IDENT '>'
    TypeQualifier ::= { TypeQualifierItem }
    TypeQualifierItem ::= "static" | "inline" | "const" | "volatile" | "atomic"
    LinkageSpecifier ::= [ extern ]

    SizeOf ::= "sizeof" '(' ( Type | IDENT ) ')'

    Reg ::= Hex
    Hex :: = 0x{ [0-9] | [a-f] | [A-F] }
    PredefVars ::= ( "true" | "false" | "NULL" )


    ArgList = Expr { ',' Expr }

	VarList ::= Var { ',' Var }
    Var ::=  IDENT { ArrDecl } [ '=' ( Expr | ArrInitList ) ] 
    
    ArrDecl ::= '[' [ Expr ] ']'
    ArrInitList ::= '{' ( IDENT | Literal | ArrInitList ) { ',' ( IDENT | Literal | ArrInitList ) } '}' 
    Literal ::= DECIMAL | INTEGRAL | SLITERAL | CLITERAL | Hex
        Decimal  ::= {0-9} [ '.' {0-9} ] [ ('e' | 'E') [ '+' | '-' ] {0-9} ]
        Integral ::= {0-9}
        Sliteral ::= \" { a-Z | 1-9 | EscapeSequence }  \"
            EscapeSequence ::= ( '\n' | '\t' | '\\' | '\'' | '\"' |  )
        Cliteral :: = \' [ a-Z | 1-9 | EscapeSequence ] \'
```