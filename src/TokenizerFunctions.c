#include <TokenizerFunctions.h>

void Enqueue(struct Queue q)
{
    void* d  = q.child->data;
    printf("%d\n", *(int*)d);
};
