#include <stdio.h>

#include "Parser.h"

/* AI slop debug functions */

const char* NodeTypeToString(NodeType type) {
    switch(type) {
        case PROG_NODE: return "PROG_NODE";
        case FUNC_NODE: return "FUNC_NODE";
        case PARAM_LIST_NODE: return "PARAM_LIST_NODE";
        case PARAM_NODE: return "PARAM_NODE";
        case BODY_NODE: return "BODY_NODE";
        case STMT_LIST_NODE: return "STMT_LIST_NODE";
        case STMT_NODE: return "STMT_NODE";
        case EXPR_STMT_NODE: return "EXPR_STMT_NODE";
        case DECL_STMT_NODE: return "DECL_STMT_NODE";
        case CTRL_STMT_NODE: return "CTRL_STMT_NODE";
        case RETURN_STMT_NODE: return "RETURN_STMT_NODE";
        case IF_STMT_NODE: return "IF_STMT_NODE";
        case SWITCH_STMT_NODE: return "SWITCH_STMT_NODE";
        case WHILE_STMT_NODE: return "WHILE_STMT_NODE";
        case DO_WHILE_STMT_NODE: return "DO_WHILE_STMT_NODE";
        case FOR_STMT_NODE: return "FOR_STMT_NODE";
        case IF_NODE: return "IF_NODE";
        case ELIF_NODE: return "ELIF_NODE";
        case ELSE_NODE: return "ELSE_NODE";
        case CASE_NODE: return "CASE_NODE";
        case DEFAULT_NODE: return "DEFAULT_NODE";
        case EXPR_LIST_NODE: return "EXPR_LIST_NODE";
        case EXPR_NODE: return "EXPR_NODE";
        case TYPE_NODE: return "TYPE_NODE";
        case ARG_LIST_NODE: return "ARG_LIST_NODE";
        case VAR_LIST_NODE: return "VAR_LIST_NODE";
        case VAR_NODE: return "VAR_NODE";
        case IDENT_NODE: return "IDENT_NODE";
        case LITERAL_NODE: return "LITERAL_NODE";
        case EMPTY_NODE: return "EMPTY_NODE";
        case ASGN_EXPR_NODE: return "ASGN_EXPR_NODE";
        case BINARY_EXPR_NODE: return "BINARY_EXPR_NODE";
        case UNARY_EXPR_NODE: return "UNARY_EXPR_NODE";
        case CALL_FUNC_NODE: return "CALL_FUNC_NODE";
        case ARR_INDEX_NODE: return "ARR_INDEX_NODE";
        case ARR_INIT_NODE: return "ARR_INIT_NODE";
        case STRUCT_NODE: return "STRUCT_NODE";
        case STRUCT_BODY_NODE: return "STRUCT_BODY_NODE";
        case TYPEDEF_NODE: return "TYPEDEF_NODE";
        default: return "UNKNOWN_NODE";
    }
}

void PrintASTNode(ASTNode* node, int indent) {
    if (!node) return;

    int i;
    for(i = 0; i < indent; i++) printf("  ");

    printf("%s", NodeTypeToString(node->type));

    if (node->token.type != 0) { 
        printf(" [Token: '%s']", node->token.lex.word);
    }

    printf("\n");

    size_t j;
    for (j = 0; j < node->childCount; j++) {
        PrintASTNode(node->children[j], indent + 1);
    }
}

void PrintAST(AST* ast) {
    if (!ast || !ast->root) {
        printf("<empty AST>\n");
        return;
    }
    PrintASTNode(ast->root, 0);
}

int main(int argc, char* argv[])
{
    /* Open code file to read */
    FILE* fptr;
    fptr = fopen(argv[1], "r");
    if(!fptr)
    {
        printf("ERROR: Opening source file %s\n", argv[1]);
        return 1;
    }

    /* Build the ast */
    AST* ast = Program(fptr); 
    if (!ast) {
        printf("ERROR: ast failed to build\n");
    }
    PrintAST(ast);
	return 0;
}
