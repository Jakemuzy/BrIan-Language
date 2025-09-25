#include <stdio.h>
#include <TokenizerFunctions.h>

int main(int argc, char* argv[])
{
	printf("Hello Cruel World: \n Param Count: %d \n Program Name: %s\n", argc, argv[0]);
    
    struct Queue m;
    m.child = malloc(sizeof(struct Node));
    if (!m.child)
    {   
        printf("Failed to init child");
    }   
    
    int d = 8;
    Enqueue(&m, d);
    Enqueue(&m, 10);

    free(m.child);
	return 0;
}
