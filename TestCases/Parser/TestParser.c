#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "Parser.h"
#include "DEBUG.h"

/* TODO: Add a LOT more test cases */
int main(int argc, char* argv[])
{
    TestCase tests[] = {
        {"TestCases/Parser/AllCases.b", true},
        {"TestCases/Parser/Arithmetic.b", true},
        {"TestCases/Parser/Arrays.b", true},
        {"TestCases/Parser/EmptyMain.b", true},
        {"TestCases/Parser/Enum.b", true},
        {"TestCases/Parser/Functions.b", true},
        {"TestCases/Parser/GlobalVar.b", true},
        {"TestCases/Parser/NestedStructs.b", true},
        {"TestCases/Parser/NoMain.b", true},
        {"TestCases/Parser/StartFunc.b", true},
        {"TestCases/Parser/Struct.b", true},
        {"TestCases/Parser/Typedef.b", true},
        {"TestCases/Parser/Invalid.b", false},  // Note: This should fail
    };
    size_t numTests = sizeof(tests) / sizeof(tests[0]);
    
    // If argument provided, run just that file
    if (argc > 1) {
        FILE* fptr = fopen(argv[1], "r");
        if (!fptr) {
            printf("ERROR: Opening source file %s\n", argv[1]);
            return 1;
        }
        
        int expectedPass = -1;
        for (size_t i = 0; i < numTests; i++) {
            if (strcmp(tests[i].filename, argv[1]) == 0) {
                expectedPass = tests[i].shouldPass;
                break;
            }
        }
        
        AST* ast = Program(fptr);
        fclose(fptr);
        
        if (!ast) {
            printf("ERROR: ast failed to build\n");
            return expectedPass ? 1 : 0;
        }
        PrintAST(ast);
        return 0;
    }
    
    // Otherwise, run all tests in the directory
    DIR *dir = opendir("TestCases/Parser");
    if (!dir) {
        perror("opendir");
        return 1;
    }
    
    int numPassed = 0;
    int numFailed = 0;
    struct dirent *entry;
    
    while ((entry = readdir(dir)) != NULL) {
        size_t len = strlen(entry->d_name);
        if (len > 2 && strcmp(entry->d_name + len - 2, ".b") == 0) {
            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s/%s", "TestCases/Parser", entry->d_name);
            
            // Find expected result for this test
            int expectedPass = -1;
            for (size_t i = 0; i < numTests; i++) {
                if (strcmp(tests[i].filename, filepath) == 0) {
                    expectedPass = tests[i].shouldPass;
                    break;
                }
            }
            
            FILE* fptr = fopen(filepath, "r");
            if (!fptr) {
                printf("SKIP: %s (cannot open)\n", filepath);
                continue;
            }
            
            AST* ast = Program(fptr);
            fclose(fptr);
            
            int actualPass = (ast != NULL);
            
            if (expectedPass == -1) {
                printf("UNKNOWN: %s (not in test cases)\n", filepath);
            } else if (actualPass == expectedPass) {
                printf("PASS: %s\n", filepath);
                numPassed++;
            } else {
                printf("FAIL: %s (expected %s, got %s)\n", 
                       filepath, 
                       expectedPass ? "pass" : "fail",
                       actualPass ? "pass" : "fail");
                numFailed++;
            }
        }
    }
    
    closedir(dir);
    
    printf("\n=== Test Results ===\n");
    printf("Passed: %d\n", numPassed);
    printf("Failed: %d\n", numFailed);
    
    return numFailed > 0 ? 1 : 0;
}