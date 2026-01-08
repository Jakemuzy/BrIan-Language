#ifndef _VECTOR_H__
#define _VECTOR_H__

#include <stdio.h>

#include "Any.h"

typedef struct VNode {
    B_ANY data;
    struct VNode* next;
    struct VNode* prev;
} VNode;

typedef struct Vector {
    int width;
    VNode* head;
    VNode* tail;
} Vector;

Vector MakeVector();
B_ANY At(Vector* v, int index);

void PushBack(Vector* v, B_ANY data);
B_ANY PopBack(Vector* v);

void Clear();

#endif
