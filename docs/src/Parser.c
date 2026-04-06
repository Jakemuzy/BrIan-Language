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

void Program(ParserContext* ctx) 
{
    ctx->ast->root = 
}