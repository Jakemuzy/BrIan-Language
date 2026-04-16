#include "GoldenTester.h"

int main(int argc, char* argv[]) 
{
    TestRun* run = ParseFlags(argc, argv);
    
    Compare(run);
    // CompileBrian(argc, argv);

    free(run);
    return 0;
}

void ReadFile(TestRun* run, DIR* dp, char* currentPath)
{
    struct dirent* entry;
    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        else if (entry->d_type == DT_DIR) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", currentPath, entry->d_name);
            DIR* nextDP = opendir(path);

            if (nextDP == NULL) {
                printf("ERROR: Invalid directory.\n");
                exit(1);
            }
            ReadFile(run, nextDP, path);
        } else {
            char* type = (run->regenerate) ? "REGENERATING" : "TESTING";
            printf("%s: %s\n", type, entry->d_name);
        }
    }
    closedir(dp);
}

// Compares acutal files
void Compare(TestRun* run) 
{
    DIR* dp = opendir(run->directory);

    if (dp == NULL) {
        printf("ERROR: Invalid directory.\n");
        exit(1);
    }

    ReadFile(run, dp, run->directory);
}

// Regenerates comparison files
void Generate(TestRun* run)
{
    // Regenerates per file 

}

TestRun* ParseFlags(int argc, char* argv[])
{
    if (argc < 2) { printf("ERROR: expected arguments.\n"); exit(1); }

    TestRun* run = malloc(sizeof(TestRun));
    run->directory = "./";
    run->regenerate = false;

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
