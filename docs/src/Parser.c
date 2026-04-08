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

/* Maybe not */
static inline bool HandleResult(ParserContext* ctx, ParseResult r, ASTNode* parent, TokenType sync) {
    if (r.flag == PARSE_ERROR) { SyncRecovery(ctx, sync); return false; }
    AddChildASTNode(ctx->arena, parent, r.node);
    return true;
}

static inline void Advance(ParserContext* ctx) { ctx->current = GetNextToken(ctx->tokenizer); }
static inline bool IsError(ParseResult* result) { return result->flag == PARSE_ERROR; }
static inline bool Match(ParserContext* ctx, TokenType t) {
    if (ctx->current.type != t) return false;
    Advance(ctx);
    return true;
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

ParserContext* InitalizeParserContext(TokenizerContext* tokenizer, size_t fileSize)
{
    ParserContext* parser = malloc(sizeof(ParserContext));

    /* Make the constant variable depending on density */
    size_t arenaSize = fileSize * ARENA_CAPACITY_MULTIPLIER_FROM_FILESIZE;

    parser->tokenizer = tokenizer;
    parser->arena = CreateArena(arenaSize);
    parser->ast = NULL;
    return parser;
}

void DestroyParserContext(ParserContext* ctx)
{
    ResetArena(ctx->arena);
}

void SyncRecovery(ParserContext* ctx, TokenType tt) 
{
    while (ctx->current.type != tt && ctx->current.type != END) {
        Advance(ctx);
    }
}

/* ----- Recursive Descent ----- */

void Program(ParserContext* ctx) 
{
    printf("Program\n");
    ctx->ast = InitalizeAST(ctx->arena);
   
    while (true) {
        ctx->current = GetNextToken(ctx->tokenizer);
        switch (ctx->current.type) {
            case FUNCTION: 
                printf("Func\n");
                ParseResult funcNode = Function(ctx);
				if (funcNode.flag == PARSE_ERROR) SyncRecovery(ctx, RBRACE);
                AddChildASTNode(ctx->arena, ctx->ast->root, funcNode.node);
                break;
            case INTERFACE:
				printf("Interface\n");
                ParseResult interfaceDeclNode = InterfaceDecl(ctx);
				if (interfaceDeclNode.flag == PARSE_ERROR) SyncRecovery(ctx, RBRACE);
                AddChildASTNode(ctx->arena, ctx->ast->root, interfaceDeclNode.node);
                break;
			case LET:
                printf("VarDecl\n");
                ParseResult varDeclNode = VarDecl(ctx);
				if (varDeclNode.flag == PARSE_ERROR) SyncRecovery(ctx, SEMI);
                AddChildASTNode(ctx->arena, ctx->ast->root, varDeclNode.node);
                break;
            case END:   
                printf("End\n");
                return;
			default: 
				ERROR(ERR_FLAG_EXIT, PARSER_ERR, "Unexpected token occured in glboal scope: '%s' on line %d, col %d", ctx->current, ctx->current.row, ctx->current.col);
        }
    }
}


ParseResult DeclQualifiers(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Function(ParserContext* ctx)
{
	ParseResult funcNode, linkageNode, qualifierNode;
	ctx->current = GetNextToken(ctx->tokenizer);

	/* Optional Specifiers and Qualifiers */
	if (ctx->current.type == EXTERN) { linkageNode = LinkageSpecifier(ctx); ctx->current = GetNextToken(ctx->tokenizer); }
	switch(ctx->current.type) { QUALIFIER_CASES  qualifierNode = TypeQualifierList(ctx); ctx->current = GetNextToken(ctx->tokenizer); }

	switch (ctx->current.type) {

		TYPE_CASES
		case IDENT:
			printf("Regular\n");
			funcNode = RegularFunc(ctx);
			if (funcNode.flag == PARSE_ERROR) return (ParseResult) { PARSE_ERROR, NULL };
			break;
		case GEN: 
			printf("Generic\n");
			funcNode = GenericFunc(ctx);
			if (funcNode.flag == PARSE_ERROR) return (ParseResult) { PARSE_ERROR, NULL };
			break;
	}

	// These must be above since they'll be appended to the end instead of beginning 
	if (linkageNode.node) AddChildASTNode(ctx->arena, funcNode.node, linkageNode.node);
	if (qualifierNode.node) AddChildASTNode(ctx->arena, funcNode.node, qualifierNode.node);

	switch (ctx->current.type) {
		case SEMI: 
			printf("Decl\n");
			funcNode.node->type = FUNC_DECL;
			break;
		case LBRACE:
			printf("Def\n");
			funcNode.node->type = FUNC_DEF;
			ParseResult bodyNode = Body(ctx);
			if (funcNode.flag == PARSE_ERROR) return (ParseResult) { PARSE_ERROR, NULL };
			AddChildASTNode(ctx->arena, funcNode.node, bodyNode.node);
			break;
		default:
			ERROR(ERR_FLAG_CONTINUE, PARSER_ERR, "Expected ';' for function declaration or '{' for function definition. Instead encountered '%s' on line %d col %d.\n", ctx->current.lexeme, ctx->current.row, ctx->current.col);
			return (ParseResult) { PARSE_ERROR, NULL };
	}

	return (ParseResult){ PARSE_VALID, funcNode.node };
}

ParseResult GenericFunc(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult RegularFunc(ParserContext* ctx)
{
	ASTNode* funcNode = InitalizeASTNode(ctx->arena, REGULAR_FUNC_NODE, ctx->current);

	// Optional DeclPrefix
	ctx->current = GetNextToken(ctx->tokenizer);
	if (ctx->current.type == MOD || ctx->current.type == MULT) {
		AddChildASTNode(ctx->arena, funcNode, InitalizeASTNode(ctx->arena, DECL_PREFIX_NODE, ctx->current));
		ctx->current = GetNextToken(ctx->tokenizer);
	}

	// Identifier name 
	if (ctx->current.type != IDENT) {
		ERROR(ERR_FLAG_CONTINUE, PARSER_ERR, "Function name expected, instead encountered '%s' on line %d col %d\n", ctx->current.lexeme, ctx->current.row, ctx->current.col);
		return (ParseResult) {PARSE_ERROR, NULL};
	}
	AddChildASTNode(ctx->arena, funcNode, InitalizeASTNode(ctx->arena, IDENT_NODE, ctx->current));
	ctx->current = GetNextToken(ctx->tokenizer);

	if (ctx->current.type != LPAREN) {
		ERROR(ERR_FLAG_CONTINUE, PARSER_ERR, "'(' expecte in function name, instead encountered '%s' on line %d col %d\n", ctx->current.lexeme, ctx->current.row, ctx->current.col);
		return (ParseResult) {PARSE_ERROR, NULL};
	}
	ctx->current = GetNextToken(ctx->tokenizer);


	switch (ctx->current.type) {
		QUALIFIER_CASES
		TYPE_CASES
		case IDENT_NODE: 
			ParseResult paramListNode = ParamList(ctx);
			AddChildASTNode(ctx->arena, funcNode, InitalizeASTNode(ctx->arena, PARAM_LIST_NODE, ctx->current));
			// No break since rparen comes after
			ctx->current = GetNextToken(ctx->tokenizer);
			break;
	}

	if (ctx->current.type != RPAREN) {
		ERROR(ERR_FLAG_CONTINUE, PARSER_ERR, "')' expected in function name, instead encountered '%s' on line %d col %d", ctx->current.lexeme, ctx->current.row, ctx->current.col);
		return (ParseResult) {PARSE_ERROR, NULL};
	}
	ctx->current = GetNextToken(ctx->tokenizer);

	return (ParseResult){ PARSE_VALID, funcNode };
}


ParseResult ParamList(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Param(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult GenParam(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}


ParseResult Lamba(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Body(ParserContext* ctx)
{
	printf("BODY\n");
	ASTNode* bodyNode = InitalizeASTNode(ctx->arena, BODY_NODE, ctx->current);

	/* TODO: groupings that follow the grammar, simplifies and makes recursive descent easier */
	while (true) {
		Advance(ctx);
		switch(ctx->current.type) {
			case IF: 
				ParseResult ifNode = IfStmt(ctx);
				if (IsError(&ifNode)) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, ifNode.node);
				break;
			case SWITCH: 
				ParseResult switchNode = SwitchStmt(ctx);
				if (IsError(&switchNode)) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, switchNode.node);
				break;
			case WHILE: 
				ParseResult whileNode = WhileStmt(ctx);
				if (IsError(&whileNode)) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, whileNode.node);
				break;
			case DO: 
				ParseResult doNode = DoWhlieStmt(ctx);
				if (IsError(&doNode)) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, doNode.node);
				break;
			case FOR: 
				ParseResult forNode = ForStmt(ctx);
				if (IsError(&forNode)) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, forNode.node);
				break;

			case LET: 
				ParseResult varDeclNode = VarDecl(ctx);
				if (IsError(&forNode)) SyncRecovery(ctx, SEMI);
				else AddChildASTNode(ctx->arena, bodyNode, varDeclNode.node);
				break;
			case ENUM:
				ParseResult enumNode = EnumDecl(ctx);
				if (IsError(&enumNode)) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, enumNode.node);
				break;
			case TYPEDEF: 
				ParseResult typedefNode = TypedefDecl(ctx);
				if (IsError(&typedefNode)) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, typedefNode.node);
				break;
			case STRUCT:
				ParseResult structNode = StructDecl(ctx);
				if (IsError(&structNode)) SyncRecovery(ctx, RBRACE);
				else AddChildASTNode(ctx->arena, bodyNode, structNode.node);
				break;

			// Don't do anything here use pratt parsing
			case SEMI: // case any thing that can be FIRST(Expr)
			EXPR_START_CASES
				// Expr Stmt 

			case RETURN:

			case BREAK: case CONTINUE:

			case LOCK: case CRITICAL:

			case RBRACE: return (ParseResult) {PARSE_VALID, bodyNode};
			default: return (ParseResult){ PARSE_ERROR, NULL };
		}
	}
}

ParseResult StmtList(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Stmt(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}


ParseResult ExprStmt(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult DeclStmt(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}


ParseResult VarDecl(ParserContext* ctx)
{
	ASTNode* varDeclNode = InitalizeASTNode(ctx->arena, VAR_DECL_NODE, ctx->current);

	if (ctx->current.type == EXTERN) { AddChildASTNode(ctx->arena, varDeclNode, LinkageSpecifier(ctx).node); }
	switch(ctx->current.type) { QUALIFIER_CASES AddChildASTNode(ctx->arena, varDeclNode, TypeQualifierList(ctx).node); }

	if (!Match(ctx, LET)) { 
		ERROR(ERR_FLAG_CONTINUE, PARSER_ERR, "Expected 'let' before variable declaration. Instead encountered '%s' on line %d col %d\n", ctx->current.lexeme, ctx->current.row, ctx->current.col); 
		return (ParseResult){ PARSE_ERROR, NULL };
	}

	switch (ctx->current.type) {
		TYPE_CASES
			AddChildASTNode(ctx->arena, varDeclNode, InitalizeASTNode(ctx->arena, TYPE_NODE, ctx->current));
			Advance(ctx);
			break;
		case IDENT:
			AddChildASTNode(ctx->arena, varDeclNode, InitalizeASTNode(ctx->arena, IDENT_NODE, ctx->current));
			Advance(ctx);
			break;
		default:	
			ERROR(ERR_FLAG_CONTINUE, PARSER_ERR, "Expected TYPE for variable declaration, instead encountered '%s' on line %d col %d\n", ctx->current.lexeme, ctx->current.row, ctx->current.col);
			return (ParseResult){ PARSE_ERROR, NULL };
	}

	ParseResult varListNode = VarList(ctx);
	if (varListNode.flag == PARSE_ERROR) { SyncRecovery(ctx, SEMI); }
	else AddChildASTNode(ctx->arena, varDeclNode, varListNode.node);

	return (ParseResult){ PARSE_VALID,  varDeclNode };
}

ParseResult GenDecl(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult StructDecl(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult GenericStruct(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult GenStructBody(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult RegularStruct(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult StructBody(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult OperatorOverload(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

// Overloadable op 
ParseResult InterfaceDecl(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult InterfaceBody(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}


ParseResult EnumDecl(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult EnumBody(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult TypedefDecl(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult TypeSpec(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult TypedefPostfix(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}


ParseResult ConcurrencyStmt(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult LockStmt(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult CriticalStmt(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}


ParseResult IfStmt(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult SwitchStmt(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Case(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Default(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult WhileStmt(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult DoWhlieStmt(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult ForStmt(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult ExprList(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}


ParseResult Expr(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult TernaryExpr(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult AsgnExpr(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult BinaryExpr(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult UnaryExpr(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Primary(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}


ParseResult Type(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Channel(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Matrix(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Vector(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult DeclPrefix(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult GenericList(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Generic(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult TypeQualifierList(ParserContext* ctx)
{
	printf("Type Qualifier\n");
	ASTNode* qualifierListNode = InitalizeASTNode(ctx->arena, QUALIFIER_LIST_NODE, ctx->current);
	while (true) {
		switch (ctx->current.type) {
			QUALIFIER_CASES
				ASTNode* qualifierNode = InitalizeASTNode(ctx->arena, TYPE_QUALIFIER_NODE, ctx->current);
				AddChildASTNode(ctx->arena, qualifierListNode, qualifierNode);
				break;
			default: 
				return (ParseResult) { PARSE_VALID, qualifierListNode };
		}
		Advance(ctx);
	}
}

ParseResult TypeQualifier(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult LinkageSpecifier(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}


ParseResult Sizeof(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}


ParseResult Reg(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Hex(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult PredefVars(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}


ParseResult ArgList(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}


ParseResult VarList(ParserContext* ctx)
{
	ASTNode* varListNode = InitalizeASTNode(ctx->arena, VAR_LIST_NODE, ctx->current);

	while (true) {
		ParseResult varNode = Var(ctx);
		if (IsError(&varNode)) { SyncRecovery(ctx, SEMI); break; }
		AddChildASTNode(ctx->arena, varListNode, varNode.node);

		if (!Match(ctx, COMMA)) break;
	}

	return (ParseResult){ PARSE_VALID, varListNode };
}

ParseResult Var(ParserContext* ctx)
{
	printf("Var\n");
	if (ctx->current.type != IDENT) {
		ERROR(ERR_FLAG_CONTINUE, PARSER_ERR, "Expected identifier name for variable declaration, instead encountered '%s' on line %d col %d\n", ctx->current.lexeme, ctx->current.row, ctx->current.col);
		return (ParseResult){ PARSE_ERROR, NULL };
	}

	ASTNode* varNode = InitalizeASTNode(ctx->arena, IDENT_NODE, ctx->current);
    Advance(ctx);

	while (Match(ctx, LBRACK)) {
		ParseResult exprNode = Expr(ctx);
		if (IsError(&exprNode)) SyncRecovery(ctx, RBRACK);
		if (!Match(ctx, RBRACK)) {
			ERROR(ERR_FLAG_CONTINUE, PARSE_ERROR, "Expected ']' for array initialization, instead encountered '%s' on line %d col %d\n", ctx->current.lexeme, ctx->current.row, ctx->current.col);
			return (ParseResult) { PARSE_ERROR, NULL };
		}
	}

	if (Match(ctx, EQ)) {
		switch (ctx->current.type) {
			case LBRACE: // ARR init list 
			EXPR_START_CASES // Expr 
		}	
	}

    return (ParseResult){ PARSE_VALID, varNode };
}


ParseResult ArrDecl(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult ArrInitList(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Literal(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}
