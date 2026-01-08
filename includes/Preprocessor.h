#ifndef _PREPROCESSOR_H__
#define _PREPROCESSOR_H__

#include "Tokenizer.h"

/* Get next token thats not skipped */
Token GetNextTokenP(FILE* fptr);

void RemoveComments(FILE* fptr);
void RemoveDirectives(FILE* fptr);
void ReplaceDirectives(FILE* fptr);

#endif 
