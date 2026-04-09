#include "ErrorHandler.h"


/* 
Maybe have the context of the program in a stateful function, and call the function
The function will also have a section called Cleanup() where is frees all the memory that was allocated
and upon an abort or exit error that function will be called.
*/

void ERROR(ErrorRecovery recovery, ErrorCategory cat, const char* format, ...)
{
    char* catStr;
    switch (cat) {
        case TOKENIZER_ERR: catStr = "TOKENIZER"; break;
        case PARSER_ERR: catStr = "SYNTAX"; break;
        case PREPROCESSER_ERR: catStr = "PREPROCESSOR"; break;
        case NAME_RESOLVER_ERR: catStr = "NAME RESOLUTION"; break;
        case TYPE_RESOLVER_ERR: catStr = "TYPE RESOLUTION"; break;
        case LLVM_ERR: catStr = "LLVM CONVERSION"; break;
        case COMPILER_ERR: catStr = "COMPILER"; break;
        default: catStr = "UNKNOWN";
    }
    fprintf(stderr, "\n%s ERROR: ", catStr);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    switch (recovery) {
        case (ERR_FLAG_ABORT): abort();
        case (ERR_FLAG_EXIT): exit(1);
        case (ERR_FLAG_CONTINUE): return;
    }

}
