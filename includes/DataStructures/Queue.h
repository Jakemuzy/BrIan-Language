#ifndef _QUEUE_H__
#define _QUEUE_H__

#include <stdio.h>
#include <stdlib.h>

#include <DataStructures/Token.h>

typedef struct Node {
    Token data;
    struct Node* child;
} Node;

typedef struct Queue {
    Node* head;
    Node* tail;
} Queue;

/* Maybe make this generic, pass the type */
Queue MakeQueue();
void FreeQueue(Queue* q);

void Enqueue(Queue* q, Token d);
Token Dequeue(Queue* q);

void PrintQueue(Queue* q);

#endif
