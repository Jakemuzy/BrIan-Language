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

char* CaptureOutput(char* sysCommand)
{
    return "";
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
        char* filePath = currentPath;
        // Truncate last /*
        CompareFile(run, currentPath, filePath);
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
    char filePath[1024];
    snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, fileName);

    // Run compiler on file (with flags) and save output
    char sysCommand[2056];
    snprintf(sysCommand, sizeof(sysCommand), "%s %s %s", "./bin/compiler", filePath, run->compilerFlag ? run->compilerFlag : "");

    // Output redirect
    if (run->regenerate) {
        // If regenerating: replace golden file with current file output
        char* parentDirPath = GetParentDirPath(directoryPath);
        strncat(sysCommand, " >> ", sizeof(sysCommand) - strlen(sysCommand) - 1);
        strncat(sysCommand, parentDirPath, sizeof(sysCommand) - strlen(sysCommand) - 1);
        strncat(sysCommand, "golden/", sizeof(sysCommand) - strlen(sysCommand) - 1);

        int output = system(sysCommand);
        printf("\tREGENERATED %s: \t%s\n", (output == 0) ? "FAIL" : "PASS");
        return;
    } 
    else if (run->suppressOutput) {
        strncat(sysCommand, " > /dev/null 2>&1", sizeof(sysCommand) - strlen(sysCommand) - 1);
    }

    // Capture the output of the compiler run
    size_t capacity = 4096;
    size_t length = 0;
    char* commandOutput = malloc(capacity);
    if (!commandOutput) { printf("ERROR: malloc failed\n"); exit(1); }
    commandOutput[0] = '\0';

    FILE* pipe = popen(sysCommand, "r");
    if (!pipe) { printf("ERROR: popen failed\n"); free(commandOutput); exit(1); }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        size_t chunkLen = strlen(buffer);
        // Grow buffer if needed
        if (length + chunkLen + 1 >= capacity) {
            capacity *= 2;
            char* temp = realloc(commandOutput, capacity);
            if (!temp) { printf("ERROR: realloc failed\n"); free(commandOutput); exit(1); }
            commandOutput = temp;
        }
        memcpy(commandOutput + length, buffer, chunkLen + 1);
        length += chunkLen;
    }

    int output = pclose(pipe);
    printf("\tCOMPARED %s:\t%s\n",fileName, (output == 0) ? "FAIL" : "PASS");

    // ... use commandOutput ...

    free(commandOutput);  // Caller must free when done


    // If comparing: Compare golden file to current file output


    // Open parent directory then golden directory
    // Open golden file 

}

