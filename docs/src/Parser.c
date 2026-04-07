#include "Parser.h"

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

/* ----- Recursive Descent ----- */

void Program(ParserContext* ctx) 
{
    printf("Program\n");
    ctx->ast = InitalizeAST(ctx->arena);
   
    while (true) {
        Token tok = GetNextToken(ctx->tokenizer);
        switch (tok.type) {
            case INTERFACE:
                ASTNode* interfaceDeclNode = InterfaceDecl(ctx);
                // Check error
                AddChildASTNode(ctx->arena, ctx->ast, interfaceDeclNode);
                break;
            case EXTERN: 
            case STATIC: case INLINE: case CONST: case VOLATILE: case ATOMIC:
                ASTNode* funcOrDecl = DeclQualifiers(ctx);
                // Check Error
                AddChildASTNode(ctx->arena, ctx->ast, funcOrDecl);
                break;
            case FUNCTION: 
                printf("Func\n");
                ASTNode* funcNode = Function(ctx);
                break;
            case IDENT:
                printf("VarDecl\n");
                ASTNode* varDeclNode = VarDecl(ctx);
                break;
            case END:   
                printf("End\n");
                // end 
                return;
                break;
        }
    }
}


ASTNode* DeclQualifiers(ParserContext* ctx)
{
	return NULL;
}


ASTNode* Function(ParserContext* ctx)
{
	return NULL;
}

ASTNode* FuncDecl(ParserContext* ctx)
{
	return NULL;
}

ASTNode* FuncDef(ParserContext* ctx)
{
	return NULL;
}


ASTNode* FuncSignature(ParserContext* ctx)
{
	return NULL;
}

ASTNode* GenericFunc(ParserContext* ctx)
{
	return NULL;
}

ASTNode* RegularFunc(ParserContext* ctx)
{
	return NULL;
}


ASTNode* ParamList(ParserContext* ctx)
{
	return NULL;
}

ASTNode* Param(ParserContext* ctx)
{
	return NULL;
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
	return NULL;
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
	return NULL;
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
	return NULL;
}

ASTNode* InterfaceBody(ParserContext* ctx)
{
	return NULL;
}


ASTNode* EnumDecl(ParserContext* ctx)
{
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


ASTNode* Expr(ParserContext* ctx)
{
	return NULL;
}

ASTNode* TernaryExpr(ParserContext* ctx)
{
	return NULL;
}

ASTNode* AsgnExpr(ParserContext* ctx)
{
	return NULL;
}

ASTNode* BinaryExpr(ParserContext* ctx)
{
	return NULL;
}

ASTNode* UnaryExpr(ParserContext* ctx)
{
	return NULL;
}

ASTNode* Primary(ParserContext* ctx)
{
	return NULL;
}


ASTNode* Type(ParserContext* ctx)
{
	return NULL;
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
	return NULL;
}

ASTNode* Var(ParserContext* ctx)
{
	return NULL;
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
