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

typedef enum ErrorRecovery {
    ERR_FLAG_ABORT, ERR_FLAG_EXIT, ERR_FLAG_CONTINUE
} ErrorRecovery;

typedef enum ErrorCategory {
    TOKENIZER_ERR, PARSER_ERR, PREPROCESSER_ERR, 
    NAME_RESOLVER_ERR, TYPE_RESOLVER_ERR, DESUGARIZER_ERR,
    LLVM_ERR, COMPILER_ERR, 
} ErrorCategory;

typedef struct Error {
    void* returnVal;
    ErrorCategory cat;
} Error;

// Probably want it to return null or something as well
Error ERROR(ErrorRecovery recovery, ErrorCategory cat, const char* format, ...);
void WARN(ErrorCategory cat, const char* format, ...);

#endif 
