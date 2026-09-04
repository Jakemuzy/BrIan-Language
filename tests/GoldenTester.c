#include "GoldenTester.h"

/* ----- Helpers ----- */

char* GetParentDirPath(char* currentPath) 
{
    char* pathCopy = strdup(currentPath);
    if (!pathCopy) return NULL;
    
    char* dir = dirname(pathCopy);
    size_t len = strlen(dir);
    
    char* parentDirPath = malloc(len + 2);
    if (!parentDirPath) {
        free(pathCopy);
        return NULL;
    }
    
    strcpy(parentDirPath, dir);
    if (len > 0 && parentDirPath[len - 1] != '/') {
        parentDirPath[len] = '/';
        parentDirPath[len + 1] = '\0';
    }
    
    free(pathCopy);
    return parentDirPath;
}

char* GetGoldenFileName(char* fileName)
{
    const char* dot = strrchr(fileName, '.');
    if (!dot) return NULL;

    size_t baseLen = dot - fileName;
    char* result = malloc(baseLen + 5); 
    if (!result) return NULL;

    memcpy(result, fileName, baseLen);
    memcpy(result + baseLen, ".cmp", 5);

    return result;
}

char* CaptureOutput(TestRun* run, char* sysCommand, int* outExitCode)
{
    size_t cmdLen = strlen(sysCommand);
    char* fullCommand = malloc(cmdLen + 6); // " 2>&1\0"
    if (!fullCommand) { printf("ERROR: malloc failed\n"); exit(1); }
    memcpy(fullCommand, sysCommand, cmdLen);
    memcpy(fullCommand + cmdLen, " 2>&1", 6);

    size_t capacity = 4096;
    size_t length = 0;
    char* commandOutput = malloc(capacity);
    if (!commandOutput) { printf("ERROR: malloc failed\n"); free(fullCommand); exit(1); }
    commandOutput[0] = '\0';

    FILE* pipe = popen(fullCommand, "r");
    if (!pipe) { 
        printf("ERROR: popen failed\n"); 
        free(fullCommand); 
        free(commandOutput); 
        exit(1); 
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        size_t chunkLen = strlen(buffer);
        if (length + chunkLen + 1 >= capacity) {
            capacity *= 2;
            char* temp = realloc(commandOutput, capacity);
            if (!temp) { printf("ERROR: realloc failed\n"); free(fullCommand); free(commandOutput); pclose(pipe); exit(1); }
            commandOutput = temp;
        }
        memcpy(commandOutput + length, buffer, chunkLen + 1);
        length += chunkLen;
    }

    int status = pclose(pipe);
    if (WIFSIGNALED(status)) {
        // FIXED: Log fullCommand BEFORE freeing it
        printf("\tCRASHED %s: signal %d\n", fullCommand, WTERMSIG(status));
        free(fullCommand);
        run->failCount++;
        *outExitCode = -1;
        commandOutput[0] = '\0';
        return commandOutput;
    }
    
    free(fullCommand);

    if (WIFEXITED(status)) {
        *outExitCode = WEXITSTATUS(status);
    } else {
        *outExitCode = status;
    }

    return commandOutput;
}

char* ReadGolden(char* goldenPath) 
{
    FILE *f = fopen(goldenPath, "rb");
    if (f == NULL) return NULL;

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (buffer) {
        if (length > 0) {
            size_t readElements = fread(buffer, 1, length, f);
            buffer[readElements] = '\0';
        } else {
            buffer[0] = '\0';
        }
    }

    fclose(f);
    return buffer;
}

int CompareOutputs(char* runOutput, char* goldenOutput)
{
    return strcmp(runOutput, goldenOutput);
}

/* ----- Flag Parsing ----- */

int main(int argc, char* argv[]) 
{
    TestRun* run = ParseFlags(argc, argv);
    RecurseDirectories(run, run->directory);
    int failed = run->failCount;
    free(run);
    return failed > 0 ? 1 : 0;
}

TestRun* ParseFlags(int argc, char* argv[])
{
    if (argc < 2) { printf("ERROR: expected arguments.\n"); exit(1); }

    TestRun* run = malloc(sizeof(TestRun));
    run->directory = "./tests";
    run->regenerate = false;
    run->suppressOutput = false;
    run->failCount = 0;
    run->compilerFlag = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--file") == 0) {
            if (++i >= argc) { printf("ERROR: expected file location.\n"); exit(1); }
            run->directory = argv[i]; 
        }
        else if (strcmp(argv[i], "--regenerate") == 0) {
            run->regenerate = true;
        }
        else {
            run->compilerFlag = argv[i];
        }
    }
    return run;
}

/* ----- Comparison ----- */

void RecurseDirectories(TestRun* run, char* currentPath) 
{
    struct stat path_stat;
    if (stat(currentPath, &path_stat) != 0) {
        printf("ERROR: Stat has failed on %s\n", currentPath);
        exit(1);
    }

    if (S_ISREG(path_stat.st_mode)) {
        char* pathCopy1 = strdup(currentPath); 
        char* pathCopy2 = strdup(currentPath); 
        char* dir  = dirname(pathCopy1);
        char* base = basename(pathCopy2);

        CompareFile(run, dir, base);
        free(pathCopy1); free(pathCopy2);
        return;
    }

    DIR* dp = opendir(currentPath);
    if (!dp) {
        printf("ERROR: Failed opening directory: %s.\n", currentPath);
        exit(1);
    }

    struct dirent* entry;
    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0 ||
            strcmp(entry->d_name, "golden") == 0) {
            continue;
        }
        
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", currentPath, entry->d_name);

        if (entry->d_type == DT_DIR) {
            RecurseDirectories(run, path);
        } else if (entry->d_type == DT_REG) {
            const char* ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".src") == 0)
                CompareFile(run, currentPath, entry->d_name);
        }
    }
    closedir(dp);
}

void CompareFile(TestRun* run, char* directoryPath, char* fileName) 
{
    char dirNorm[512];
    strncpy(dirNorm, directoryPath, sizeof(dirNorm) - 1);
    dirNorm[sizeof(dirNorm) - 1] = '\0';
    size_t len = strlen(dirNorm);
    if (len > 1 && dirNorm[len - 1] == '/')
        dirNorm[len - 1] = '\0';

    char filePath[512];
    snprintf(filePath, sizeof(filePath), "%s/%s", dirNorm, fileName);

    char sysCommand[512];
    snprintf(sysCommand, sizeof(sysCommand), "%s %s %s", "./bin/compiler", filePath, run->compilerFlag ? run->compilerFlag : "");

    char* parentDirPath = GetParentDirPath(directoryPath);
    char* goldenFileName = GetGoldenFileName(fileName);
    if (!parentDirPath || !goldenFileName) {
        free(parentDirPath); free(goldenFileName);
        return;
    }

    char targetGoldenDir[1028];
    snprintf(targetGoldenDir, sizeof(targetGoldenDir), "%sgolden", parentDirPath);
    
    char mkdirCmd[1200];
    snprintf(mkdirCmd, sizeof(mkdirCmd), "mkdir -p %s", targetGoldenDir);
    int status = system(mkdirCmd); (void)status;
  
    if (run->regenerate) {
        char goldenDirCommand[1200];
        snprintf(goldenDirCommand, sizeof(goldenDirCommand), "%s > %s/%s 2>&1", sysCommand, targetGoldenDir, goldenFileName);

        int output = system(goldenDirCommand);
        printf("\tREGENERATED %s: \t%s\n", fileName, (output != 0) ? "FAIL" : "PASS");
        
        free(goldenFileName);
        free(parentDirPath);
        return;
    } 

    if (run->suppressOutput) {
        strncat(sysCommand, " > /dev/null", sizeof(sysCommand) - strlen(sysCommand) - 1);
    }

    char goldenPath[1028];
    snprintf(goldenPath, sizeof(goldenPath), "%s/%s", targetGoldenDir, goldenFileName);

    int compilerExitCode = 0;
    char* runOutput = CaptureOutput(run, sysCommand, &compilerExitCode);
    char* goldenOutput = ReadGolden(goldenPath);

    int expectedExitCode = 0; 
    bool pathRulesEvaluated = false;

    if (strstr(filePath, "/invalid/") != NULL) {
        expectedExitCode = 1;
        pathRulesEvaluated = true;
    } else if (strstr(filePath, "/valid/") != NULL) {
        expectedExitCode = 0;
        pathRulesEvaluated = true;
    }

    if (!goldenOutput) {
        printf("\tCOMPARED %s:\tFAIL (Missing or unreadable golden file)\n", fileName);
        run->failCount++;
    } else if (compilerExitCode == -1) {
        printf("\tCOMPARED %s:\tFAIL (Compiler crashed)\n", fileName);
    } else {
        int outputDiff = CompareOutputs(runOutput, goldenOutput);
        bool exitCodeCorrect = !pathRulesEvaluated || (compilerExitCode == expectedExitCode);
        bool passed = (outputDiff == 0 && exitCodeCorrect);

        if (passed) {
            printf("\tCOMPARED %s:\tPASS\n", fileName);
        } else {
            printf("\tCOMPARED %s:\tFAIL", fileName);
            if (outputDiff != 0) printf(" (Output mismatch)");
            if (!exitCodeCorrect) printf(" (Wrong exit code: expected %d, got %d)", expectedExitCode, compilerExitCode);
            printf("\n");
            run->failCount++;
        }
    }

    free(runOutput);
    free(goldenOutput);
    free(goldenFileName);
    free(parentDirPath);
}

