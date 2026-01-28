#ifndef _TYPE_CHECKER_H__
#define _TYPE_CHECKER_H__

#include "NameResolver.h"

typedef enum {
    TYPE_INT, TYPE_CHAR, TYPE_DOUBLE, TYPE_FLOAT,
    TYPE_PTR, TYPE_FUNC, TYPE_STRUCT, TYPE_STRING
} TypeKind;


typedef struct TYPE {
    enum {
        TYPE_NULL, TYPE_INT, TYPE_STRING, TYPE_ARR,
        TYPE_VOID, TYPE_DOUBLE, TYPE_FLOAT
    } kind;
    union {
        struct TYPE* array;
        struct {Symbol* sym; struct TYPE* type;} name;
    } u;
} TYPE;

TYPE* TY_NULL(void);
TYPE* TY_INT(void);
TYPE* TY_STRING(void);
TYPE* TY_VOID(void);

TYPE* TY_ARR(TYPE* type);
TYPE* TY_NAME(Symbol* sym, TYPE* type);

#endif 