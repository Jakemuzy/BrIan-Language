#include "Desugar.h"

/*
    TODO:
        Differentiate between prefix and postfix
        prefix should have the new value, postfix should have the old when desugared
*/

/* ---------- Helper ---------- */

Token NewToken(char* lex, TokenType tokType, ASTNode* original) 
{
    size_t size = strlen(lex);
    Token tok = (Token){tokType, (Lexeme){lex, size + 1, size + 1}, original->token.line};
    return tok;
}

void ASTInsert(ASTNode* parent, size_t childPos, ASTNode** children, size_t childCount)
{
    /* Inserts element at pos */

    parent->childCount += childCount;
    parent->children = realloc(parent->children, parent->childCount * sizeof(ASTNode*));

    /* Shift */
    for (size_t i = parent->childCount - 1; i >= childPos + childCount; --i)
        parent->children[i] = parent->children[i - childCount];

    /* Infill */
    for (size_t i = childPos, j = 0; i < childPos + childCount; i++, j++)
        parent->children[i] = children[j];
}

/* -------------------- */

AST* Desugar(AST* input) 
{
    /* 
    Depth first detect node types
    if its a node that needs desugaring, desugar it 
    assigned shadowed variables unique names, this is done via the symbol table
    */
    input->root = DesugarNode(input->root);
    return input;
}

ASTNode* DesugarNode(ASTNode* input)
{
    switch (input->type) {
        case (ASGN_EXPR_NODE):
            return DesugarAssignNode(input);
        case (BINARY_EXPR_NODE):
            return DesugarBinaryNode(input);
        case (UNARY_EXPR_NODE):
            return DesugarUnaryNode(input);
        case (DO_WHILE_STMT_NODE):
            return DesugarDoWhile(input);
        case (SWITCH_STMT_NODE):
            return DesugarSwitch(input);
        case (IF_STMT_NODE):
            return DesugarIf(input);
        case (IDENT_NODE):
            /* Remove shadowing from ST, give custom name */
    }

    for (size_t i = 0; i < input->childCount; i++) {
        /* Handle For Node custom since it adds additional nodes to parent */
        if (input->children[i]->type == FOR_STMT_NODE)
           DesugarFor(input->children[i], input, i);
        else 
            input->children[i] = DesugarNode(input->children[i]);
    }

    return input;
}


ASTNode* DesugarAssignNode(ASTNode* input)
{
    ASTNode* operatorNode = InitASTNode();
    operatorNode->type = BINARY_EXPR_NODE;

    switch (input->token.type) {
        case PEQ:     operatorNode->token = NewToken("+", PLUS, input->children[0]); break;
        case SEQ:     operatorNode->token = NewToken("-", MINUS, input->children[0]); break;
        case MEQ:     operatorNode->token = NewToken("*", MULT, input->children[0]); break;
        case DEQ:     operatorNode->token = NewToken("/", DIV, input->children[0]); break;
        case MODEQ:   operatorNode->token = NewToken("%", MOD, input->children[0]); break;
        case ANDEQ:   operatorNode->token = NewToken("&", AND, input->children[0]); break;
        case OREQ:    operatorNode->token = NewToken("|", OR, input->children[0]); break;
        case ANDLEQ:  operatorNode->token = NewToken("&&", ANDL, input->children[0]); break;
        case ORLEQ:   operatorNode->token = NewToken("||", ORL, input->children[0]); break;
        case NEGEQ:   operatorNode->token = NewToken("~", NEG, input->children[0]); break;
        case XOREQ:   operatorNode->token = NewToken("^", XOR, input->children[0]); break;
        case RIGHTEQ: operatorNode->token = NewToken(">>", RSHIFT, input->children[0]); break;
        case LEFTEQ:  operatorNode->token = NewToken("<<", LSHIFT, input->children[0]); break; 
        default: 
            ASTFreeNodes(1, operatorNode);
            return DesugarNode(input);
    }

    input->type = ASGN_EXPR_NODE;
    input->token = NewToken("=", EQ, input->children[0]);

    ASTPushChildNode(operatorNode, input->children[0]);
    ASTPushChildNode(operatorNode, input->children[1]);
    input->children[1] = operatorNode;
    return input;
}

ASTNode* DesugarBinaryNode(ASTNode* input) 
{

    switch (input->type) {
        case (POW):
            /*operatorNode->type = MULT;  
            ASTPushChildNode(operatorNode, input->children[0]);
            ASTPushChildNode(operatorNode, input->children[1]);
            ASTFreeNodes(1, operatorNode);
            return operatorNode;
            */
    }
    return NULL;
}

ASTNode* DesugarUnaryNode(ASTNode* input)
{
    /* TODO: Default should return DesugarNode because could be nested */
    /* TODO: prefix vs postfix */

    /* TODO: Postfix will have to create another node where it modifies after and saves current first */

    ASTNode* operatorNode = InitASTNode();
    ASTNode* literalNode = InitASTNode();
    literalNode->type = LITERAL_NODE;
    literalNode->token = NewToken("1", INTEGRAL, input->children[0]);

    switch (input->token.type) {
        case (PP): operatorNode->token = NewToken("+", PLUS, input->children[0]);  break;
        case (SS): operatorNode->token = NewToken("-", MINUS, input->children[0]);; break;
        default: 
            ASTFreeNodes(2, operatorNode, literalNode);
            return DesugarNode(input);
    }

    input->type = ASGN_EXPR_NODE;
    input->token = NewToken("=", EQ, input);

    operatorNode->type = BINARY_EXPR_NODE;
    ASTPushChildNode(operatorNode, input->children[0]);
    ASTPushChildNode(operatorNode, literalNode);

    ASTPushChildNode(input, operatorNode);
    return input;
}

ASTNode* DesugarFor(ASTNode* input, ASTNode* parent, size_t pos)
{
    /* Have to pass parent because declaration gets pushed before while loop */
    ASTNode* whileNode = InitASTNode();
    whileNode->type = WHILE_STMT_NODE;

    /* Add declarations */
    ASTNode* declaration = input->children[0];
    ASTNode** children = malloc(sizeof(ASTNode*) * declaration->childCount);
    for (size_t i = 0; i < declaration->childCount; i++) 
        children[i] = DesugarNode(declaration->children[i]);
    ASTInsert(parent, pos, children, declaration->childCount);

    
    ASTNode* condition = input->children[1];
    ASTPushChildNode(whileNode, DesugarNode(condition));


    ASTNode* increment = input->children[2];
    ASTNode* bodyNode = InitASTNode();
    bodyNode->type = BODY_NODE;
    for (size_t i = 0; i < increment->childCount; i++) 
        ASTPushChildNode(bodyNode, DesugarNode(increment->children[i]));
    ASTPushChildNode(whileNode, bodyNode);

    parent->children[pos + declaration->childCount] = whileNode;
    return input;
}

ASTNode* DesugarDoWhile(ASTNode* input)
{
    /* The only difference between while and do while is the order of condition and body */
    input->type = WHILE_STMT_NODE;

    ASTNode* temp = DesugarNode(input->children[0]);
    input->children[0] = DesugarNode(input->children[1]);
    input->children[1] = temp;
    return input;
}

ASTNode* DesugarSwitch(ASTNode* input)
{
    /* 
    Since mine takes exprs and not literals, it would desugar to an 
    if elif else node, I opt to do this later, since I plan on actually 
    making the switch only allow literals  
    */
}

ASTNode* DesugarIf(ASTNode* input)
{
    ASTNode* elseNode = NULL;
    size_t elifEnd = input->childCount;
    if (input->children[input->childCount - 1]->type == ELSE_NODE) {
        elseNode = input->children[input->childCount - 1];
        elifEnd = input->childCount - 1;
    }

    for (size_t i = elifEnd; i-- > 1; ) {
        /* Converts to an if node and nests inside of an else node */
        ASTNode* elif = input->children[i];
        elif->type = IF_STMT_NODE;   

        ASTNode* newElse = InitASTNode();
        newElse->type = ELSE_NODE;
        ASTPushChildNode(newElse, DesugarNode(elif));
        if (elseNode != NULL)
            ASTPushChildNode(newElse, elseNode);
        elseNode = newElse;
    }

    input->children[0] = DesugarNode(input->children[0]);
    if (elseNode != NULL)
        ASTPushChildNode(input->children[0], elseNode);

    /* Cleans up the old node */
    for (size_t i = 1; i < input->childCount; i++)
        input->children[i] = NULL;
    input->childCount = 1;
    return input;
}
