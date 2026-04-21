#include "GoldenTester.h"

/* ----- Helpers ----- */

char* GetParentDirPath(char* currentPath) 
{
    char* parentDirPath = strdup(currentPath);  
    if (!parentDirPath) return NULL;

    char* lastSlash = strrchr(parentDirPath, '/');
    if (lastSlash)
        *(lastSlash + 1) = '\0';  // Truncate after the last slash

    return parentDirPath;  // MUST FREE LATER
}

char* GetGoldenFileName(char* fileName)
{
    char* file = strdup(fileName);  
    if (!file) return NULL;

    char* trimmedExtension = strrchr(file, '.');
    if (trimmedExtension) {
        *(++trimmedExtension) = 'c';
        *(++trimmedExtension) = 'm';
        *(++trimmedExtension) = 'p';
        *(++trimmedExtension) = '\0';
        return file;
    }

    return NULL; // Must free later again
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

void CompareOutputs(char* runOutput, char* goldenOutput)
{

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
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (entry->d_type == DT_DIR) {
            char path[1024];
            snprintf(path, sizeof(path), "%s%s", currentPath, entry->d_name);

            printf("In directory: %s.\n", path);
            RecurseDirectories(run, path);
        } else if (entry->d_type == DT_REG) {
            CompareFile(run, currentPath, entry->d_name);
        }
    }

    closedir(dp);
    //ReadFile(run, dp, run->directory);
}

void CompareFile(TestRun* run, char* directoryPath, char* fileName) 
{
    // Open File
    char filePath[512];
    snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, fileName);

    // Run compiler on file (with flags) and save output
    char sysCommand[512];
    snprintf(sysCommand, sizeof(sysCommand), "%s %s %s", "./bin/compiler", filePath, run->compilerFlag ? run->compilerFlag : "");

    // Output redirect
    if (run->regenerate) {
        // If regenerating: replace golden file with current file output
        char* parentDirPath = GetParentDirPath(directoryPath);
        char* goldenFileName = GetGoldenFileName(fileName);

        char goldenDirCommand[512];
        snprintf(goldenDirCommand, sizeof(goldenDirCommand), "%s %s %s%s%s", sysCommand, ">", parentDirPath, "golden/", goldenFileName);

        printf("%s\n", goldenDirCommand);
        int output = system(goldenDirCommand);
        printf("\tREGENERATED %s: \t%s\n", fileName, (output == 0) ? "FAIL" : "PASS");
        free(parentDirPath);
        return;
    } 
    else if (run->suppressOutput) {
        strncat(sysCommand, " > /dev/null 2>&1", sizeof(sysCommand) - strlen(sysCommand) - 1);
    }

    // Capture the output of the compiler run
    char* commandOutput = CaptureOutput(sysCommand);

    // Get golden output 
    // If comparing: Compare golden file to current file output

    int output = 0;
    printf("\tCOMPARED %s:\t%s\n",fileName, (output == 0) ? "FAIL" : "PASS");

    free(commandOutput);  
}

