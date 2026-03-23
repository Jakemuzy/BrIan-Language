#include "Desugar.h"

Token NewToken(char* lex, TokenType tokType, ASTNode* original) 
{
    Token tok = (Token){tokType, (Lexeme){lex, 2, 2}, original->token.line};
    return tok;
}


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


ASTNode* DesugarBinaryNode(ASTNode* input)
{
    /* TODO: Default should return DesugarNode because could be nested */

    ASTNode* eqNode = InitASTNode();
    eqNode->type = EQ;
    ASTPushChildNode(eqNode, input->children[0]);

    ASTNode* operatorNode = InitASTNode();

    switch (input->token.type) {
        case PEQ:    operatorNode->type = PLUS;   break;
        case SEQ:    operatorNode->type = MINUS;  break;
        case MEQ:    operatorNode->type = MULT;   break;
        case DEQ:    operatorNode->type = DIV;    break;
        case MODEQ:  operatorNode->type = MOD;    break;
        case ANDEQ:  operatorNode->type = AND;    break;
        case OREQ:   operatorNode->type = OR;     break;
        case ANDLEQ: operatorNode->type = ANDL;   break;
        case ORLEQ:  operatorNode->type = ORL;    break;
        case NEGEQ:  operatorNode->type = NEG;    break;
        case XOREQ:  operatorNode->type = XOR;    break;
        case RIGHTEQ:operatorNode->type = RSHIFT; break;
        case LEFTEQ: operatorNode->type = LSHIFT; break; 
        case (POW):
            operatorNode->type = MULT;  
            ASTPushChildNode(operatorNode, input->children[0]);
            ASTPushChildNode(operatorNode, input->children[1]);
            ASTFreeNodes(1, eqNode);
            return operatorNode;
        default: 
            ASTFreeNodes(2, eqNode, operatorNode);
            return DesugarNode(input);
    }

    ASTPushChildNode(operatorNode, input->children[0]);
    ASTPushChildNode(operatorNode, input->children[1]);
    ASTPushChildNode(eqNode, operatorNode);
    return eqNode;
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
    for (size_t i = 0; i < declaration->childCount; i++) parent->childCount++;
    parent->children = realloc(parent->children, parent->childCount * sizeof(ASTNode*));

    /* Shift */
    for (size_t i = parent->childCount - 1; i >= pos + declaration->childCount; --i)
        parent->children[i] = parent->children[i - declaration->childCount];

    /* Infill */
    for (size_t i = pos, j = 0; i < pos + declaration->childCount; i++, j++)
        parent->children[i] = DesugarNode(declaration->children[j]);
    
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
