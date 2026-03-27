#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "TypeChecker.h"
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
        if (!nss) {
            exit(1);  // Name resolution failed
        }
        
        TYPE* typeCheck = TypeCheck(nss, ast->root);
        int resultPass = (typeCheck != NULL && typeCheck->kind != TYPE_ERROR) ? 0 : 1;
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
        {"TestCases/TypeChecker/DefinedType.b", true},
        {"TestCases/TypeChecker/ImplicitCast.b", true},
        {"TestCases/TypeChecker/InvalidImplictCast.b", false},
        {"TestCases/TypeChecker/InvalidInt.b", false},
        {"TestCases/TypeChecker/FuncType.b", true},
        {"TestCases/TypeChecker/FuncInvalidParamTypes.b", false},
        {"TestCases/TypeChecker/MemAccessInvalid.b", false},
        {"TestCases/TypeChecker/MemAccessValid.b", true},
        {"TestCases/TypeChecker/IntAssign.b", true},
        {"TestCases/TypeChecker/FloatToDoubleImplicitCast.b", true},
        {"TestCases/TypeChecker/InvalidFloatToIntAssign.b", false},
        {"TestCases/TypeChecker/InvalidVoidVarDecl.b", false},
        {"TestCases/TypeChecker/IntFloatArithmeticPromotion.b", true},
        {"TestCases/TypeChecker/InvalidStringToIntAssign.b", false},
        {"TestCases/TypeChecker/BoolToIntWidening.b", true},
        {"TestCases/TypeChecker/IntToBoolWidening.b", true},
        {"TestCases/TypeChecker/IntDivision.b", true},
        {"TestCases/TypeChecker/InvalidFloatToIntArithmetic.b", false},
        {"TestCases/TypeChecker/IntCompoundAssign.b", true},
        {"TestCases/TypeChecker/FloatIntCompoundAssign.b", true},
        {"TestCases/TypeChecker/InvalidStringCompoundAssign.b", false},
        {"TestCases/TypeChecker/UnaryNegation.b", true},
        {"TestCases/TypeChecker/UnaryLogicalNot.b", true},
        {"TestCases/TypeChecker/InvalidUnaryLogicalNotFloat.b", false},
        {"TestCases/TypeChecker/UnaryBitwiseNot.b", true},
        {"TestCases/TypeChecker/InvalidUnaryBitwiseNotFloat.b", false},
        {"TestCases/TypeChecker/BitwiseShift.b", true},
        {"TestCases/TypeChecker/InvalidBitwiseShiftFloat.b", false},
        {"TestCases/TypeChecker/BitwiseAnd.b", true},
        {"TestCases/TypeChecker/InvalidBitwiseAndFloat.b", false},
        {"TestCases/TypeChecker/BitwiseOr.b", true},
        {"TestCases/TypeChecker/BitwiseXor.b", true},
        {"TestCases/TypeChecker/LogicalAndBool.b", true},
        {"TestCases/TypeChecker/LogicalOrBool.b", true},
        {"TestCases/TypeChecker/EqualityComparison.b", true},
        {"TestCases/TypeChecker/InvalidEqualityMixedTypes.b", false},
        {"TestCases/TypeChecker/RelationalComparison.b", true},
        {"TestCases/TypeChecker/InvalidRelationalComparisonString.b", false},
        {"TestCases/TypeChecker/PowExpr.b", true},
        {"TestCases/TypeChecker/InvalidPowExprString.b", false},
        {"TestCases/TypeChecker/I8ToI16Widening.b", true},
        {"TestCases/TypeChecker/InvalidI16ToI8Narrowing.b", false},
        {"TestCases/TypeChecker/U8ToU16Widening.b", true},
        {"TestCases/TypeChecker/InvalidSignedToUnsigned.b", false},
        {"TestCases/TypeChecker/InvalidI64ToI32Narrowing.b", false},
        {"TestCases/TypeChecker/StructMemberAccess.b", true},
        {"TestCases/TypeChecker/InvalidStructMemberTypeMismatch.b", false},
        {"TestCases/TypeChecker/PostfixIncrementInt.b", true},
        {"TestCases/TypeChecker/PostfixIncrementFloat.b", true},
        {"TestCases/TypeChecker/InvalidPostfixIncrementString.b", false},
        {"TestCases/TypeChecker/FuncCall.b", true},
        {"TestCases/TypeChecker/InvalidFuncReturnTypeMismatch.b", false},
        {"TestCases/TypeChecker/InvalidFuncParamTypeMismatch.b", false},
        {"TestCases/TypeChecker/InvalidVoidFuncAssign.b", false},
        {"TestCases/TypeChecker/InvalidIfBodyTypeMismatch.b", false},
        {"TestCases/TypeChecker/WhileIntCondition.b", true},
        {"TestCases/TypeChecker/WhileBoolCondition.b", true},
        {"TestCases/TypeChecker/InvalidWhileStringCondition.b", false},
        {"TestCases/TypeChecker/TernaryMatchingTypes.b", true},
        {"TestCases/TypeChecker/InvalidTernaryMismatchedBranches.b", false},
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
    DIR *dir = opendir("TestCases/TypeChecker");
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
            snprintf(filepath, sizeof(filepath), "%s/%s", "TestCases/TypeChecker", entry->d_name);
            
            // Construct relative path to match test cases array
            char relative_path[512];
            snprintf(relative_path, sizeof(relative_path), "TestCases/TypeChecker/%s", entry->d_name);
            
            // Find expected result for this test
            int expectedPass = -1;
            for (size_t i = 0; i < numTests; i++) {
                if (strcmp(tests[i].filename, relative_path) == 0) {
                    expectedPass = tests[i].shouldPass;
                    break;
                }
            }
            
            int result = run_single_test(filepath, expectedPass);
            
            if (result == -2) {
                printf("\tSKIP: %s (cannot open)\n", relative_path);
            } else if (result == -1) {
                printf("\tERROR: %s (subprocess failed)\n", relative_path);
                numFailed++;
            } else if (expectedPass == -1) {
                printf("\tUNKNOWN: %s (not in test cases)\n", relative_path);
            } else if (result == 1) {
                printf("\tPASS: %s\n", relative_path);
                numPassed++;
            } else {
                printf("FAIL: %s (expected %s, got %s)\n", 
                       relative_path, 
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