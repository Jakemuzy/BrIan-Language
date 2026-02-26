#include "Type.h"

/* ---------- Types ---------- */

TYPE* TY_VOID(void)   { static TYPE t = { TYPE_VOID };   return &t; }
TYPE* TY_ERROR(void)  { static TYPE t = { TYPE_ERROR };  return &t; }
TYPE* TY_NULL(void)   { static TYPE t = { TYPE_NULL };   return &t; }

TYPE* TY_INT(void)    { static TYPE t = { TYPE_INT };    return &t; }
TYPE* TY_FLOAT(void)  { static TYPE t = { TYPE_FLOAT };  return &t; }
TYPE* TY_DOUBLE(void) { static TYPE t = { TYPE_DOUBLE }; return &t; }
TYPE* TY_BOOL(void)   { static TYPE t = { TYPE_BOOL };   return &t; }
TYPE* TY_STRING(void) { static TYPE t = { TYPE_STRING }; return &t; }

TYPE* TY_I8(void)  { static TYPE t = { TYPE_I8 };  return &t; }
TYPE* TY_I16(void) { static TYPE t = { TYPE_I16 }; return &t; }
TYPE* TY_I32(void) { static TYPE t = { TYPE_I32 }; return &t; }
TYPE* TY_I64(void) { static TYPE t = { TYPE_I64 }; return &t; }

TYPE* TY_U8(void)  { static TYPE t = { TYPE_U8 };  return &t; }
TYPE* TY_U16(void) { static TYPE t = { TYPE_U16 }; return &t; }
TYPE* TY_U32(void) { static TYPE t = { TYPE_U32 }; return &t; }
TYPE* TY_U64(void) { static TYPE t = { TYPE_U64 }; return &t; }

TYPE* TY_ARR(TYPE* type, int size) { TYPE* typ = malloc(sizeof(TYPE)); typ->kind = TYPE_ARR; typ->u.array.element = type; typ->u.array.size = size; return typ; }
TYPE* TY_NAME(Symbol* sym, TYPE* type) { TYPE* typ = malloc(sizeof(TYPE)); typ->kind = TYPE_NAME; typ->u.name.sym = sym; typ->u.name.type = type; return typ; }

TYPE* TY_NAT() { static TYPE t = { TYPE_NAT }; return &t; }
TYPE_LIST TY_LIST(TYPE* head, TYPE_LIST* tail);

TYPE* StrToType(char* typeName)
{
    if (!typeName) return NULL;

    // Built-in primitive types
    if (strcmp(typeName, "void") == 0)   return TY_VOID();
    if (strcmp(typeName, "int") == 0)    return TY_INT();
    if (strcmp(typeName, "char") == 0)   return TY_INT();   /* Implicit */
    if (strcmp(typeName, "float") == 0)  return TY_FLOAT();
    if (strcmp(typeName, "double") == 0) return TY_DOUBLE();
    if (strcmp(typeName, "bool") == 0)   return TY_BOOL();
    if (strcmp(typeName, "string") == 0) return TY_STRING();

    if (strcmp(typeName, "I8") == 0)   return TY_I8();
    if (strcmp(typeName, "I16") == 0)  return TY_I16();
    if (strcmp(typeName, "I32") == 0)  return TY_I32();
    if (strcmp(typeName, "I64") == 0)  return TY_I64();

    if (strcmp(typeName, "U8") == 0)   return TY_U8();
    if (strcmp(typeName, "U16") == 0)  return TY_U16();
    if (strcmp(typeName, "U32") == 0)  return TY_U32();
    if (strcmp(typeName, "U64") == 0)  return TY_U64();

    // Not a recognized built-in type
    return NULL;
}