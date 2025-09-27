#include <Any.h>

/* 
Static because I want implementation private, user should only call ANY(val) 
*/

B_ANY MakeAny(const void* rawData)
{
    B_ANY any;
    any.size = sizeof(rawData);
    any.RawData = malloc(any.size);
    if(any.RawData)
    {
        memcpy(any.RawData, rawData, any.size);
    }
    
    return any;
}
