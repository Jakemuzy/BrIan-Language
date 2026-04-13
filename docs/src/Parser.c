#include "Parser.h"

/* ----- Small Helpers ----- */

#define TYPE_CASES \
    case CHAR: case BOOL: case INT: case LONG: case DOUBLE: case FLOAT: \
    case VOID: case STRING: case I8: case I16: case I32: case I64: \
    case U8: case U16: case U32: case U64: case MAT: case VEC: \
    case MUTEX: case SEMAPHORE: case TASK: case CHANNEL: case FUNCPTR: \
	case CLOSURE: case IDENT:

#define QUALIFIER_CASES \
	case STATIC: case INLINE: case CONST: case VOLATILE: case ATOMIC:

#define EXPR_START_CASES \
    case IDENT: case INTEGRAL: case REAL: case CLITERAL: case SLITERAL: case SIZEOF: \
    case LPAREN: case LAMBDA: \
    case PLUS: case MINUS: case NOT: case NEG: case MULT: case AND: \
    case INC: case DEC: \
    case SPAWN: case AWAIT: case SEND: \
    case HEX: case TRUE: case FALSE: case NILL:
/* Helpers */

static inline void Advance(ParserContext* ctx) { ctx->current = GetNextToken(ctx->tokenizer); }

static inline bool Match(ParserContext* ctx, TokenType t) {
    if (ctx->current.type != t) return false;
    Advance(ctx);
    return true;
}

static inline void SyncRecovery(ParserContext* ctx, TokenType tt) 
{
    while (ctx->current.type != tt && ctx->current.type != END) 
        Advance(ctx);
	ctx->panicMode = false;
}

static inline ASTNode* ParseERROR(ParserContext* ctx, char* format) {
    ctx->failure = true; ctx->panicMode = true;
    char buf[256];
    snprintf(buf, sizeof(buf), "%s Instead encountered '%s' on line %d, col %d\n",
             format, ctx->current.lexeme, ctx->current.row, ctx->current.col);
    ERROR(ERR_FLAG_CONTINUE, PARSER_ERR, "%s", buf);
    return NULL;
}


/* ----- Recursive Descent ----- */


ParserContext* InitalizeParserContext(TokenizerContext* tokenizer)
{
    ParserContext* parser = malloc(sizeof(ParserContext));

	parser->failure = false; parser->panicMode = false;
    parser->tokenizer = tokenizer;
	// Share the same arena to avoid token's str copy issues 
    parser->arena = tokenizer->arena;	
    parser->ast = InitalizeAST(parser->arena);
    return parser;
}

void DestroyParserContext(ParserContext* ctx)
{
    ResetArena(ctx->arena);
}


/* ----- Recursive Descent ----- */

void Program(ParserContext* ctx) 
{
	// Advance one to start, every other instance the child function Advances first before returning
	Advance(ctx);	
    while (true) {
        switch (ctx->current.type) {
            case FUNCTION: 
                ASTNode* funcNode = Function(ctx);
				if (ctx->panicMode) { SyncRecovery(ctx, RBRACE); Advance(ctx); }
                AddChildASTNode(ctx->arena, ctx->ast->root, funcNode);

				// TODO: func decls SHOULD require semi after in global scope
                break;
            case INTERFACE:
                ASTNode* interfaceDeclNode = InterfaceDecl(ctx);
				if (ctx->panicMode) { SyncRecovery(ctx, RBRACE); Advance(ctx); }
                else AddChildASTNode(ctx->arena, ctx->ast->root, interfaceDeclNode);
                break;
			case LET:
                ASTNode* varDeclNode = VarDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, SEMI);
                else AddChildASTNode(ctx->arena, ctx->ast->root, varDeclNode);

				if (!Match(ctx, SEMI)) { ParseERROR(ctx, "Expected semicolon ';' after variable declartion."); return; }
                break;
			case ENUM:
				ASTNode* enumNode = EnumDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, ctx->ast->root, enumNode);

				if (!Match(ctx, SEMI)) ParseERROR(ctx, "Expected semicolon ';' after enum declaration.");
				break;
			case TYPEDEF: 
				ASTNode* typedefNode = TypedefDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, ctx->ast->root, typedefNode);

				if (!Match(ctx, SEMI)) ParseERROR(ctx, "Expected semicolon ';' after typedef declaration.");
				break;
			case STRUCT:
				ASTNode* structNode = StructDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, ctx->ast->root, structNode);
				break;
            case END:   
                return;
			default:	
				ParseERROR(ctx, "Unexpected token occured in global scope."); return;
        }
    }
}


ASTNode* DeclQualifiers(ParserContext* ctx)
{
	return NULL;
}

ASTNode* Function(ParserContext* ctx)
{
	ASTNode* funcNode = NULL, *linkageNode = NULL, *qualifierNode = NULL;
	Advance(ctx);

	/* Optional Specifiers and Qualifiers */
	if (ctx->current.type == EXTERN) { linkageNode = LinkageSpecifier(ctx); }
	switch(ctx->current.type) { QUALIFIER_CASES  qualifierNode = TypeQualifierList(ctx); default: break; }

	switch (ctx->current.type) {
		TYPE_CASES
			funcNode = RegularFunc(ctx); break;
		case LESS:   funcNode = GenericFunc(ctx); break;
		default: return ParseERROR(ctx, "Expected function return type or generic return type.");
	}
	if (ctx->panicMode) return NULL;

	if (linkageNode) PrependChildASTNode(ctx->arena, funcNode, linkageNode);
	if (qualifierNode) PrependChildASTNode(ctx->arena, funcNode, qualifierNode);

	switch (ctx->current.type) {
		case SEMI: 
			funcNode->type = FUNC_DECL;
			break;
		case LBRACE:
			funcNode->type = FUNC_DEF;
			ASTNode* bodyNode = Body(ctx);
			if (ctx->panicMode) return NULL;
			else AddChildASTNode(ctx->arena, funcNode, bodyNode);
			break;
		default:
			return ParseERROR(ctx, "Expected '{' for function definition or ';' for function declaration.");
	}

	return funcNode;
}

ASTNode* GenericFunc(ParserContext* ctx)
{
	ASTNode* genericNode = Generic(ctx);
	if (ctx->panicMode) SyncRecovery(ctx, LPAREN);

	if (ctx->current.type != IDENT) return ParseERROR(ctx, "Function name expected.");
	ASTNode* funcNode = InitalizeASTNode(ctx->arena, GEN_FUNC_NODE, ctx->current);
	AddChildASTNode(ctx->arena, funcNode, genericNode);
	Advance(ctx);

	// Generic paramaters are optional
	if (ctx->current.type == LESS) {
		ASTNode* genricParams = GenericList(ctx);
		if (ctx->panicMode) SyncRecovery(ctx, LPAREN);
		else AddChildASTNode(ctx->arena, funcNode, genricParams);
	}

	if (!Match(ctx, LPAREN)) return ParseERROR(ctx, "Expected '(' after function name.");

	switch (ctx->current.type) {
		QUALIFIER_CASES
		TYPE_CASES
			ASTNode* paramListNode = ParamList(ctx);
			if (ctx->panicMode) SyncRecovery(ctx, RPAREN);
			else AddChildASTNode(ctx->arena, funcNode, paramListNode);
			break;
		default: break; // Maybe have this do something? Empty node?
	}

	if (!Match(ctx, RPAREN)) return ParseERROR(ctx, "Expected ')' after function paramaters.");
	return funcNode;
}

ASTNode* RegularFunc(ParserContext* ctx)
{
	// Function pointers need differentiation
	ASTNode* typeNode = Type(ctx);
	if (ctx->panicMode) SyncRecovery(ctx, IDENT);

	// TODO: For custom types this will be the wrong message
	if (ctx->current.type != IDENT) return ParseERROR(ctx, "Function name expected.");
	ASTNode* funcNode = InitalizeASTNode(ctx->arena, REGULAR_FUNC_NODE, ctx->current);
	Advance(ctx);

	AddChildASTNode(ctx->arena, funcNode, typeNode);

	if (!Match(ctx, LPAREN)) return ParseERROR(ctx, "Expected '(' after function name.");

	switch (ctx->current.type) {
		QUALIFIER_CASES
		TYPE_CASES
			ASTNode* paramListNode = ParamList(ctx);
			if (ctx->panicMode) SyncRecovery(ctx, RPAREN);
			else AddChildASTNode(ctx->arena, funcNode, paramListNode);
			break;
		default: break; // Maybe have this do something? Empty node?
	}

	if (!Match(ctx, RPAREN)) return ParseERROR(ctx, "Expected ')' after function paramaters.");

	return funcNode;
}


ASTNode* ParamList(ParserContext* ctx)
{
	ASTNode* paramListNode = InitalizeASTNode(ctx->arena, PARAM_LIST_NODE, DUMMY_TOKEN);

	while (true) {
		ASTNode* qualifierNode = NULL;
		/* TODO: SEPARATE FUNC QUALIFIERS ? */
		switch (ctx->current.type) { QUALIFIER_CASES qualifierNode = TypeQualifier(ctx); default: break; }

		switch (ctx->current.type) {
			TYPE_CASES
				ASTNode* paramNode = Param(ctx);
				if (qualifierNode) PrependChildASTNode(ctx->arena, paramNode, qualifierNode);

				// TODO: Ideal to go to comma THEN rparen, but for later
				if (ctx->panicMode) SyncRecovery(ctx, RPAREN);
				else AddChildASTNode(ctx->arena, paramListNode, paramNode);
				break;
			case RPAREN: return paramListNode;
			default: return ParseERROR(ctx, "Expected paramaters inside function signature.");
		}

		switch (ctx->current.type) {
			case COMMA: Advance(ctx); break;
			case RPAREN: return paramListNode;
			default: break;
		}
	}
}

ASTNode* Param(ParserContext* ctx)
{
	ASTNode* typeNode = Type(ctx);
	if (ctx->panicMode) SyncRecovery(ctx, IDENT);

    if (ctx->current.type != IDENT) return ParseERROR(ctx, "Expected parameter identifier.");
	ASTNode* paramNode = InitalizeASTNode(ctx->arena, PARAM_NODE, ctx->current);
    Advance(ctx);

	AddChildASTNode(ctx->arena, paramNode, typeNode);
    return paramNode;
}

ASTNode* Lambda(ParserContext* ctx)
{
	Advance(ctx);
	ASTNode* lambdaNode = InitalizeASTNode(ctx->arena, LAMBDA_NODE, DUMMY_TOKEN);

	// Function Pointer should impelment AnonParamList
	switch (ctx->current.type) {
		TYPE_CASES 
			// TODO: Types are ambigous with lambda paramater list 	
			ASTNode* typeNode = Type(ctx);
			if (ctx->panicMode) SyncRecovery(ctx, LPAREN);
			else AddChildASTNode(ctx->arena, lambdaNode, typeNode);
			break;
		default: return ParseERROR(ctx, "Expected lambda to have a valid return type.");
	}

	if (!Match(ctx, LPAREN)) return ParseERROR(ctx, "Expected '(' after lambda declaration.");

	switch (ctx->current.type) {
		QUALIFIER_CASES
		TYPE_CASES
			ASTNode* paramListNode = ParamList(ctx);	
			if (ctx->panicMode) SyncRecovery(ctx, RPAREN);
			else AddChildASTNode(ctx->arena, lambdaNode, paramListNode);
			break;
		default: break; // Maybe have this do something? Empty node?
	}

	if (!Match(ctx, RPAREN)) return ParseERROR(ctx, "Expected ')' after function paramaters.");


	if (ctx->current.type != CAPTURES) return ParseERROR(ctx, "Expected explicit capture list via 'captures' keyword.");
	ASTNode* capturesNode = Captures(ctx);
	if(ctx->panicMode) SyncRecovery(ctx, LBRACE);
	else AddChildASTNode(ctx->arena, lambdaNode, capturesNode);

	if (ctx->current.type != LBRACE) return ParseERROR(ctx, "Expected lambda to have a body.");
	ASTNode* bodyNode = Body(ctx);
	if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
	else AddChildASTNode(ctx->arena, lambdaNode, bodyNode);

	return lambdaNode;
}

ASTNode* Captures(ParserContext* ctx)
{
	Advance(ctx);
	ASTNode* capturesNode = InitalizeASTNode(ctx->arena, CAPTURES_NODE, DUMMY_TOKEN);

    while (true) {
        if (ctx->current.type != IDENT) return ParseERROR(ctx, "Expected variable to capture.");
        AddChildASTNode(ctx->arena, capturesNode, InitalizeASTNode(ctx->arena, IDENT_NODE, ctx->current));
        Advance(ctx);

        if (ctx->current.type == LBRACE) return capturesNode;
        if (!Match(ctx, COMMA)) return ParseERROR(ctx, "Expected ',' or '{' after specified captures.");
    }
}

ASTNode* Body(ParserContext* ctx)
{
	ASTNode* bodyNode = InitalizeASTNode(ctx->arena, BODY_NODE, DUMMY_TOKEN);
	Advance(ctx);

	while (true) {
		switch(ctx->current.type) {
			case IF: 
				ASTNode* ifNode = IfStmt(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, ifNode);
				break;
			case SWITCH: 
				ASTNode* switchNode = SwitchStmt(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, switchNode);
				break;
			case WHILE: 
				ASTNode* whileNode = WhileStmt(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, whileNode);
				break;
			case DO: 
				ASTNode* doNode = DoWhileStmt(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, doNode);
				break;
			case FOR: 
				ASTNode* forNode = ForStmt(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, forNode);
				break;
			case LET: 
				ASTNode* varDeclNode = VarDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, SEMI);
				else AddChildASTNode(ctx->arena, bodyNode, varDeclNode);

				if (!Match(ctx, SEMI)) return ParseERROR(ctx, "Expected semicolon ';' after variable declaration.");
				break;
			case ENUM:
				ASTNode* enumNode = EnumDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, enumNode);

				if (!Match(ctx, SEMI)) return ParseERROR(ctx, "Expected semicolon ';' after enum declaration.");
				break;
			case TYPEDEF: 
				ASTNode* typedefNode = TypedefDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, typedefNode);

				if (!Match(ctx, SEMI)) return ParseERROR(ctx, "Expected semicolon ';' after typedef declaration.");
				break;
			case STRUCT:
				ASTNode* structNode = StructDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, structNode);
				break;

			// case SEMI: 
				// Empty Expr Stmt
			EXPR_START_CASES
				ASTNode* exprNode = Expr(ctx, PREC_NONE);
				if (ctx->panicMode) SyncRecovery(ctx, SEMI);
				else AddChildASTNode(ctx->arena, bodyNode, exprNode);

				if (!Match(ctx, SEMI)) return ParseERROR(ctx, "Expected semicolon ';' after expression.");
				break;
			case RETURN:
				ASTNode* returnNode = ReturnStmt(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, SEMI);
				else AddChildASTNode(ctx->arena, bodyNode, returnNode);

				if (!Match(ctx, SEMI)) return ParseERROR(ctx, "Expected semicolon ';' after return expression.");
				break;
			case BREAK: case CONTINUE:

			case LOCK: case CRITICAL:

			/* 
				TODO: I don't like having rbrace check here, gives less clarity on error, return instead
				but then again if I just return, I do an extra check. Worth the debate.
			*/
			case RBRACE: Advance(ctx); return bodyNode;
			default: return ParseERROR(ctx, "Unexpected token in body.");
		}
	}
}

ASTNode* VarDecl(ParserContext* ctx)
{
	ASTNode* varDeclNode = InitalizeASTNode(ctx->arena, VAR_DECL_NODE, DUMMY_TOKEN);
	Advance(ctx);

	if (ctx->current.type == EXTERN) { AddChildASTNode(ctx->arena, varDeclNode, LinkageSpecifier(ctx)); }
	switch(ctx->current.type) { QUALIFIER_CASES AddChildASTNode(ctx->arena, varDeclNode, TypeQualifierList(ctx)); default: break; }

	switch (ctx->current.type) {
		TYPE_CASES
			ASTNode* typeNode = Type(ctx);
			if (ctx->panicMode) SyncRecovery(ctx, IDENT);
			else AddChildASTNode(ctx->arena, varDeclNode, typeNode);
			break;
		default: return ParseERROR(ctx, "Expected Type for variable declaration.");
	}

	ASTNode* varListNode = VarList(ctx);
	if (ctx->panicMode) SyncRecovery(ctx, SEMI);
	else AddChildASTNode(ctx->arena, varDeclNode, varListNode);

	return varDeclNode;
}

ASTNode* GenDecl(ParserContext* ctx)
{
	return NULL;
}

ASTNode* StructDecl(ParserContext* ctx)
{
	// This function is a little unorthodox compared to the others especially 
	// the different handlign of the cases, maybe take a look at later 
	Advance(ctx);

	if (ctx->current.type != IDENT) return ParseERROR(ctx, "Expected struct identifier.");
	ASTNode* structNode = InitalizeASTNode(ctx->arena, STRUCT_DECL_NODE, ctx->current);
	Advance(ctx);

	switch (ctx->current.type) {
		case LESS: 
			// Gen Struct
			ASTNode* genericListNode = GenericList(ctx);
			if (ctx->panicMode) SyncRecovery(ctx, LBRACE);
			else AddChildASTNode(ctx->arena, structNode, genericListNode);

			ASTNode* genBodyNode = GenStructBody(ctx);
			if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
			else AddChildASTNode(ctx->arena, structNode, genBodyNode);
			break;
		case COLON: 
			ASTNode* implementsNode = Implements(ctx);	
			if (ctx->panicMode) SyncRecovery(ctx, LBRACE);
			else AddChildASTNode(ctx->arena, structNode, implementsNode);

			// Can't just fall through, need to check if it has LBRACE, then fall through
			if (ctx->current.type != LBRACE) return ParseERROR(ctx, "Expected '{' struct body after implenting interface.");
		case LBRACE: 
			ASTNode* structBodyNode = StructBody(ctx); 
			if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
			else AddChildASTNode(ctx->arena, structNode, structBodyNode);

			if (!Match(ctx, RBRACE)) return ParseERROR(ctx, "Expected '}' after struct body.");
			break;
		default: return ParseERROR(ctx, "Invalid token after struct identifier.");
	}

	return structNode;
}

ASTNode* GenericStruct(ParserContext* ctx)
{
	return NULL;
}

ASTNode* GenStructBody(ParserContext* ctx)
{
	return NULL;
}

ASTNode* RegularStruct(ParserContext* ctx)
{
	return NULL;
}

ASTNode* StructBody(ParserContext* ctx)
{
	Advance(ctx);
	ASTNode* structBodyNode = InitalizeASTNode(ctx->arena, STRUCT_BODY_NODE, DUMMY_TOKEN);
	while (true) {
		switch (ctx->current.type) {
			case OPERATOR:
				ASTNode* overloadOpNode = OperatorOverload(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, structBodyNode, overloadOpNode);
				break;
			case FUNCTION:
				ASTNode* funcNode = Function(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, SEMI);
				else AddChildASTNode(ctx->arena, structBodyNode, funcNode);
				break;
			case LET: 
				ASTNode* varDeclNode = VarDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, SEMI);
				else AddChildASTNode(ctx->arena, structBodyNode, varDeclNode);

				if (!Match(ctx, SEMI)) return ParseERROR(ctx, "Expected semicolon ';' after variable declaration inside of struct.");
				break;
			case ENUM:
				ASTNode* enumNode = EnumDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, structBodyNode, enumNode);

				if (!Match(ctx, SEMI)) return ParseERROR(ctx, "Expected semicolon ';' after enum declaration inside of struct.");
				break;
			case TYPEDEF: 
				ASTNode* typedefNode = TypedefDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, structBodyNode, typedefNode);

				if (!Match(ctx, SEMI)) return ParseERROR(ctx, "Expected semicolon ';' after typedef declaration inside of struct.");
				break;
			case STRUCT:
				ASTNode* structNode = StructDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, structBodyNode, structNode);
				break;
			case RBRACE: return structBodyNode;
			default: return ParseERROR(ctx, "Unexpected struct member.");
		}

	}
}

ASTNode* OperatorOverload(ParserContext* ctx)
{
	Advance(ctx);

	switch (ctx->current.type) {
		// Overloadable operators
		case PLUS: case MINUS: case MULT: case DIV: case MOD:
		case DOTPROD: case EQQ: case NEQQ: case LESS: case GREAT:
		case LEQQ: case GEQQ: case LSHIFT: case RSHIFT: case AND: 
		case OR: case XOR: case NEG:
			break;
		case LBRACK:
			if (!Match(ctx, RBRACK)) return ParseERROR(ctx, "Expected matching ']' for operator overloaded index.");
			break;	
		default: return ParseERROR(ctx, "Invalid operator for overloading.");
	}

	// Only stores ] for indexing, but thats fine. Unambigous.
	ASTNode* overloadOpNode = InitalizeASTNode(ctx->arena, OPERATOR_OVERLOAD_NODE, ctx->current);
	Advance(ctx);

	if (!Match(ctx, LPAREN)) return ParseERROR(ctx, "Expected '(' after overloaded operator.");

	// Only allow up 1 or 2 paramaters ( unary or binary )
	switch (ctx->current.type) {
		case IDENT:
			ASTNode* paramNode = Param(ctx);
			if (ctx->panicMode) SyncRecovery(ctx, RPAREN);
			else AddChildASTNode(ctx->arena, overloadOpNode, paramNode);
			break;
		default: return ParseERROR(ctx, "Operator overloading doesn't allow primitve types.");
	}

	if (Match(ctx, COMMA)) {
		if (ctx->current.type != IDENT) return ParseERROR(ctx, "Operator overloading doesn't allow primitive types.");

		ASTNode* paramNode = Param(ctx);
		if (ctx->panicMode) SyncRecovery(ctx, RPAREN);
		else AddChildASTNode(ctx->arena, overloadOpNode, paramNode);
	}

	if (!Match(ctx, RPAREN)) return ParseERROR(ctx, "Expected ')' after overloaded operator paramter.");

	if (ctx->current.type != LBRACE) return ParseERROR(ctx, "Overloaded operator requires body in order specify behavior.");
	ASTNode* bodyNode = Body(ctx);
	if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
	else AddChildASTNode(ctx->arena, overloadOpNode, bodyNode);

	return overloadOpNode;
}

// Overloadable op 
ASTNode* InterfaceDecl(ParserContext* ctx)
{
	Advance(ctx);

	if (ctx->current.type != IDENT) return ParseERROR(ctx, "Expected interface name.");
	ASTNode* interfaceDeclNode = InitalizeASTNode(ctx->arena, INTERFACE_DECL_NODE, ctx->current);
	Advance(ctx);

	if (!Match(ctx, LBRACE)) ParseERROR(ctx, "Expected '{' to begin interface declaration.");

	ASTNode* interfaceBodyNode = InterfaceBody(ctx);
	if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
	else AddChildASTNode(ctx->arena, interfaceDeclNode, interfaceBodyNode);

	if (!Match(ctx, RBRACE)) ParseERROR(ctx, "Expected '}' to end interface declaration.");

	return interfaceDeclNode;
}

ASTNode* InterfaceBody(ParserContext* ctx)
{
	ASTNode* interfaceBodyNode = InitalizeASTNode(ctx->arena, INTERFACE_BODY_NODE, DUMMY_TOKEN);

	while (true) {
		switch (ctx->current.type) {
			case LET: 
				ASTNode* varDeclNode = VarDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, SEMI);
				else AddChildASTNode(ctx->arena, interfaceBodyNode, varDeclNode); 

				if (!Match(ctx, SEMI)) return ParseERROR(ctx, "Expect semicolon ';' after variable declartion.");
				break;
			case FUNCTION:
				ASTNode* funcNode = Function(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, SEMI);
				else AddChildASTNode(ctx->arena, interfaceBodyNode, funcNode); 

				if (!Match(ctx, SEMI)) return ParseERROR(ctx, "Expect semicolon ';' after function declartion.");
				break;
			case RBRACE:
				return interfaceBodyNode;
			default:
				return ParseERROR(ctx, "Only FUNCTIONS or VARAIBLES allowed inside of interface's body.");
		}
	}
}

ASTNode* Implements(ParserContext* ctx)
{
    Advance(ctx);
    ASTNode* implementsNode = InitalizeASTNode(ctx->arena, IMPLEMENTS_NODE, DUMMY_TOKEN);

    while (true) {
        if (ctx->current.type != IDENT) return ParseERROR(ctx, "Expected identifier for interface implementation.");
        
        AddChildASTNode(ctx->arena, implementsNode, InitalizeASTNode(ctx->arena, IDENT_NODE, ctx->current));
        Advance(ctx);

        if (ctx->current.type == LBRACE) return implementsNode;
        if (!Match(ctx, COMMA)) return ParseERROR(ctx, "Expected ',' or '{' after interface implementation.");
    }
}

ASTNode* EnumDecl(ParserContext* ctx)
{
	Advance(ctx);

	if (ctx->current.type != IDENT) ParseERROR(ctx, "Expected Identifier for enum.");
	ASTNode* enumNode = InitalizeASTNode(ctx->arena, ENUM_DECL_NODE, ctx->current);
	Advance(ctx);

	if (!Match(ctx, LBRACE)) return ParseERROR(ctx,  "Expected '{' to begin Enum body.");

	ASTNode* enumBodyNode = InitalizeASTNode(ctx->arena, ENUM_BODY_NODE, DUMMY_TOKEN);
	while (true) {
		if (ctx->current.type == IDENT) {
			AddChildASTNode(ctx->arena, enumBodyNode, InitalizeASTNode(ctx->arena, IDENT_NODE, ctx->current));
			Advance(ctx); 

			if (Match(ctx, COMMA))  continue;
		}

		else if (ctx->current.type == RBRACE) break;
		else return ParseERROR(ctx, "Expected Identifier in enum.");
	}

	printf("%s\n",ctx->current.lexeme);
	if (!Match(ctx, RBRACE)) return ParseERROR(ctx, "Expected '}' to end Enum body.");

	AddChildASTNode(ctx->arena, enumNode, enumBodyNode);
	return enumNode;
}

ASTNode* TypedefDecl(ParserContext* ctx)
{
	return NULL;
}

ASTNode* TypeSpec(ParserContext* ctx)
{
	return NULL;
}

ASTNode* TypedefPostfix(ParserContext* ctx)
{
	return NULL;
}


ASTNode* ConcurrencyStmt(ParserContext* ctx)
{
	return NULL;
}

ASTNode* LockStmt(ParserContext* ctx)
{
	return NULL;
}

ASTNode* CriticalStmt(ParserContext* ctx)
{
	return NULL;
}


ASTNode* IfStmt(ParserContext* ctx)
{
	ASTNode* ifStmtNode = InitalizeASTNode(ctx->arena, IF_STMT_NODE, DUMMY_TOKEN);
	Advance(ctx);

	ASTNode* ifNode = InitalizeASTNode(ctx->arena, IF_NODE, DUMMY_TOKEN);
	if (!Match(ctx, LPAREN)) return ParseERROR(ctx, "Expected '(' to begin If Statment's condition.");

	switch (ctx->current.type) {
		EXPR_START_CASES
			ASTNode* conditionalNode = Expr(ctx, PREC_NONE);
			if (ctx->panicMode) SyncRecovery(ctx, RPAREN);
			else AddChildASTNode(ctx->arena, ifNode, conditionalNode);
			break;
		case RPAREN: break;
		default: return ParseERROR(ctx, "Expected expression in If Statements condition.");
	}

	if (!Match(ctx, RPAREN)) return ParseERROR(ctx, "Expected ')' to end If Statment's condition.");

	if (ctx->current.type != LBRACE) return ParseERROR(ctx, "Expected '{' to begin If Statements body");
	ASTNode* ifBodyNode = Body(ctx);
	if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
	else AddChildASTNode(ctx->arena, ifNode, ifBodyNode);
	AddChildASTNode(ctx->arena, ifStmtNode, ifNode);
	printf("%s\n", ctx->current.lexeme);

	while (Match(ctx, ELIF)) {
		ASTNode* elifNode = InitalizeASTNode(ctx->arena, ELIF_NODE, DUMMY_TOKEN);
		if (!Match(ctx, LPAREN)) return ParseERROR(ctx, "Expected '(' to begin Else If Statment's condition.");

		switch (ctx->current.type) {
			EXPR_START_CASES
				ASTNode* conditionalNode = Expr(ctx, PREC_NONE);
				if (ctx->panicMode) SyncRecovery(ctx, RPAREN);
				else AddChildASTNode(ctx->arena, elifNode, conditionalNode);
				break;
			case RPAREN: break;
		}

		if (!Match(ctx, RPAREN)) return ParseERROR(ctx, "Expected ')' to end Else If Statment's condition.");
			
		if (ctx->current.type != LBRACE) return ParseERROR(ctx, "Expected '{' to begin Else If Statements body");
		ASTNode* elifBodyNode = Body(ctx);
		if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
		else AddChildASTNode(ctx->arena, elifNode, elifBodyNode);
		AddChildASTNode(ctx->arena, ifStmtNode, elifNode);
	}

	if (Match(ctx, ELSE)) {
		ASTNode* elseNode = InitalizeASTNode(ctx->arena, ELSE_NODE, DUMMY_TOKEN);
			
		if (ctx->current.type != LBRACE) return ParseERROR(ctx, "Expected '{' to begin Else Statements body");
		ASTNode* elifBodyNode = Body(ctx);
		if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
		else AddChildASTNode(ctx->arena, elseNode, elifBodyNode);
		AddChildASTNode(ctx->arena, ifStmtNode, elseNode);
	}

	return ifStmtNode;
}

ASTNode* SwitchStmt(ParserContext* ctx)
{
	return NULL;
}

ASTNode* Case(ParserContext* ctx)
{
	return NULL;
}

ASTNode* Default(ParserContext* ctx)
{
	return NULL;
}

ASTNode* WhileStmt(ParserContext* ctx)
{
	return NULL;
}

ASTNode* DoWhileStmt(ParserContext* ctx)
{
	return NULL;
}

ASTNode* ForStmt(ParserContext* ctx)
{
	Advance(ctx);
	ASTNode* forStmtNode = InitalizeASTNode(ctx->arena, FOR_STMT_NODE, DUMMY_TOKEN);

	if (!Match(ctx, LPAREN)) return ParseERROR(ctx, "Expected '(' to begin for statement.");

	switch (ctx->current.type) {
		case LET:
			ASTNode* initNode = VarDecl(ctx);
			if (ctx->panicMode) SyncRecovery(ctx, SEMI);
			else AddChildASTNode(ctx->arena, forStmtNode, initNode);
			break;
		EXPR_START_CASES
			ASTNode* exprInitNode = ExprList(ctx);
			if (ctx->panicMode) SyncRecovery(ctx, SEMI);
			else AddChildASTNode(ctx->arena, forStmtNode, exprInitNode);
			break;
		case SEMI: break;
		default: return ParseERROR(ctx, "Expected initalizer section of for statement.");
	}

	if (!Match(ctx, SEMI)) return ParseERROR(ctx, "Expected semicolon ';' between initializer and conditional sections of for statement.");

	switch (ctx->current.type) {
		EXPR_START_CASES
			ASTNode* conditionNode = Expr(ctx, PREC_NONE);
			if (ctx->panicMode) SyncRecovery(ctx, SEMI);
			else AddChildASTNode(ctx->arena, forStmtNode, conditionNode);
			break;
		case SEMI: break;
		default: return ParseERROR(ctx, "Expected conditional section of for statement.");
	}

	if (!Match(ctx, SEMI)) return ParseERROR(ctx, "Expected semicolon ';' between conditional and incremental sections of for statement.");

	switch (ctx->current.type) {
		EXPR_START_CASES
			ASTNode* exprListNode = ExprList(ctx);
			if (ctx->panicMode) SyncRecovery(ctx, RPAREN);
			else AddChildASTNode(ctx->arena, forStmtNode, exprListNode);
			break;
		case RPAREN: break;
		default: return ParseERROR(ctx, "Expected incremental section of for statement.");
	}

	if (!Match(ctx, RPAREN)) return ParseERROR(ctx, "Expected ')' to end for statement.");


	if (!Match(ctx, LBRACE)) return ParseERROR(ctx, "Expected '{' to begin for statement's body.");
	ASTNode* bodyNode = Body(ctx);
	if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
	else AddChildASTNode(ctx->arena, forStmtNode, bodyNode);

	return forStmtNode;
}

ASTNode* ReturnStmt(ParserContext* ctx)
{
	Advance(ctx);

	// Typically if there is nothing in a node, we omit the parent node, however, 
	// In this case we still need to type check the lack of a return type of the function
	ASTNode* returnStmtNode = InitalizeASTNode(ctx->arena, RETURN_STMT_NODE, DUMMY_TOKEN);
	switch (ctx->current.type) {
		EXPR_START_CASES	
			ASTNode* exprNode = Expr(ctx, PREC_NONE);
			if (ctx->panicMode) SyncRecovery(ctx, SEMI);
			else AddChildASTNode(ctx->arena, returnStmtNode, exprNode);
			break;
		case SEMI: break;
		default: return ParseERROR(ctx, "Expected a return value.");
	}
	return returnStmtNode;
}

ASTNode* ExprList(ParserContext* ctx)
{
    ASTNode* exprListNode = InitalizeASTNode(ctx->arena, EXPR_LIST_NODE, DUMMY_TOKEN);

    // First always guaranteed by predictive parsing
    ASTNode* exprNode = Expr(ctx, PREC_NONE);
    if (ctx->panicMode) SyncRecovery(ctx, RPAREN);
    else AddChildASTNode(ctx->arena, exprListNode, exprNode);

    // Rest only if comma follows
    while (Match(ctx, COMMA)) {
        exprNode = Expr(ctx, PREC_NONE);
        if (ctx->panicMode) {
            SyncRecovery(ctx, RPAREN);
            return exprListNode;
        }
        AddChildASTNode(ctx->arena, exprListNode, exprNode);
    }

    return exprListNode;
}

// TODO: Distinguish between pre / postfix in the actual ASTNode
ASTNode* Expr(ParserContext* ctx, PRECEDENCE prec)
{
	ASTNode* left = NULL;
	switch (ctx->current.type) {
		// Literals
		case REAL: case INTEGRAL:
		case CLITERAL: case SLITERAL: 
		case HEX: case NILL: case FALSE: 
		case TRUE: 
			left = InitalizeASTNode(ctx->arena, LITERAL_NODE, ctx->current);
			Advance(ctx); break;
		case IDENT:
			left = InitalizeASTNode(ctx->arena, IDENT_NODE, ctx->current);
			Advance(ctx); break;
		case LAMBDA:
			left = Lambda(ctx);
			if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
			break;
		case SIZEOF:
			break;
		case LPAREN:
			Advance(ctx);
			left = Expr(ctx, PREC_NONE);
			if (ctx->panicMode) SyncRecovery(ctx, RPAREN);

			if (!Match(ctx, RPAREN)) return ParseERROR(ctx, "Expected closing ')' for parenthesized expression.");
			break;
		default:
			// Prefix
			ParseRule prefixRule = PRECEDENCE_TABLE[ctx->current.type];
			if (!prefixRule.prefix) return ParseERROR(ctx, "Expected expression.");
            left = prefixRule.prefix(ctx, prec);
	}

	// Infix
	ParseRule rule = PRECEDENCE_TABLE[ctx->current.type];
	while (rule.prec > prec && rule.infix != NULL) {
		left = rule.infix(ctx, rule.rightAssoc ? rule.prec - 1 : rule.prec, left);
		rule = PRECEDENCE_TABLE[ctx->current.type];
	}

	return left;
}

ASTNode* PrefixExpr(ParserContext* ctx, PRECEDENCE prec)
{
	ASTNode* unaryNode = InitalizeASTNode(ctx->arena, PREFIX_EXPR_NODE, ctx->current);
	Advance(ctx);

	ASTNode* exprNode = Expr(ctx, PREC_PRE - 1);
	if (ctx->panicMode) return NULL;
	else AddChildASTNode(ctx->arena, unaryNode, exprNode);

	return unaryNode;
}

ASTNode* PostfixExpr(ParserContext* ctx, PRECEDENCE prec, ASTNode* left) {
    ASTNode* node = InitalizeASTNode(ctx->arena, POSTFIX_EXPR_NODE, ctx->current);
    AddChildASTNode(ctx->arena, node, left);
    Advance(ctx);
    return node;
}

ASTNode* BinaryExpr(ParserContext* ctx, PRECEDENCE prec, ASTNode* left)
{
	ASTNode* binaryNode;
	switch (ctx->current.type) {
		case MEM:
			Advance(ctx);
			if (ctx->current.type != IDENT) return ParseERROR(ctx, "Invalid struct member access.");
			binaryNode = InitalizeASTNode(ctx->arena, MEMBER_NODE, ctx->current);
			AddChildASTNode(ctx->arena, binaryNode, left);
			Advance(ctx);  
			return binaryNode;  
		case SMEM:
			Advance(ctx);
			if (ctx->current.type != IDENT) return ParseERROR(ctx, "Invalid safe struct member access.");
			binaryNode = InitalizeASTNode(ctx->arena, SMEMBER_NODE, ctx->current);
			AddChildASTNode(ctx->arena, binaryNode, left);
			Advance(ctx);  
			return binaryNode;  
		break;
		case REF:
			Advance(ctx);
			if (ctx->current.type != IDENT) return ParseERROR(ctx, "Invalid struct member pointer access.");
			binaryNode = InitalizeASTNode(ctx->arena, REF, ctx->current);
			AddChildASTNode(ctx->arena, binaryNode, left);
			Advance(ctx);  
			return binaryNode;  
		case SREF:
			Advance(ctx);
			if (ctx->current.type != IDENT) return ParseERROR(ctx, "Invalid safe struct member pointer access.");
			binaryNode = InitalizeASTNode(ctx->arena, SREF, ctx->current);
			AddChildASTNode(ctx->arena, binaryNode, left);
			Advance(ctx);  
			return binaryNode;  
		case AS:
			Advance(ctx);
			switch (ctx->current.type) {
				TYPE_CASES
					break;
				default: return ParseERROR(ctx, "Invalid type for casting.");
			}
			binaryNode = InitalizeASTNode(ctx->arena, CAST_NODE, ctx->current);
			AddChildASTNode(ctx->arena, binaryNode, left);
			Advance(ctx);  
			return binaryNode;  
		case LBRACK: // Index
			break;
		default: binaryNode = InitalizeASTNode(ctx->arena, BINARY_EXPR_NODE, ctx->current);

	}
	Advance(ctx);	

	AddChildASTNode(ctx->arena, binaryNode, left);
	ASTNode* right = Expr(ctx, prec);
	if (ctx->panicMode) return NULL;	// Main expr handles errors
	else AddChildASTNode(ctx->arena, binaryNode, right);

	return binaryNode;
}

ASTNode* AsgnExpr(ParserContext* ctx, PRECEDENCE prec, ASTNode* left)
{
	ASTNode* asgnNode = InitalizeASTNode(ctx->arena, ASGN_EXPR_NODE, ctx->current);
	Advance(ctx);	

	AddChildASTNode(ctx->arena, asgnNode, left);
	ASTNode* right = Expr(ctx, prec);
	if (ctx->panicMode) return NULL;	
	else AddChildASTNode(ctx->arena, asgnNode, right);

	return asgnNode;
}

ASTNode* TernaryExpr(ParserContext* ctx, PRECEDENCE prec, ASTNode* left)
{
    ASTNode* ternaryNode = InitalizeASTNode(ctx->arena, TERNARY_EXPR_NODE, ctx->current);
    Advance(ctx); 
    AddChildASTNode(ctx->arena, ternaryNode, left); 

    ASTNode* then = Expr(ctx, PREC_NONE);
    if (ctx->panicMode) return NULL;
    else AddChildASTNode(ctx->arena, ternaryNode, then);

    if (!Match(ctx, COLON)) return ParseERROR(ctx, "Expected ':' in ternary expression.");

    ASTNode* els = Expr(ctx, prec); 
    if (ctx->panicMode) return NULL;
    else AddChildASTNode(ctx->arena, ternaryNode, els);

    return ternaryNode;
}

ASTNode* Type(ParserContext* ctx)
{
	// Function pointers and closures are special types 
	if (ctx->current.type == FUNCPTR || ctx->current.type == CLOSURE) {
        return FuncPointerType(ctx);
    }

	ASTNode* typeNode = InitalizeASTNode(ctx->arena, TYPE_NODE, ctx->current);
	Advance(ctx);

	if (ctx->current.type == MOD || ctx->current.type == MULT) {
		AddChildASTNode(ctx->arena, typeNode, InitalizeASTNode(ctx->arena, DECL_PREFIX_NODE, ctx->current));
		Advance(ctx);
	}

	return typeNode;
}

ASTNode* Channel(ParserContext* ctx)
{
	return NULL;
}

ASTNode* Matrix(ParserContext* ctx)
{
	return NULL;
}

ASTNode* Vector(ParserContext* ctx)
{
	return NULL;
}

ASTNode* FuncPointerType(ParserContext* ctx)
{
    ASTNode* fpNode = InitalizeASTNode(ctx->arena, ctx->current.type == FUNCPTR ? FUNC_POINTER_NODE : CLOSURE_NODE, ctx->current);
    Advance(ctx);

	/* TODO: ALLOW FUNC POINTER QUALIFIERS */

	switch (ctx->current.type) { 
		TYPE_CASES 
			ASTNode* returnTypeNode = Type(ctx);
			if (ctx->panicMode) SyncRecovery(ctx, LPAREN);
			else AddChildASTNode(ctx->arena, fpNode, returnTypeNode);
			break;
		default: return ParseERROR(ctx, "Expected function pointer to return a valid type.");
	}

	if (!Match(ctx, LPAREN)) return ParseERROR(ctx, "Expected function pointer / closure to have '(' to begin paramaters.") ;
	if (Match(ctx, RPAREN)) return fpNode;

	while (true) {
		switch (ctx->current.type) {
			TYPE_CASES
				ASTNode* anonParam = Type(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RPAREN);
				else AddChildASTNode(ctx->arena, fpNode, anonParam);
				break;
			default: return ParseERROR(ctx, "Expected anonymous paramater inside of function pointer.")	;
		}

		if (Match(ctx, COMMA)) continue;
		else if (Match(ctx, RPAREN)) return fpNode; 
		else return ParseERROR(ctx, "Expected anonymous paramaters in function pointer paramaters.");
	}
}

ASTNode* DeclPrefix(ParserContext* ctx)
{
	return NULL;
}

ASTNode* GenericList(ParserContext* ctx)
{
	Advance(ctx);
	ASTNode* genericListNode = InitalizeASTNode(ctx->arena, GENERIC_LIST_NODE, DUMMY_TOKEN);

	while (true) {
		// At least one required
		if (ctx->current.type != IDENT) return ParseERROR(ctx, "Expected valid identifier for generic ie.) <IDENTIFIER>.");
		AddChildASTNode(ctx->arena, genericListNode, (InitalizeASTNode(ctx->arena, GENERIC_NODE, ctx->current)));
		Advance(ctx);


		if (Match(ctx, COMMA)) continue;
		else if (Match(ctx, GREAT)) break; 
		else return ParseERROR(ctx, "Expected '>' to close generic.");
	}

	return genericListNode;
}

ASTNode* Generic(ParserContext* ctx)
{
	Advance(ctx);

	if (ctx->current.type != IDENT) return ParseERROR(ctx, "Expected valid identifier for generic ie.) <IDENTIFIER>.");
	ASTNode* genNode = InitalizeASTNode(ctx->arena, GENERIC_NODE, ctx->current);
	Advance(ctx);

	if (!Match(ctx, GREAT)) return ParseERROR(ctx, "Expected '>' to close generic.");
	return genNode;
}

ASTNode* TypeQualifierList(ParserContext* ctx)
{
	ASTNode* qualifierListNode = InitalizeASTNode(ctx->arena, QUALIFIER_LIST_NODE, DUMMY_TOKEN);
	while (true) {
		switch (ctx->current.type) {
			QUALIFIER_CASES
				ASTNode* qualifierNode = InitalizeASTNode(ctx->arena, TYPE_QUALIFIER_NODE, ctx->current);
				AddChildASTNode(ctx->arena, qualifierListNode, qualifierNode);
				break;
			default: return qualifierListNode;
		}
		Advance(ctx);
	}
}

ASTNode* TypeQualifier(ParserContext* ctx)
{
	return NULL;
}

ASTNode* LinkageSpecifier(ParserContext* ctx)
{
	return NULL;
}


ASTNode* Sizeof(ParserContext* ctx)
{
	return NULL;
}


ASTNode* Reg(ParserContext* ctx)
{
	return NULL;
}

ASTNode* Hex(ParserContext* ctx)
{
	return NULL;
}

ASTNode* PredefVars(ParserContext* ctx)
{
	return NULL;
}


ASTNode* ArgList(ParserContext* ctx)
{
	return NULL;
}


ASTNode* VarList(ParserContext* ctx)
{
	ASTNode* varListNode = InitalizeASTNode(ctx->arena, VAR_LIST_NODE, DUMMY_TOKEN);

	while (true) {
		ASTNode* varNode = Var(ctx);
		if (ctx->panicMode) { SyncRecovery(ctx, SEMI); break; }
		AddChildASTNode(ctx->arena, varListNode, varNode);

		if (!Match(ctx, COMMA)) break;
	}
	return varListNode;
}

ASTNode* Var(ParserContext* ctx)
{
	if (ctx->current.type != IDENT) ParseERROR(ctx, "Expected identifier name for variable declaration.");
	ASTNode* varNode = InitalizeASTNode(ctx->arena, VAR_NODE, ctx->current);
    Advance(ctx);

	while (Match(ctx, LBRACK)) {
		ASTNode* exprNode = Expr(ctx, PREC_NONE);
		if (ctx->panicMode) { SyncRecovery(ctx, RBRACK); return varNode; }
		if (!Match(ctx, RBRACK)) return ParseERROR(ctx, "Epexted ']' for array initalization.");

		// TODO: Idk if this is correct
		ASTNode* arrayInitNode = InitalizeASTNode(ctx->arena, ARR_DECL_NODE, DUMMY_TOKEN);
		AddChildASTNode(ctx->arena, arrayInitNode, exprNode);
		AddChildASTNode(ctx->arena, varNode, arrayInitNode);
	}

	if (Match(ctx, EQ)) {
		switch (ctx->current.type) {
			case LBRACE: // ARR init list 
				ASTNode* arrListNode = ArrInitList(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, varNode, arrListNode);
				break;
			EXPR_START_CASES // Expr k
				ASTNode* exprNode = Expr(ctx, PREC_NONE);
				if (ctx->panicMode) SyncRecovery(ctx, SEMI);
				else AddChildASTNode(ctx->arena, varNode, exprNode);
				break;
			default: return ParseERROR(ctx, "Invalid assignment to variable.");
		}	
	}

	return varNode;
}


ASTNode* ArrDecl(ParserContext* ctx)
{
	return NULL;
}

ASTNode* ArrInitList(ParserContext* ctx)
{
	return NULL;
}

ASTNode* Literal(ParserContext* ctx)
{
	return NULL;
}
