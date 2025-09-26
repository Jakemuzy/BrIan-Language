#include <FileStream.h>

#define FILESIZE 100

FileStream ReadFileStream(char* filename)
{
    FileStream fs = (char*)malloc(sizeof(char)*FILESIZE);
    if(!fs)
    {
        printf("ERROR: Filestream failed to init\n");
    }

    FILE* fptr;
    fptr = fopen(filename, "r");

    fgets(fs, FILESIZE, fptr);
    printf("%s", fs);

    return fs;
}
