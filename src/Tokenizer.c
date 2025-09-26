#include <stdio.h>
#include <stdbool.h>

#include <Queue.h>

int main(int argc, char* argv[])
{
	printf("Hello Cruel World: \n Param Count: %d \n Program Name: %s\n", argc, argv[0]);
    Queue Tokens  = MakeQueue();

    char curr;
    while((curr = getchar()) != '\n')
    {
        bool CHAR = true;
        if(curr == 'a')
        {
            Enqueue(&Tokens, CHAR);
        }

    }
    FreeQueue(&Tokens);
	return 0;
}
