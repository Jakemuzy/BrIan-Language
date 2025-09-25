#include <TokenizerFunctions.h>

void Enqueue(struct Queue* q, int d)
{
    Node* currentChild = q->child;

    /* Finds the last node in the queue */
    while(currentChild->child)
        currentChild = currentChild->child;

    /* Creates a new child filled data */
    currentChild->child = (Node*)malloc(sizeof(Node));
    if (!currentChild->child)
    {
        printf("Failed to init child node");
    }

    currentChild->child->data = d;

    printf("%d\n", (currentChild->child->data));
};
