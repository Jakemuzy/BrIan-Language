#ifndef _QUEUE_H__
#define _QUEUE_H__

#include <stdio.h>
#include <stdlib.h>

struct Node {
    void* data;
    struct Node* child;
};

struct Queue {
    struct Node* child;
};

void Enqueue(struct Queue q);

#endif
