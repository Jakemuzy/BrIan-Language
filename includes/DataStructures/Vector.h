#ifndef _VECTOR_H__
#define _VECTOR_H__

#include "Any.h"

typedef struct VNode 
{
    B_ANY data;
    VNode* next;
    VNode* prev;
}

typedef struct Vector 
{
    int width;
    void* head;
    void* tail;
} Vector;

Vector MakeVector(int size);
void* At(Vector* v, int index);

void PushBack(Vector* v, B_ANY data);
B_ANY PopBack(Vector* v);

void Clear();

#endif
