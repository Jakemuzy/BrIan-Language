#ifndef _BRIAN_ERROR_HANDLER_H_ 
#define _BRIAN_ERROR_HANDLER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/*      BrIan Error Handler
    ----------------------------
     Refer to docs/pipeline.md 
           for details.   

*/

typedef enum {
    ERR_FLAG_PROPOGATE, ERR_FLAG_IGNORE, 
    ERR_FLAG_CONTINUE,  ERR_FLAG_ABORT,
    ERR_FLAG_EXIT
} ErrorRecovery;

typedef enum {
    TOKENIZER_ERR, PARSER_ERR, PREPROCESSER_ERR, 
    NAME_RESOLVER_ERR, TYPE_RESOLVER_ERR, DESUGARIZER_ERR,
    LLVM_ERR, COMPILER_ERR, 
} ErrorCategory;

typedef enum {
    /* Could really expand this if need be */
    /* Rename to ErrorCode */
    INSUFFICIENT_MEMORY_ERR, MEMORY_CONFLICT_ERR, 
    LOGICAL_ERR, UNKNOWN_ERR
} ErrorType;

typedef struct {
    char* msg;
    void* context;
} ErrorContext;

typedef struct {
    void* returnVal;
    ErrorCategory cat;
} Error;

// Probably want it to return null or something as well
Error ERROR(ErrorRecovery recovery, ErrorCategory cat, ErrorType type, const char* format, ...);
void WARN(ErrorCategory cat, ErrorType type, const char* format, ...);

#endif 
