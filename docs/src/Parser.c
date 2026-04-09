#include "Parser.h"

#define TYPE_CASES \
    case CHAR: case BOOL: case INT: case LONG: case DOUBLE: case FLOAT: \
    case VOID: case STRING: case I8: case I16: case I32: case I64: \
    case U8: case U16: case U32: case U64: case MAT: case VEC: \
    case MUTEX: case SEMAPHORE: case TASK: case CHANNEL:

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

static inline ASTNode* ParseERROR(ParserContext* ctx, char* format) {
    ctx->failure = true; ctx->panicMode = true;
    char buf[256];
    snprintf(buf, sizeof(buf), "%s Instead encountered '%s' on line %d, col %d\n",
             format, ctx->current.lexeme, ctx->current.row, ctx->current.col);
    ERROR(ERR_FLAG_CONTINUE, PARSER_ERR, "%s", buf);
    return NULL;
}

static inline void SyncRecovery(ParserContext* ctx, TokenType tt) 
{
    while (ctx->current.type != tt && ctx->current.type != END) 
        Advance(ctx);
	ctx->panicMode = false;
}

/* 
Could implement a predictive parsing table (similar to dfa table)
*/

/* 
Error Recovery - want to recover from error, so other syntax
errors can be found.

Can delete, replace or insert tokens to recover. Can pretend that
a valid token was there, print a messsage nad return normally.

Insertion is dangerous since it could cascade and loop infinite.
Deletion is safer, since EOF will cause loop to terminate.

Delection - skip tokens until token in follow set is reached 
*/


/* 
LL(k) must predict which production to use. Better to us LR(k)
R means rightmost derivation. Parser has a stack nad an input, the first
k tokens of the input are the lookahead. Based on the contents of the stack and the lookahead
the parser performs two kinds of actions

Shift: move the first input token to the top of the stack
Reduce: Choose a grammar rule X -> A B C; pop C, B, A from the top of the stack,
        push X onto the stack

This is what I used in the past
*/

/* 
LLR konw when to shift / reduce by using a DFA, applied to the stack
probably overkill for my grammar since only 2 cases of LL3
*/


/*
typedef struct {
    TokenizerContext* tokenizer;
    Token currenet;
    Token buffer[3];
    TypeTable* typeTable;
} ParserContext;
*/


/* 
Shouldn't pass the check down to the next function
should check the first set inside of the function, to avoid
unnecesary descent. Check first set before entering.
This also allows for better error messaging since nesting
wont propagte up. 
*/


/* 
Optimizations: 

Pratt parsing YES
Packrat parsing PROBABLY NOT 
Arena Allocator to reduce malloc YES


*/

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
    //printf("Program\n");
   
    while (true) {
		Advance(ctx);
        switch (ctx->current.type) {
            case FUNCTION: 
                //printf("Func\n");
                ASTNode* funcNode = Function(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
                AddChildASTNode(ctx->arena, ctx->ast->root, funcNode);
                break;
            case INTERFACE:
				//printf("Interface\n");
                ASTNode* interfaceDeclNode = InterfaceDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
                AddChildASTNode(ctx->arena, ctx->ast->root, interfaceDeclNode);
                break;
			case LET:
                //printf("VarDecl\n");
                ASTNode* varDeclNode = VarDecl(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, SEMI);
                AddChildASTNode(ctx->arena, ctx->ast->root, varDeclNode);
                break;
            case END:   
                //printf("End\n");
                return;
			default: 
				ERROR(ERR_FLAG_EXIT, PARSER_ERR, "Unexpected token occured in glboal scope: '%s' on line %d, col %d", ctx->current, ctx->current.row, ctx->current.col);
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
		case IDENT: funcNode = RegularFunc(ctx); break;
		case GEN:   funcNode = GenericFunc(ctx); break;
		default: return ParseERROR(ctx, "Expected function return type or generic return type.");
	}
	if (ctx->panicMode) return NULL;

	printf("Child Count: %d\n", funcNode->childCount);
	if (linkageNode) PrependChildASTNode(ctx->arena, funcNode, linkageNode);
	if (qualifierNode) PrependChildASTNode(ctx->arena, funcNode, qualifierNode);
	printf("Child Count: %d\n", funcNode->childCount);

	switch (ctx->current.type) {
		case SEMI: 
			//printf("Decl\n");
			funcNode->type = FUNC_DECL;
			break;
		case LBRACE:
			//printf("Def\n");
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
	return NULL;
}

ASTNode* RegularFunc(ParserContext* ctx)
{
	ASTNode* typeNode = InitalizeASTNode(ctx->arena, TYPE_NODE, ctx->current);
	Advance(ctx);

	// Optional DeclPrefix
	if (ctx->current.type == MOD || ctx->current.type == MULT) {
		AddChildASTNode(ctx->arena, typeNode, InitalizeASTNode(ctx->arena, DECL_PREFIX_NODE, ctx->current));
		Advance(ctx);
	}

	if (ctx->current.type != IDENT) return ParseERROR(ctx, "Function name expected.");
	ASTNode* funcNode = InitalizeASTNode(ctx->arena, REGULAR_FUNC_NODE, ctx->current);
	Advance(ctx);

	AddChildASTNode(ctx->arena, funcNode, typeNode);

	if (ctx->current.type != LPAREN) return ParseERROR(ctx, "Expected '(' after function name.");
	Advance(ctx);

	switch (ctx->current.type) {
		QUALIFIER_CASES
		TYPE_CASES
		case IDENT: 
			ASTNode* paramListNode = ParamList(ctx);
			AddChildASTNode(ctx->arena, funcNode, paramListNode);
			Advance(ctx);
			break;
		default: break; // Maybe have this do something? Empty node?
	}

	if (ctx->current.type != RPAREN) return ParseERROR(ctx, "Epexted ') after funtion name.");
	Advance(ctx);

	return funcNode;
}


ASTNode* ParamList(ParserContext* ctx)
{
	ASTNode* paramListNode = InitalizeASTNode(ctx->arena, PARAM_LIST_NODE, DUMMY_TOKEN);

	while (true) {
		ASTNode* qualifierNode = NULL;
		switch (ctx->current.type) { QUALIFIER_CASES qualifierNode = TypeQualifier(ctx); default: break; }

		switch (ctx->current.type) {
			TYPE_CASES
			case IDENT:
				ASTNode* paramNode = Param(ctx);
				if (qualifierNode) PrependChildASTNode(ctx->arena, paramNode, qualifierNode);

				// TODO: Ideal to go to comma THEN rparen, but for later
				if (ctx->panicMode) SyncRecovery(ctx, RPAREN);
				else AddChildASTNode(ctx->arena, paramListNode, paramNode);
				break;
			case GEN:  
				ASTNode* genParamNode = GenParam(ctx);
				if (qualifierNode) PrependChildASTNode(ctx->arena, genParamNode, qualifierNode);

				if (ctx->panicMode) SyncRecovery(ctx, RPAREN);
				else AddChildASTNode(ctx->arena, paramListNode, genParamNode);
				break;
			case RPAREN: return paramListNode;
			default: return ParseERROR(ctx, "Expected paramters OR ')' inside function signature.");
		}
	}
}

ASTNode* Param(ParserContext* ctx)
{
    ASTNode* paramNode = InitalizeASTNode(ctx->arena, PARAM_NODE, DUMMY_TOKEN);

	AddChildASTNode(ctx->arena, paramNode, InitalizeASTNode(ctx->arena, TYPE_NODE, ctx->current));
    Advance(ctx);

    if (ctx->current.type == MOD || ctx->current.type == MULT) {
        ASTNode* declPrefixNode = InitalizeASTNode(ctx->arena, DECL_PREFIX_NODE, ctx->current);
        Advance(ctx);
        AddChildASTNode(ctx->arena, paramNode, declPrefixNode);
    }

    if (ctx->current.type != IDENT) return ParseERROR(ctx, "Expected parameter identifier.");
	AddChildASTNode(ctx->arena, paramNode, InitalizeASTNode(ctx->arena, IDENT_NODE, ctx->current));
    Advance(ctx);

    return paramNode;
}

ASTNode* GenParam(ParserContext* ctx)
{
	return NULL;
}


ASTNode* Lamba(ParserContext* ctx)
{
	return NULL;
}

ASTNode* Body(ParserContext* ctx)
{
	//printf("BODY\n");
	ASTNode* bodyNode = InitalizeASTNode(ctx->arena, BODY_NODE, DUMMY_TOKEN);
	Advance(ctx);


	/* TODO: groupings that follow the grammar, simplifies and makes recursive descent easier */
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
				ASTNode* doNode = DoWhlieStmt(ctx);
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

				// Don't do anything here use pratt parsing

			case RETURN:

			case BREAK: case CONTINUE:

			case LOCK: case CRITICAL:

			case RBRACE: return bodyNode;
			default: return ParseERROR(ctx, "Unexpected token in body.");
		}
	}
}

ASTNode* StmtList(ParserContext* ctx)
{
	return NULL;
}

ASTNode* Stmt(ParserContext* ctx)
{
	return NULL;
}


ASTNode* ExprStmt(ParserContext* ctx)
{
	return NULL;
}

ASTNode* DeclStmt(ParserContext* ctx)
{
	return NULL;
}


ASTNode* VarDecl(ParserContext* ctx)
{
	ASTNode* varDeclNode = InitalizeASTNode(ctx->arena, VAR_DECL_NODE, DUMMY_TOKEN);
	Advance(ctx);

	if (ctx->current.type == EXTERN) { AddChildASTNode(ctx->arena, varDeclNode, LinkageSpecifier(ctx)); }
	switch(ctx->current.type) { QUALIFIER_CASES AddChildASTNode(ctx->arena, varDeclNode, TypeQualifierList(ctx)); default: break; }

	switch (ctx->current.type) {
		TYPE_CASES
			AddChildASTNode(ctx->arena, varDeclNode, InitalizeASTNode(ctx->arena, TYPE_NODE, ctx->current));
			Advance(ctx);
			break;
		case IDENT:
			AddChildASTNode(ctx->arena, varDeclNode, InitalizeASTNode(ctx->arena, IDENT_NODE, ctx->current));
			Advance(ctx);
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
	return NULL;
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
	return NULL;
}

ASTNode* OperatorOverload(ParserContext* ctx)
{
	return NULL;
}

// Overloadable op 
ASTNode* InterfaceDecl(ParserContext* ctx)
{
	ASTNode* interfaceDeclNode = InitalizeASTNode(ctx->arena, INTERFACE_DECL_NODE, DUMMY_TOKEN);
	Advance(ctx);

	if (ctx->current.type != IDENT) return ParseERROR(ctx, "Expected interface name.");
	AddChildASTNode(ctx->arena, interfaceDeclNode, InitalizeASTNode(ctx->arena, IDENT_NODE, ctx->current));
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
				break;
			case FUNCTION:
				ASTNode* funcNode = Function(ctx);
				if (ctx->panicMode) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, interfaceBodyNode, funcNode); 
				break;
			case RBRACE:
				return interfaceBodyNode;
			default:
				return ParseERROR(ctx, "Expected Variable or Function declaration inside of interface.");
		}
	}
}


ASTNode* EnumDecl(ParserContext* ctx)
{
	ASTNode* enumNode = InitalizeASTNode(ctx->arena, ENUM_DECL_NODE, DUMMY_TOKEN);
	Advance(ctx);

	if (ctx->current.type != IDENT) ParseERROR(ctx, "Expected Identifier for enum.");
	AddChildASTNode(ctx->arena, enumNode, InitalizeASTNode(ctx->arena, IDENT_NODE, ctx->current));

	// Probably an easier way to do this loop 
	ASTNode* enumBodyNode = InitalizeASTNode(ctx->arena, ENUM_BODY_NODE, ctx->current);
	while (true) {
		Advance(ctx);
		if (ctx->current.type == IDENT) {
			AddChildASTNode(ctx->arena, enumBodyNode, InitalizeASTNode(ctx->arena, IDENT_NODE, ctx->current));
			if (Match(ctx, COMMA))  continue;
			else if (Match(ctx, RBRACE)) break;
			else return ParseERROR(ctx, "Expected Identifier in enum.");
		} 
		else if (Match(ctx, RBRACE)) break;
		return ParseERROR(ctx, "Expected Identifier in enum.");
	}

	return NULL;
}

ASTNode* EnumBody(ParserContext* ctx)
{
	return NULL;
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
	return NULL;
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

ASTNode* DoWhlieStmt(ParserContext* ctx)
{
	return NULL;
}

ASTNode* ForStmt(ParserContext* ctx)
{
	return NULL;
}

ASTNode* ExprList(ParserContext* ctx)
{
	return NULL;
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
			Advance(ctx);
			break;
		case LAMBDA:
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

ASTNode* UnaryExpr(ParserContext* ctx, PRECEDENCE prec)
{
	//printf("UNARY\n");
	ASTNode* unaryNode = InitalizeASTNode(ctx->arena, UNARY_EXPR_NODE, ctx->current);
	Advance(ctx);

	ASTNode* exprNode = Expr(ctx, PREC_PRE - 1);
	if (ctx->panicMode) return NULL;
	else AddChildASTNode(ctx->arena, unaryNode, exprNode);

	return unaryNode;
}

ASTNode* BinaryExpr(ParserContext* ctx, PRECEDENCE prec, ASTNode* left)
{
	//printf("BINARY\n");
	ASTNode* binaryNode = InitalizeASTNode(ctx->arena, BINARY_EXPR_NODE, ctx->current);
	Advance(ctx);	

	AddChildASTNode(ctx->arena, binaryNode, left);
	ASTNode* right = Expr(ctx, prec);
	if (ctx->panicMode) return NULL;	// Main expr handles errors
	else AddChildASTNode(ctx->arena, binaryNode, right);

	return binaryNode;
}

ASTNode* AsgnExpr(ParserContext* ctx, PRECEDENCE prec, ASTNode* left)
{
	//printf("ASGN\n");
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

ASTNode* DeclPrefix(ParserContext* ctx)
{
	return NULL;
}

ASTNode* GenericList(ParserContext* ctx)
{
	return NULL;
}

ASTNode* Generic(ParserContext* ctx)
{
	return NULL;
}

ASTNode* TypeQualifierList(ParserContext* ctx)
{
	//printf("Type Qualifier\n");
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
	//printf("Var\n");
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
