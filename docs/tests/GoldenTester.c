#include "GoldenTester.h"

/* ----- Helpers ----- */

char* GetParentDirPath(char* currentPath) 
{
    char* parentDirPath = strdup(currentPath);  
    if (!parentDirPath) return NULL;

    // Strip trailing slash first
    size_t len = strlen(parentDirPath);
    if (len > 1 && parentDirPath[len - 1] == '/')
        parentDirPath[len - 1] = '\0';

    char* lastSlash = strrchr(parentDirPath, '/');
    if (lastSlash)
        *(lastSlash + 1) = '\0';  // Truncate after the last slash

    return parentDirPath;  // MUST FREE LATER
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

    return result; // Must free later
}

char* CaptureOutput(char* sysCommand)
{
    // Append 2>&1 to capture stderr alongside stdout
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
    free(fullCommand);
    if (!pipe) { printf("ERROR: popen failed\n"); free(commandOutput); exit(1); }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        size_t chunkLen = strlen(buffer);
        if (length + chunkLen + 1 >= capacity) {
            capacity *= 2;
            char* temp = realloc(commandOutput, capacity);
            if (!temp) { printf("ERROR: realloc failed\n"); free(commandOutput); exit(1); }
            commandOutput = temp;
        }
        memcpy(commandOutput + length, buffer, chunkLen + 1);
        length += chunkLen;
    }

    pclose(pipe);
    return commandOutput;
}

char* ReadGolden(char* goldenPath) 
{
    FILE *f = fopen(goldenPath, "rb");
    if (f == NULL) {
        printf("Cannot open golden file: %s\n", goldenPath);
        return NULL;
    } 

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (buffer) {
        fread(buffer, 1, length, f);
        buffer[length] = '\0'; 
    }

    fclose(f);
    return buffer;
}

int CompareOutputs(char* runOutput, char* goldenOutput)
{
    // Can do somehting more sophisticated later
    return strcmp(runOutput, goldenOutput);
}

/* ----- Flag Parsing ----- */

int main(int argc, char* argv[]) 
{
    TestRun* run = ParseFlags(argc, argv);
    
    RecurseDirectories(run, run->directory);
    // CompileBrian(argc, argv);

    free(run);
    return 0;
}

TestRun* ParseFlags(int argc, char* argv[])
{
    if (argc < 2) { printf("ERROR: expected arguments.\n"); exit(1); }

    TestRun* run = malloc(sizeof(TestRun));
    run->directory = "./tests";
    run->regenerate = false;
    run->suppressOutput = false;        // Make this a flag

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--file") == 0) {
            if (++i >= argc) { printf("ERROR: expected file location after --file flag.\n"); exit(1); }

            run->directory = argv[i]; 
        }
        else if (strcmp(argv[i], "--regenerate") == 0) {
            run->regenerate = true;
        }
        else {
            // Directly pass other flags 
            // Yes only one is allowed for now 
            run->compilerFlag = argv[i];
        }
    }
    return run;
}


/* ----- Comparison ----- */

void RecurseDirectories(TestRun* run, char* currentPath) 
{
    // If individual file, handle separately 
    struct stat path_stat;

    if (stat(currentPath, &path_stat) != 0) {
        printf("ERROR: Stat has failed.\n");
        exit(1);
    }

    if (S_ISREG(path_stat.st_mode)) {
        char* pathCopy1 = strdup(currentPath); 
        char* pathCopy2 = strdup(currentPath); 

        char* dir  = dirname(pathCopy1);
        char* base = basename(pathCopy2);

        // Truncate last /*
        CompareFile(run, dir, base);
        free(pathCopy1); free(pathCopy2);
        return;
    }

    // If directory recurse
    struct dirent* entry;
    DIR* dp = opendir(currentPath);
    if (!dp) {
        printf("ERROR: Failed opening direcotry: %s.\n", currentPath);
        exit(1);
    }

    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0 ||
            strcmp(entry->d_name, "golden") == 0) {
            continue;
        }
        if (entry->d_type == DT_DIR) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", currentPath, entry->d_name);

            printf("In directory: %s.\n", path);
            RecurseDirectories(run, path);
        } else if (entry->d_type == DT_REG) {
            // Checks if its a .src file before entering
            const char* ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".src") == 0)
                CompareFile(run, currentPath, entry->d_name);
        }
    }

    closedir(dp);
    //ReadFile(run, dp, run->directory);
}

void CompareFile(TestRun* run, char* directoryPath, char* fileName) 
{
    // Strip trailing slash from directoryPath to normalize
    char dirNorm[512];
    strncpy(dirNorm, directoryPath, sizeof(dirNorm));
    size_t len = strlen(dirNorm);
    if (len > 1 && dirNorm[len - 1] == '/')
        dirNorm[len - 1] = '\0';

    // Open File
    char filePath[512];
    snprintf(filePath, sizeof(filePath), "%s/%s", dirNorm, fileName);

    // Run compiler on file (with flags) and save output
    char sysCommand[512];
    snprintf(sysCommand, sizeof(sysCommand), "%s %s %s", "./bin/compiler", filePath, run->compilerFlag ? run->compilerFlag : "");

    char* parentDirPath = GetParentDirPath(directoryPath);
    char* goldenFileName = GetGoldenFileName(fileName);

    // Output redirect
    if (run->regenerate) {
        // If regenerating: replace golden file with current file output
        char goldenDirCommand[512];
        snprintf(goldenDirCommand, sizeof(goldenDirCommand), "%s > %sgolden/%s 2>&1", sysCommand, parentDirPath, goldenFileName);

        int output = system(goldenDirCommand);
        printf("\tREGENERATED %s: \t%s\n", fileName, (output == 1) ? "FAIL" : "PASS");
        free(parentDirPath);
        return;
    } 
    else if (run->suppressOutput) {
        strncat(sysCommand, " > /dev/null 2>&1", sizeof(sysCommand) - strlen(sysCommand) - 1);
    }

    char* goldenPath[1028];
    snprintf(goldenPath, sizeof(goldenPath), "%sgolden/%s", parentDirPath, goldenFileName);

    // Capture the output of the compiler and the golden file 
    char* runOutput = CaptureOutput(sysCommand);
    char* goldenOutput = ReadGolden(goldenPath);

    int output = CompareOutputs(runOutput, goldenOutput);
    printf("\tCOMPARED %s:\t%s\n",fileName, (output == 1) ? "FAIL" : "PASS");

    free(goldenFileName);  
    free(parentDirPath);
}

