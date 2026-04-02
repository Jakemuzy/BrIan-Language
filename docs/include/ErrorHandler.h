#ifndef _BRIAN_ERROR_HANDLER_H_ 
#define _BRIAN_ERROR_HANDLER_H_

#include "Token.h"

/*      BrIan Error Handler
    ----------------------------
     Refer to docs/pipeline.md 
           for details.   
*/

typedef enum {
    ERR_FLAG_PROPOGATE, ERR_FLAG_IGNORE, 
    ERR_FLAG_CONTINUE,  ERR_FLAG_ABORT
} ErrorFlag;

typedef enum {
    TOKENIZER_ERR, PARSER_ERR, PREPROCESSER_ERR, 
    NAME_RESOLVER_ERR, TYPE_RESOLVER_ERR, DESUGARIZER_ERR,
    OTHER_ERR
} ErrorType;

typedef struct {
    ErrorType type;

    char* message;
    void* context;
} BrianError;

#endif 
