#ifndef _BRIAN_OPERATOR_RULES_H_
#define _BRIAN_OPERATOR_RULES_H_

#include "Type.h"

/*      BrIan Operator Rules
    ----------------------------
     Refer to docs/pipeline.md 
           for details.  

       All the type checking
    rules that BrIan uses during
      its type checking phase.

*/

typedef enum TypeCategory {
    C_NUMERIC, C_INTEGRAL, C_SIGNED, C_UNSIGNED,
    C_DECIMAL, C_BOOLEAN,  C_TRUTHY, C_POINTER, 
    C_ANY
};



#endif
