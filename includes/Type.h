#ifndef _TYPE_H__
#define _TYPE_H__

#include "Symbol.h"

/* ---------- Type Structures  ----------- */

typedef enum TypeKind {
    TYPE_INT, TYPE_BOOL, TYPE_DOUBLE, TYPE_FLOAT,   /* TODO: could probably make int just implicity I32 */
    TYPE_PTR, TYPE_FUNC, TYPE_STRING, TYPE_ARR, 
    TYPE_VOID, TYPE_ERROR, TYPE_NULL, TYPE_NAME,

    TYPE_I8, TYPE_I16, TYPE_I32, TYPE_I64,
    TYPE_U8, TYPE_U16, TYPE_U32, TYPE_U64,

    TYPE_NAT    /* Not a type */
} TypeKind;

typedef struct TYPE {
    TypeKind kind;
    union {     /* kind == TYPE_ARR -> u.array | kind == OTHER -> u.name */
        struct { struct TYPE* element; size_t size; } array;     
        struct {Symbol* sym; struct TYPE* type;} name;  
    } u;
} TYPE;

typedef struct TYPE_LIST {  /* Especially useful for function paramater lists */
    TYPE* head;
    struct TYPE_LIST* tail;
} TYPE_LIST;

typedef struct TYPE_FIELD { /* Struct Fields */
/* 
Symbol stores type already, though TYPE* might be useful
here becaues of custom types, also consider the possibility
of TYPE_FIELDS storing a function type. (As in the case of 
a function inside a struct )
*/
    Symbol* sym;    
    TYPE* type;
} TYPE_FIELD;

typedef struct TYPE_FIELD_LIST {
    TYPE_FIELD* head;
    struct TYPE_FIELD_LIST* tail;
} TYPE_FIELD_LIST;

typedef struct TYPE_STRUCT {
    TYPE_FIELD_LIST* fields;
} TYPE_STRUCT;

/* Constructors */
TYPE* TY_ERROR(void);
TYPE* TY_VOID(void);
TYPE* TY_INT(void);
TYPE* TY_FLOAT(void);
TYPE* TY_DOUBLE(void);
TYPE* TY_BOOL(void);
TYPE* TY_STRING(void);
TYPE* TY_NULL(void);

TYPE* TY_I8();
TYPE* TY_I16();
TYPE* TY_I32();
TYPE* TY_I64();

TYPE* TY_U8();
TYPE* TY_U16();
TYPE* TY_U32();
TYPE* TY_U64();

TYPE* TY_ARR(TYPE* element, int size);
TYPE* TY_NAME(Symbol* sym, TYPE* type);

TYPE* TY_NAT();
TYPE_LIST TY_LIST(TYPE* head, TYPE_LIST* tail);
TYPE_FIELD* TY_FIELD(Symbol* sym, TYPE* type);
TYPE_FIELD_LIST* TY_FIELD_LIST(TYPE_FIELD* head, TYPE_FIELD_LIST* tail);
TYPE_STRUCT* TY_STRUCT(TYPE_FIELD_LIST* fields);

/* TODO: Implement structs as fields */

TYPE* StrToType(char* typeName);

#endif