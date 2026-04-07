#include "Parser.h"

#define TYPE_CASES \
    case CHAR: case BOOL: case INT: case LONG: case DOUBLE: case FLOAT: \
    case VOID: case STRING: case I8: case I16: case I32: case I64: \
    case U8: case U16: case U32: case U64: case MAT: case VEC: \
    case MUTEX: case SEMAPHORE: case TASK: case CHANNEL:

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
	while (GetNextToken(ctx->tokenizer).type != tt) ;
		// if (ctx->current.type == END && tt != END) ERROR();
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
                AddChildASTNode(ctx->arena, ctx->ast, funcNode.node);
                break;
            case INTERFACE:
				printf("Interface\n");
                ParseResult interfaceDeclNode = InterfaceDecl(ctx);
				if (interfaceDeclNode.flag == PARSE_ERROR) SyncRecovery(ctx, RBRACE);
                AddChildASTNode(ctx->arena, ctx->ast, interfaceDeclNode.node);
                break;
			case LET:
                printf("VarDecl\n");
                ParseResult varDeclNode = VarDecl(ctx);
				if (varDeclNode.flag == PARSE_ERROR) SyncRecovery(ctx, SEMI);
                AddChildASTNode(ctx->arena, ctx->ast, varDeclNode.node);
                break;
            case END:   
                printf("End\n");
                return;
			default: 
				ERROR(ERR_FLAG_EXIT, PARSER_ERR, "Unexpected token occured in glboal scope: '%s'", ctx->current);
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
	if (ctx->current.type == QUALIFIER) { qualifierNode = TypeQualifier(ctx); ctx->current = GetNextToken(ctx->tokenizer); }

	switch (ctx->current.type) {

		TYPE_CASES
		case IDENT:

		case GEN: 
	}

	if (linkageNode.node) AddChildASTNode(ctx->arena, funcNode.node, linkageNode.node);
	if (qualifierNode.node) AddChildASTNode(ctx->arena, funcNode.node, qualifierNode.node);

	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult FuncDecl(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult FuncDef(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}


ParseResult FuncSignature(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult GenericFunc(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult RegularFunc(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
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
	return (ParseResult){ PARSE_VALID, NULL };
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
	return (ParseResult){ PARSE_VALID, NULL };
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
	return (ParseResult){ PARSE_VALID, NULL };
}

ParseResult Var(ParserContext* ctx)
{
	return (ParseResult){ PARSE_VALID, NULL };
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
