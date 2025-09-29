#include <Any.h>
#include <stdio.h>

B_ANY MakeAny(const void* rawData, DataType type)
{
    B_ANY any;
    any.size = sizeof(rawData);
    any.type = type;
    any.RawData = malloc(any.size);
    if(any.RawData)
    {
        memcpy(any.RawData, rawData, any.size);
    }
    return any;
}

bool IsArray(B_ANY any) 
{
    int size;

    switch (any.type)
    {
        case C_CHAR:
            size = sizeof(char);
            break;
        case C_SHORT:
            size = sizeof(short)
            break;
        case C_INT:
            size = sizeof(int);
            break;
        case C_FLOAT:
            size = sizeof(float);
            break;
        case C_DOUBLE:
            size = sizeof(double);
            break;
        default:
            return 0;
            break;
    }

    if(sizeof(any.RawData) > size)
        return true;
    return false;
}

