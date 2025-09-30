#ifndef _ANY_H__
#define _ANY_H__

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* 
The purpose of this file is to mainly serve as a way to achieve "Generics" in C. It acts as a structure that stores a void* to the raw data, and keeps track of the type of the data 
*/

#define ANY(val, type) MakeAny(&(val), type)

typedef enum {
    C_CHAR, C_SHORT, C_INT, C_FLOAT, C_DOUBLE, C_STR, C_ANY
} DataType;

typedef struct B_ANY {
    void* RawData;
    size_t size;
    DataType type;
} B_ANY;

B_ANY MakeAny(const void* rawData, DataType type);
bool IsArray(B_ANY);

#endif
