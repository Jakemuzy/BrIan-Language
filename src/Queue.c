#include <Queue.h>

Queue MakeQueue()
{
    Queue q;
    q.tail = (Node*)malloc(sizeof(Node));
    q.head = q.tail;

    return q;
}

void FreeQueue(Queue* q)
{
    Node* nextChild = q->head;
    while(nextChild)
    {
        Node* currentChild = nextChild;
        nextChild = currentChild->child;
        free(currentChild);
    }
}

void Enqueue(Queue* q, int d)
{
    Node* currentTail = q->tail;

    /* Creates a new child filled data */
    currentTail->child = (Node*)malloc(sizeof(Node));
    if (!currentTail->child)
    {
        printf("Failed to init child node");
    }

    q->tail = currentTail->child;
    q->tail->data = d;

    printf("%d\n", (q->tail->data));
};

int Dequeue(Queue* q)
{
    Node* prevHead = q->head;
    int value = prevHead->data;
    
    q->head = prevHead->child;

    free(prevHead);
    return value;
}


void PrintQueue(Queue* q)
{
    printf("HEAD: %d\nTAIL: %d\n", (q->head->data), (q->tail->data));
}
    
