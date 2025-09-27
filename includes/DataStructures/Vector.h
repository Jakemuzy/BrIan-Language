#ifndef _VECTOR_H__
#define _VECTOR_H__

typedef struct Vector {
    /* Store data about primitive type and array size */
    int width;

    void* data;
    Vector* child;
} Vector;

Vector MakeVector(int size);
void* At(Vector* v, int element);

void PushBack(Vector* v, void* data);
void* PopBack(Vector* v);

void Clear();
void FreeVector();

#endif
