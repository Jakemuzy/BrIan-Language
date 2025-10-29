#ifndef _VECTOR_H__
#define _VECTOR_H__

#include "Any.h"

typedef struct
{
    B_ANY data;
    struct VNode* next;
    struct VNode* prev;
} VNode;

typedef struct
{
    int width;
    VNode* head;
    VNode* tail;
} Vector;

Vector MakeVector();
void* At(Vector* v, int index);

void PushBack(Vector* v, B_ANY data);
B_ANY PopBack(Vector* v);

void Clear();

#endif
