#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "NameResolver.h"
#include "DEBUG.h"

// Helper function to run a single test in a child process
static int run_single_test(const char *filepath, int expectedPass) {
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return -1;
    }
    
    if (pid == 0) {
        // Child process - run the test
        FILE* fptr = fopen(filepath, "r");
        if (!fptr) {
            exit(2);  // Can't open
        }
        
        AST* ast = Program(fptr);
        fclose(fptr);
        
        if (!ast) {
            exit(1);  // Parse failed
        }
        
        Namespaces* nss = ResolveNames(ast);
        int resultPass = (nss != NULL) ? 0 : 1;  // 0 = pass, 1 = fail
        exit(resultPass);
    }
    
    // Parent process - wait for child
    int status;
    waitpid(pid, &status, 0);
    
    if (!WIFEXITED(status)) {
        return -1;  // Child crashed
    }
    
    int exit_code = WEXITSTATUS(status);
    if (exit_code == 2) {
        return -2;  // Can't open file
    }
    
    int actualPass = (exit_code == 0) ? 1 : 0;
    return (actualPass == expectedPass) ? 1 : 0;  // 1 = pass, 0 = fail
}

int main(int argc, char* argv[])
{
        TestCase tests[] = {
        {"TestCases/NameResolver/AllCases.b", true},
        {"TestCases/NameResolver/ForBodyInit.b", true},
        {"TestCases/NameResolver/ForEscape.b", false},
        {"TestCases/NameResolver/ForScope.b", true},
        {"TestCases/NameResolver/ForShadow.b", true},
        {"TestCases/NameResolver/FuncLeak.b", false},
        {"TestCases/NameResolver/FuncShadow.b", true},
        {"TestCases/NameResolver/FuncUndefined.b", false},
        {"TestCases/NameResolver/IfNest.b", true},
        {"TestCases/NameResolver/IfNestDouble.b", true},
        {"TestCases/NameResolver/IfNestShadow.b", true},
        {"TestCases/NameResolver/IfScope.b", true},
        {"TestCases/NameResolver/IfShadow.b", true},
        {"TestCases/NameResolver/MultiVar.b", true},
        {"TestCases/NameResolver/ParamShadow.b", true},
        {"TestCases/NameResolver/Recursion.b", true},
        {"TestCases/NameResolver/Redeclaration.b", false},
        {"TestCases/NameResolver/Resolved.b", true},
        {"TestCases/NameResolver/Undefined.b", false},
        {"TestCases/NameResolver/VarCall.b", false},
        {"TestCases/NameResolver/VarEscape.b", false},
        {"TestCases/NameResolver/VarGlob.b", true},
        {"TestCases/NameResolver/VarInvalidScope.b", false},
        {"TestCases/NameResolver/VarShadow.b", true},
        {"TestCases/NameResolver/DoWhile.b", false},
        {"TestCases/NameResolver/While.b", false},
        {"TestCases/NameResolver/Switch.b", true},
        {"TestCases/NameResolver/For.b", true},
        {"TestCases/NameResolver/Return.b", true},
        {"TestCases/NameResolver/Struct.b", true},
        {"TestCases/NameResolver/IfVar.b", true},
        {"TestCases/NameResolver/IfVarUndefined.b", false},
        {"TestCases/NameResolver/WhileShadow.b", true},
        {"TestCases/NameResolver/WhileUndefined.b", false},
        {"TestCases/NameResolver/DoWhileBasic.b", true},
        {"TestCases/NameResolver/DoWhileInvalidScope.b", false},
        {"TestCases/NameResolver/ForInner.b", true},
        {"TestCases/NameResolver/ForInvalid.b", false},
        {"TestCases/NameResolver/SwitchBasic.b", true},
        {"TestCases/NameResolver/SwitchUndefined.b", false},
        {"TestCases/NameResolver/ReturnUndefined.b", false},
        {"TestCases/NameResolver/StructBasic.b", true},
        {"TestCases/NameResolver/StructFunc.b", true},
        {"TestCases/NameResolver/StructUndefined.b", true},
        {"TestCases/NameResolver/StructInvalidField.b", false},
        {"TestCases/NameResolver/NestedStructs.b", true},
        {"TestCases/NameResolver/EnumBasic.b", true},
        {"TestCases/NameResolver/EnumInvalid.b", true},
        {"TestCases/NameResolver/FuncArgs.b", true},
        {"TestCases/NameResolver/FuncBadArgs.b", false},
        {"TestCases/NameResolver/FuncCall.b", true},
        {"TestCases/NameResolver/FuncInvalidCall.b", false},
        {"TestCases/NameResolver/FuncExcessArgs.b", false},
        {"TestCases/NameResolver/FuncMinimalArgs.b", false},
    };
    size_t numTests = sizeof(tests) / sizeof(tests[0]);
    
    // If argument provided, run just that file (in child process for isolation)
    if (argc > 1) {
        int expectedPass = -1;
        for (size_t i = 0; i < numTests; i++) {
            if (strcmp(tests[i].filename, argv[1]) == 0) {
                expectedPass = tests[i].shouldPass;
                break;
            }
        }
        
        int result = run_single_test(argv[1], expectedPass);
        return (result == 1) ? 0 : 1;
    }
    
    // Otherwise, run all tests in the directory
    DIR *dir = opendir("TestCases/NameResolver");
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
            snprintf(filepath, sizeof(filepath), "%s/%s", "TestCases/NameResolver", entry->d_name);
            
            // Find expected result for this test
            int expectedPass = -1;
            for (size_t i = 0; i < numTests; i++) {
                if (strcmp(tests[i].filename, filepath) == 0) {
                    expectedPass = tests[i].shouldPass;
                    break;
                }
            }
            
            int result = run_single_test(filepath, expectedPass);
            
            if (result == -2) {
                printf("\tSKIP: %s (cannot open)\n", filepath);
            } else if (result == -1) {
                printf("\tERROR: %s (subprocess failed)\n", filepath);
                numFailed++;
            } else if (expectedPass == -1) {
                printf("\tUNKNOWN: %s (not in test cases)\n", filepath);
            } else if (result == 1) {
                printf("\tPASS: %s\n", filepath);
                numPassed++;
            } else {
                printf("\tFAIL: %s (expected %s, got %s)\n", 
                       filepath, 
                       expectedPass ? "pass" : "fail",
                       expectedPass ? "fail" : "pass");
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