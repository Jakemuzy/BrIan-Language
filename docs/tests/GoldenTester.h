#ifndef _BRIAN_TESTER_H_
#define _BRIAN_TESTER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "Compiler.h"
#include "Tokenizer.h"
#include "Parser.h"

/*          BrIan Tester
    ----------------------------
    BrIans main testing framework
     Compares per phase program
     output to comparison files, 
    to determine output validity

*/

typedef struct TestRun {
    char* directory;
    bool regenerate;

    char* compilerFlag;
    size_t passCount, failCount;
} TestRun;

void ReadFile(TestRun* run, DIR* dp, char* currentPath);
void Compare(TestRun* run);
void Generate(TestRun* run);
TestRun* ParseFlags(int argc, char* argv[]);

#endif
