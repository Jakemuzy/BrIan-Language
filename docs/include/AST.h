#ifndef _BRIAN_AST_H_
#define _BRIAN_AST_H_

#include "ArenaAllocator.h"

#include "Token.h"

/*            BrIan AST
    ----------------------------
        Abstract syntax tree 
           implementation 

*/

typedef enum NodeType {
    // Program Structure
    PROG_NODE, FUNC_NODE, FUNC_DECL, FUNC_DEF,
    GEN_FUNC_NODE, REGULAR_FUNC_NODE, 
    PARAM_LIST_NODE, PARAM_NODE, GEN_PARAM_NODE,
    LAMBDA_NODE, BODY_NODE, STMT_LIST_NODE,

    // Statements
    EXPR_STMT_NODE, RETURN_STMT_NODE, JUMP_STMT_NODE,
    CONCURRENCY_STMT_NODE, LOCK_STMT_NODE, CRITICAL_STMT_NODE,

    // Declarations
    VAR_DECL_NODE, GEN_DECL_NODE, 
    STRUCT_DECL_NODE, GEN_STRUCT_DECL_NODE, GEN_STRUCT_BODY_NODE, 
    REGULAR_STRUCT_NODE, STRUCT_BODY_NODE, OPERATOR_OVERLOAD_NODE,
    INTERFACE_DECL_NODE, INTERFACE_BODY_NODE,
    ENUM_DECL_NODE, ENUM_BODY_NODE,
    TYPEDEF_DECL_NODE, TYPESPEC_NODE, TYPEDEF_POSTFIX_NODE,

    // Control Flow
    IF_STMT_NODE, ELIF_NODE, ELSE_NODE,
    SWITCH_STMT_NODE, CASE_STMT_NODE, DEFAULT_STMT_NODE,
    WHILE_STMT_NODE, DO_WHILE_STMT_NODE, FOR_STMT_NODE,

    // Expressions
    TERNARY_EXPR_NODE, ASGN_EXPR_NODE, BINARY_EXPR_NODE, UNARY_EXPR_NODE,
    CAST_NODE, INDEX_NODE, CALL_FUNC_NODE, 
    MEMBER_NODE, REF_NODE, SREF_NODE, SMEMBER_NODE,
    EXPR_LIST_NODE,

    // Types
    TYPE_NODE, CHANNEL_NODE, MATRIX_NODE, VECTOR_NODE,
    DECL_PREFIX_NODE, GENERIC_LIST_NODE, GENERIC_NODE,
    TYPE_QUALIFIER_NODE, QUALIFIER_LIST_NODE,
    LINKAGE_SPECIFIER_NODE, REGISTER_NODE,

    // Misc
    SIZEOF_NODE, HEX_NODE, PREDEFINED_VAR_NODE,
    ARG_LIST_NODE, VAR_LIST_NODE, VAR_NODE, ARR_DECL_NODE, ARR_INIT_LIST_NODE,
    LITERAL_NODE, IDENT_NODE,
} NodeType;

typedef struct ASTNode {
    struct ASTNode** children;
    size_t childCount, childCapacity;

    NodeType type;
    Token token;
} ASTNode;

typedef struct AST {
    ASTNode* root;
} AST;

// Arena handles allocation and destruction
AST* InitalizeAST(Arena* arena);
ASTNode* InitalizeASTNode(Arena* arena, NodeType type, Token token);
void AddChildASTNode(Arena* arena, ASTNode* parent, ASTNode* child);
void PrependChildASTNode(Arena* arena, ASTNode* parent, ASTNode* child);

#endif 
