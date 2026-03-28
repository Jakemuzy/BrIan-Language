## Comments are discarded by tokenizer 
## '%' Means a register, allowed only during decl (similar to: int* var; )
## String underlying is { char*, U64 size, U64 max }
## Lambdas capture current scope 

```
	Program ::=  { Import } { Function | DeclStmt | InterfaceDecl }

    Import ::= '#' "import" [ "device" ] SLITERAL ';'

    Function ::= FuncDecl | FuncDef 
    FuncDecl ::= FuncSignature ';'
    FuncDef ::= FuncSignature Body

    FuncSignature ::= GenericFunc | RegularFunc
    GenericFunc ::= TypeQualifier Generic [ DeclPrefix ] IDENT GenericList '(' [ ParamList ] ')'
    RegularFunc ::= TypeQualifier Type [ DeclPrefix ] IDENT '(' [ ParamList ] ')'

    ParamList ::= ( Param | GenParam ) { ',' ( Param | Genparam ) }
    Param ::= TypeQualifier Type [ DeclPrefix ] IDENT
    GenParam ::= TypeQualifier Generic [ DeclPrefix ] IDENT 

    Lambda ::= "lambda" '(' [ParamList ] ')' Body     
	Body ::= '{' StmtList '}'
    StmtList ::= { Stmt }
	Stmt ::= CtrlStmt | DeclStmt | ExprStmt | ReturnStmt | JumpStmt | Comment

    ExprStmt ::= ';' | Expr ';'  
	DeclStmt ::= ( VarDecl | StructDecl | EnumDecl | TypedefDecl ) 
        VarDecl ::= ( Type | IDENT ) ( IDENT { ',' IDENT } )  ';'
        VarDef ::= ( Type | IDENT ) VarList ';'
        GenDecl ::= Generic Varlist
        StructDecl ::= GenericStruct | RegularStruct
        GenericStruct ::= "struct" IDENT GenericList '{' GenStructBody '}'
            GenStructBody :: { GenDecl | GenFunc }
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
	MultExpr ::= PowExpr { ( '*' | '/' | '%' ) PowExpr }  
    PowExpr ::= Prefix [ '**' PowExpr ]
    Prefix ::= ( '++' | '--' | '+' | '-' | '!' | '~' | '*' | '&' | Cast ) Prefix | Postfix 
        Cast ::= '(' Type | IDENT ')'
    Postfix ::= Primary { '++' | '--' | '$' | Index | CallFunc | Member }
        Index ::= '[' Expr' ']'
        CallFunc ::= IDENT '(' [ ArgList ] ')'
        Member ::= '.' IDENT
    Primary ::= Literal | PredefinedVars | '(' Expr ')' | Lambda

    Type ::= ( "char" | "bool" | "int" | "long" | "double" | "float" | "void" | "string" | "I8" | "I16" | "I32" | "I64" | "U8" | "U16" | "U32" | "U64" | Matrix | Vector ) 
        Matrix ::= "mat" '<' {1-9} 'x' {1-9} '>'
        Vector ::= "vec" '<' {1-9} '>'
    DeclPrefix ::= ( '*' | '%' )          
    GenericList ::= '<' Generic { ',' Generic } '>'
        Generic ::= IDENT
    TypeQualifier ::= [ const ] [ static ] [ volatile ] [ inline ] 

    Malloc ::= "malloc" '(' IDENT | Integral ')'
    Calloc ::= "calloc" '(' ( IDENT | Integral ) ',' ( IDENT | Integral ) ')'
    Realloc ::= "realloc" '(' IDENT ',' ( IDENT | Integral ) ')'
    SizeOf ::= "sizeof" '(' Type | IDENT ')'
    // Variadic paramaters 

    Transpose ::= "transpose" '(' IDENT ')' 
    Inverse ::= "inverse" '(' IDENT ')'

    Reg ::= Hex
    Hex :: = 0x{ [0-9] | [a-f] | [A-F] }
    PredefVars ::= ( "true" | "false" | "NULL" )


    ArgList = Expr { ',' Expr }

	VarList ::= Var { ',' Var }
    Var ::=  IDENT { ArrDecl } [ '=' ( Expr | ArrInitList ) | Reg ] 
    
    ArrDecl ::= '[' [ Expr ] ']'
    ArrInitList ::= '{' ( Literal | ArrInitList ) { ',' Literal | ArrInitList } '}' 
    Literal ::= IDENT | DECIMAL | INTEGRAL | SLITERAL | CLITERAL 
        Decimal ::= {1-9}['.'{1-9}]
        Integral ::= {1-9}
        Sliteral ::= \" { a-Z | 1-9 | EscapeSequence }  \"
            EscapeSequence ::= ( '\n' | '\t' | '\\' | '\'' | '\"' |  )
        Cliteral :: = \' [ a-Z | 1-9 | EscapeSequence ] \'

    Comment ::= ('//' { ? } '\n') | ('/*' { ? } '*/')
	...	
```