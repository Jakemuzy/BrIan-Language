#ifndef _BRIAN_TESTER_H_
#define _BRIAN_TESTER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

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
    bool regenerate, suppressOutput;

    char* compilerFlag;
    size_t passCount, failCount;
} TestRun;

/* ----- Helpers ----- */
char* GetParentDirPath(char* currentPath);
char* CaptureOutput(char* sysCommand);
void CompareOutputs(char* runOutput, char* goldenOutput);

/* ----- Comparison ----- */
void RecurseDirectories(TestRun* run, char* currentPath);
void CompareFile(TestRun* run, char* directoryPath, char* fileName);
//void RegenerateGolden(TestRun* run, DIR* dp, char* currentPath, char* fileName);

/* ----- Flag Parsing ----- */
TestRun* ParseFlags(int argc, char* argv[]);

#endif
