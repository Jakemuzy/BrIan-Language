#include <stdio.h>
#include <Queue.h>

int main(int argc, char* argv[])
{
	printf("Hello Cruel World: \n Param Count: %d \n Program Name: %s\n", argc, argv[0]);
    
    Queue m = MakeQueue();
    
    int d = 8;
    Enqueue(&m, d);
    Enqueue(&m, 10);
    Enqueue(&m, 25);

    printf("%s\n", "Before: ");
    PrintQueue(&m);

    d = Dequeue(&m);
    printf("%s\n", "After: ");
    PrintQueue(&m);   
 
    FreeQueue(&m);
	return 0;
}
