#ifndef _QUEUE_H__
#define _QUEUE_H__

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* child;
} Node;

typedef struct Queue {
    struct Node* child;
} Queue;

void Enqueue(struct Queue* q, int d);

#endif
