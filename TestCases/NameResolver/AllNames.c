#include <stdio.h>
#include <stdlib.h>

#include "NameResolver.h"
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
        case VAR_DECL_NODE: return "VAR_DECL_NODE";
        case STRUCT_DECL_NODE: return "STRUCT_DECL_NODE";
        case ENUM_DECL_NODE: return "ENUM_DECL_NODE";
        case TYPEDEF_DECL_NODE: return "TYPEDEF_DECL_NODE";
        case STRUCT_BODY_NODE: return "STRUCT_BODY_NODE";
        case ENUM_BODY_NODE: return "ENUM_BODY_NODE";
        case TYPEDEF_POSTFIX_ARR: return "TYPEDEF_POSTFIX_ARR";
        case TYPEDEF_POSTFIX_PTR: return "TYPEDEF_POSTFIX_PTR";
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
        case VAR_EXPR_LIST_NODE: return "VAR_EXPR_LIST_NODE";
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
        case ARR_DECL_NODE: return "ARR_DECL_NODE";
        case MEMBER_ACCESS_NODE: return "MEMBER_ACCESS_NODE";
        case SAFE_MEMBER_ACCESS_NODE: return "SAFE_MEMBER_ACCESS_NODE";
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

typedef struct {
    const char* filename;
    bool shouldPass;
} TestCase;

int main(int argc, char* argv[])
{
    TestCase tests[] = {
        {"TestCases/NameResolver/AllCases.b", true},
        {"TestCases/NameResolver/ForBodyInit.b", true},
        {"TestCases/NameResolver/ForEscape.b", false},
        {"TestCases/NameResolver/ForScope.b", true},
        {"TestCases/NameResolver/ForShadow.b", true},
        {"TestCases/NameResolver/FuncLeak.b", false},
        {"TestCases/NameResolver/FuncShadow.b", true},
        {"TestCases/NameResolver/FuncUndefined.b", false},
        {"TestCases/NameResolver/IfNest.b", true},
        {"TestCases/NameResolver/IfNestDouble.b", true},
        {"TestCases/NameResolver/IfNestShadow.b", true},
        {"TestCases/NameResolver/IfScope.b", true},
        {"TestCases/NameResolver/IfShadow.b", true},
        {"TestCases/NameResolver/MultiVar.b", true},
        {"TestCases/NameResolver/ParamShadow.b", true},
        {"TestCases/NameResolver/Recursion.b", true},
        {"TestCases/NameResolver/Redeclaration.b", false},
        {"TestCases/NameResolver/Resolved.b", true},
        {"TestCases/NameResolver/Undefined.b", false},
        {"TestCases/NameResolver/VarCall.b", false},
        {"TestCases/NameResolver/VarEscape.b", false},
        {"TestCases/NameResolver/VarGlob.b", true},
        {"TestCases/NameResolver/VarInvalidScope.b", false},
        {"TestCases/NameResolver/VarShadow.b", true},
        {"TestCases/NameResolver/DoWhile.b", false},
        {"TestCases/NameResolver/While.b", false},
        {"TestCases/NameResolver/Switch.b", true},
        {"TestCases/NameResolver/For.b", true},
        {"TestCases/NameResolver/Return.b", true},
        {"TestCases/NameResolver/Struct.b", true},
        {"TestCases/NameResolver/IfVar.b", true},
        {"TestCases/NameResolver/IfVarUndefined.b", false},
        {"TestCases/NameResolver/WhileShadow.b", true},
        {"TestCases/NameResolver/WhileUndefined.b", false},
        {"TestCases/NameResolver/DoWhileBasic.b", true},
        {"TestCases/NameResolver/DoWhileInvalidScope.b", false},
        {"TestCases/NameResolver/ForInner.b", true},
        {"TestCases/NameResolver/ForInvalid.b", false},
        {"TestCases/NameResolver/SwitchBasic.b", true},
        {"TestCases/NameResolver/SwitchUndefined.b", false},
        {"TestCases/NameResolver/ReturnUndefined.b", false},
        {"TestCases/NameResolver/StructBasic.b", true},
        {"TestCases/NameResolver/StructFunc.b", true},
        {"TestCases/NameResolver/StructUndefined.b", true},
        {"TestCases/NameResolver/StructInvalidField.b", true},
        {"TestCases/NameResolver/NestedStructs.b", true},
        {"TestCases/NameResolver/EnumBasic.b", true},
        {"TestCases/NameResolver/EnumInvalid.b", true},
        {"TestCases/NameResolver/FuncArgs.b", true},
        {"TestCases/NameResolver/FuncBadArgs.b", false},
        {"TestCases/NameResolver/FuncCall.b", true},
        {"TestCases/NameResolver/FuncInvalidCall.b", false},
        {"TestCases/NameResolver/FuncExcessArgs.b", false},
        {"TestCases/NameResolver/FuncMinimalArgs.b", false},
    };

    int expectedPass = -1;
    size_t numTests = sizeof(tests) / sizeof(tests[0]);
    for (size_t i = 0; i < numTests; i++) {
        if (strcmp(tests[i].filename, argv[1]) == 0) {
            expectedPass = tests[i].shouldPass;
            break;
        }
    }

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
        exit(1);
    }
   
    //PrintAST(ast);

    if (!ast) {
        printf("ERROR: AST failed to build for %s\n", argv[1]);
        return expectedPass ? 1 : 0;  
    }

    Namespaces* nss = ResolveNames(ast);
    int resultPass = (nss != NULL) ? 1 : 0;

    return (resultPass == expectedPass) ? 0 : 1;
}    