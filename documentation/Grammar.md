```
	Program ::=  { Function | DeclStmt }

    Function ::= Type IDENT '(' [ ParamList ] ')' Body
    ParamList ::= Param { ',' Param }
    Param ::= Type IDENT


// NOT IMPLEMENTED YET
    Lambda ::= "func" '(' [ParamList ] ')' Body     


	Body ::= '{' StmtList '}'
	StmtList ::= { Stmt }  
	Stmt ::= CtrlStmt | DeclStmt | ExprStmt | ReturnStmt 

    ExprStmt ::= ';' | Expr ';'  
	DeclStmt ::= ( VarDecl | StructDecl | EnumDecl | TypedefDecl ) ';'
        VarDecl ::= ( Type | IDENT ) Varlist 
        StructDecl ::= "struct" IDENT '{' StructBody '}' 
            StructBody ::= { DeclStmt | Function }
        EnumDecl ::= "enum" IDENT EnumBody 
            EnumBody ::= '{' IDENT [ = INTEGRAL ] { ',' IDENT [ = INTEGRAL ] } '}'
        TypedefDecl ::= "typedef" TypeSpec IDENT
            TypeSpec ::= ( Type | IDENT ) { TypedefPostfix }
            TypedefPostfix ::= ( '*' | '[' [ INTEGRAL ] ']' )
	CtrlStmt ::= IfStmt | SwitchStmt | WhileStmt | DoWhileStmt | ForStmt  
    ReturnStmt ::= "return" [Expr] ';'  

    IfStmt ::= If { Elif } [ Else ] 
        If ::= "if" '(' Expr ')' Body
        Elif ::= "elif" '(' Expr ')' Body 
        Else ::= "else" Body 
    SwitchStmt ::= "switch" '(' Expr ')' '{' {CaseStmt} [DefaultStmt] '}'
        Case ::= "case" Expr Body       // ALLOW ONLY LITERALS 
        Default ::= "default" Body
    WhileStmt ::= "while" '(' Expr ')' Body
    DoWhileStmt ::= "do" ( Body ) 
                    "while" '(' Expr ')' ';'  
    ForStmt ::= "for" '(' [ ExprList ]';' [ Expr ] ';' [ ExprList ] ')' Body

    VarExprList ::= ( Expr | VarDecl ) { ',' ( Expr | VarDecl ) }
    ExprList ::= Expr { ',' Expr }

    Expr ::= AsgnExpr  
    AsgnExpr ::= OrlExpr [ ( '=' | '+=' | '-=' | '*=' | '/=' | '%=' | '<<=' | '>>=' | '&=' | '^=' | '|=' | '&&=' | '||=') AsgnExpr ]
    OrlExpr ::= AndlExpr { '||' AndlExpr }  
    AndlExpr ::= OrExpr { '&&' OrExpr } 
    OrExpr ::= XorExpr { '|' XorExpr }
    XorExpr ::= AndExpr { '^' AndExpr }
