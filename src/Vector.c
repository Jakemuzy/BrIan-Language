#include "Vector.h"

Vector MakeVector(int size)
{
    Vector v;

    DataType type = C_NULL;
    int i = 0;

    v.width = size;
    v.head = malloc(sizeof(VNode));
    v.tail = v.head;

    VNode curr = v.root;
    while(i < size)
    {
        curr.data = ANY(NULL, C_NULL);
        curr.next = malloc(sizeof(VNode));
        curr.prev = curr;
        curr = curr.next;
        i++;
    }
}

void* At(Vector* v, int index)
{
    int i = 0;
    VNode curr = v.root;

    while(curr && i < element)
    {
        curr = curr.next;
        i++;
    }

    if(!curr.data)
    {
        printf("ERROR: Index %d has no data associated", index);
        return NULL;
    }

    return curr.data;
}
 
void PushBack(Vector* v, B_ANY data);
B_ANY PopBack(Vector* v);
 
void Clear();

