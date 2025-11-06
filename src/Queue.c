#include <DataStructures/Queue.h>

Queue MakeQueue()
{
    Queue q;
    q.head = NULL;
    q.tail = NULL;
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

void Enqueue(Queue* q, Token d)
{

    if (q->head == NULL && q->tail == NULL)
    {
        /* Fills the head with data */
        q->head = (Node*)malloc(sizeof(Node));
        if (!q->head)
        {
            printf("Failed to init head");
            return;
        }
        q->tail = q->head;
        q->tail->data = d;
    }
    else
    {
        Node* currentTail = q->tail;

        /* Creates a new tail filled data */
        currentTail->child = (Node*)malloc(sizeof(Node));
        
        /* Error Checking */
        if (!currentTail->child)
        {
           printf("Failed to init child node");
           return;
        }
        q->tail = currentTail->child;
        q->tail->data = d;
    }
    
};

Token Dequeue(Queue* q)
{
    if (q->head == NULL)
    {
        printf("Queue is empty\n");
        Token null;
        return null;
    }

    Node* prevHead = q->head;
    Token value = prevHead->data;
       
    if(q->head == q->tail)
        q->head = q->tail =  NULL;
    else 
        q->head = prevHead->child;

    free(prevHead);
    return value;
}


void PrintQueue(Queue* q)
{
	/*
    if(q->head == NULL)
    {
        printf("Nothing in Queue\n");
        return;
    }
    printf("HEAD: %d\nTAIL: %d\n", (q->head->data), (q->tail->data));
    */
}
    
