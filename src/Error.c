#include "Error.h"

char* TokenToStr(TokenType type)
{
    switch (type)
    {
        /* Ctrl Stmt Key Words */
        case IF: return "if";
        case ELIF: return "elif";
        case ELSE: return "else";
        case DO: return "do";
        case WHILE: return "while";
        case FOR: return "for";
        case SWITCH: return "switch";
        case CASE: return "case";
        case DEFAULT: return "default";
        case BREAK: return "break";
        case RET: return "return";

        /* Data Type Key Words */
        case CHAR: return "char";
        case BOOL: return "bool";
        case SHORT: return "short";
        case INT: return "int";
        case FLOAT: return "float";
        case DOUBLE: return "double";
        case LONG: return "long";
        case VOID: return "void";
        case STRING: return "string";
        case I8: return "i8";
        case I16: return "i16";
        case I32: return "i32";
        case I64: return "i64";
        case U8: return "u8";
        case U16: return "u16";
        case U32: return "u32";
        case U64: return "u64";
        case ENUM: return "enum";
        case STRUCT: return "struct";
        case TYPEDEF: return "typedef";

        /* Qualifiers */
        case CONST: return "const";
        case SIGNED: return "signed";
        case UNSIGNED: return "unsigned";
        case STATIC: return "static";

        /* Assignment Operators */
        case EQ: return "=";
        case PEQ: return "+=";
        case SEQ: return "-=";
        case MEQ: return "*=";
        case DEQ: return "/=";
        case MODEQ: return "%=";
        case ANDEQ: return "&=";
        case OREQ: return "|=";
        case ANDLEQ: return "&&=";
        case ORLEQ: return "||=";
        case NEGEQ: return "~=";
        case XOREQ: return "^=";
        case RIGHTEQ: return ">>=";
        case LEFTEQ: return "<<=";
        case PP: return "++";
        case SS: return "--";

        /* Arithmetic Operators */
        case PLUS: return "+";
        case MINUS: return "-";
        case DIV: return "/";
        case MULT: return "*";
        case POW: return "**";
        case MOD: return "%";

        /* Comparison Operators */
        case EQQ: return "==";
        case NEQQ: return "!=";
        case GEQQ: return ">=";
        case LEQQ: return "<=";
        case NOT: return "!";
        case ANDL: return "&&";
        case ORL: return "||";
        case GREAT: return ">";
        case LESS: return "<";

        /* Bitwise */
        case NEG: return "~";
        case XOR: return "^";
        case OR: return "|";
        case AND: return "&";
        case LSHIFT: return ">>";
        case RSHIFT: return "<<";

        /* Brackets */
        case LPAREN: return ")";
        case RPAREN: return ")";
        case LBRACK: return "[";
        case RBRACK: return "]";
        case LBRACE: return "{";
        case RBRACE: return "}";

        /* Literals / Identifiers */
        case IDENT: return "ident";
        case INTEGRAL: return "integral";
        case DECIMAL: return "decimal";
        case CLITERAL: return "cliteral";
        case SLITERAL: return "sliteral";

        /* Memory */
        case REGISTER: return "register";
        case SREF: return "?->";
        case SMEM: return "?.";
        case QUESTION: return "?";
        case REF: return "->";
        case MEM: return ".";

        /* Other */
        case SEMI: return ";";
        case COLON: return ":";
        case COMMA: return ",";
        case HASH: return "#";
        case COMMENT: return "//";
        case END: return "END";
        case ERR: return "ERR";

        case NA: return "NA";

        default: return "UNKNOWN_TOKEN";
    }
}