## Comments are discarded by tokenizer 
## '%' Means a register, allowed only during decl (similar to: int* var; )
## Strings are immutable
## Lambdas capture current scope 
## malloc, calloc, realloc, inverse and transpose are builtin functions, while sizeof is a keyword
## Vector and Matrix initalizers are just 1D and 2D arrays, this is already valid syntax in the language

```
	Program ::=  { Import } { Function | DeclStmt | InterfaceDecl }

    Import ::= '#' "import" [ "device" ] SLITERAL ';'

    Function ::= FuncDecl | FuncDef 
    FuncDecl ::= FuncSignature ';'
    FuncDef ::= FuncSignature Body

    FuncSignature ::= GenericFunc | RegularFunc
    GenericFunc ::= LinkageSpecifier TypeQualifier Generic [ DeclPrefix ] IDENT GenericList '(' [ ParamList ] ')'
    RegularFunc ::= LinkageSpecifier TypeQualifier Type [ DeclPrefix ] IDENT '(' [ ParamList ] ')'

    ParamList ::= ( Param | GenParam ) { ',' ( Param | Genparam ) }
    Param ::= TypeQualifier Type [ DeclPrefix ] IDENT
    GenParam ::= TypeQualifier Generic [ DeclPrefix ] IDENT 

    Lambda ::= "lambda" '(' [ParamList ] ')' Body     
	Body ::= '{' StmtList '}'
    StmtList ::= { Stmt }
	Stmt ::= CtrlStmt | DeclStmt | ExprStmt | ReturnStmt | JumpStmt | Comment

    ExprStmt ::= ';' | Expr ';'  
	DeclStmt ::= ( VarDecl | StructDecl | EnumDecl | TypedefDecl ) 
        VarDecl ::= LinkageSpecifier TypeQualifier ( Type | IDENT ) VarList ';'
        GenDecl ::= LinkageSpecifier TypeQualifier Generic Varlist ';'
        StructDecl ::= GenericStruct | RegularStruct
        GenericStruct ::= "struct" IDENT GenericList '{' GenStructBody '}'
            GenStructBody :: { GenDecl | GenericFunc }
        RegularStruct ::= "struct" IDENT [ ':' IDENT ] '{' StructBody '}' 
            StructBody ::= { DeclStmt | Function }
        InterfaceDecl ::= "interface" IDENT '{' InterfaceBody '}'
            InterfaceBody ::= { VarDecl | FuncDecl }

        EnumDecl ::= "enum" IDENT EnumBody 
            EnumBody ::= '{' IDENT [ = INTEGRAL ] { ',' IDENT [ = INTEGRAL ] } '}'
        TypedefDecl ::= "typedef" TypeSpec IDENT
            TypeSpec ::= ( Type | IDENT ) { TypedefPostfix }
            TypedefPostfix ::= ( '*' | '[' [ INTEGRAL ] ']' )
	CtrlStmt ::= IfStmt | SwitchStmt | WhileStmt | DoWhileStmt | ForStmt  
    ReturnStmt ::= "return" [Expr] ';'  
    JumpStmt ::= ( "break" || "continue" ) ';'

    IfStmt ::= "if" '(' Expr ')' Body { "elif" '(' Expr ')' Body } [ "else" Body ] 
    SwitchStmt ::= "switch" '(' Expr ')' '{' {CaseStmt} [DefaultStmt] '}'
        Case ::= "case" Literal Body       
        Default ::= "default" Body
    WhileStmt ::= "while" '(' Expr ')' Body
    DoWhileStmt ::= "do" ( Body ) 
                    "while" '(' Expr ')' ';'  
    ForStmt ::= "for" '(' [ VarExprList ]';' [ Expr ] ';' [ ExprList ] ')' Body

    VarExprList ::= ( Expr | VarDecl ) { ',' ( Expr | VarDecl ) }
    ExprList ::= Expr { ',' Expr }

    Expr ::= TernaryExpr  
    TernaryExpr ::= AsgnExpr [ '?' Expr ':' TernaryExpr ]
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
	MultExpr ::= PowExpr { ( '*' | '/' | '%' | '@' ) PowExpr }  
    PowExpr ::= Prefix [ '**' PowExpr ]
    Prefix ::= ( '++' | '--' | '+' | '-' | '!' | '~' | '*' | '&' | Cast ) Prefix | Postfix 
        Cast ::= '(' Type | IDENT ')'
    Postfix ::= Primary { '++' | '--' | Index | CallFunc | Member | Ref | SafeMem | SafeRef }
        Index ::= '[' Expr' ']'
        CallFunc ::= '(' [ ArgList ] ')'    
        Member ::= '.' IDENT
        Ref ::= '->' IDENT
        SafeMem ::= '.?' IDENT
        SafeRef ::= '->?' IDENT
    Primary ::= IDENT | Literal | PredefinedVars | SizeOf | '(' Expr ')' | Lambda

    Type ::= ( "char" | "bool" | "int" | "long" | "double" | "float" | "void" | "string" | "I8" | "I16" | "I32" | "I64" | "U8" | "U16" | "U32" | "U64" | Matrix | Vector ) 
        Matrix ::= "mat" '<' {1-9} 'x' {1-9} '>'
        Vector ::= "vec" '<' {1-9} '>'
    DeclPrefix ::= ( '*' | '%' )          
    GenericList ::= '<' Generic { ',' Generic } '>'
        Generic ::= IDENT
    TypeQualifier ::= [ const ] [ static ] [ volatile ] [ inline ] 
    LinkageSpecifier ::= [ extern ]

    SizeOf ::= "sizeof" '(' Type | IDENT ')'

    Reg ::= Hex
    Hex :: = 0x{ [0-9] | [a-f] | [A-F] }
    PredefVars ::= ( "true" | "false" | "NULL" )


    ArgList = Expr { ',' Expr }

    // VEC AND MAT INITALIZERS NOT YET CREATED, 
    // Implement as 1D and 2D arrays, already handled nested arrays  
	VarList ::= Var { ',' Var }
    Var ::=  IDENT { ArrDecl } [ '=' ( Expr | ArrInitList ) | Reg ] 
    
    ArrDecl ::= '[' [ Expr ] ']'
    ArrInitList ::= '{' ( IDENT | Literal | ArrInitList ) { ',' ( IDENT | Literal | ArrInitList ) } '}' 
    Literal ::= DECIMAL | INTEGRAL | SLITERAL | CLITERAL 
        Decimal ::= {1-9}['.'{1-9}]
        Integral ::= {1-9}
        Sliteral ::= \" { a-Z | 1-9 | EscapeSequence }  \"
            EscapeSequence ::= ( '\n' | '\t' | '\\' | '\'' | '\"' |  )
        Cliteral :: = \' [ a-Z | 1-9 | EscapeSequence ] \'

    Comment ::= ('//' { ? } '\n') | ('/*' { ? } '*/')
	...	
```