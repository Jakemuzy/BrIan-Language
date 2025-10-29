#include "Vector.h"

Vector MakeVector()
{
    Vector v;

    v.width = 0;
    v.head = malloc(sizeof(VNode));
    v.tail = v.head;

    v.head->prev = NULL;
    return v;
}

void* At(Vector* v, int index)
{
    int i = 0;
    VNode* curr = v->head;

    while(curr && i != index)
    {
        curr = curr->next;
        i++;
    }

    if(!curr->data)
    {
        printf("ERROR: Index %d has no data associated", index);
        return NULL;
    }

    return curr->data;
}
 
void Push(Vector* v, B_ANY data)
{ 
    VNode* curr = v->tail;
    curr->data = data;
    curr->next = malloc(sizeof(VNode));
    curr->next->prev = curr;
    
    v->tail = curr->next;
}
B_ANY Pop(Vector* v)
{
}
 
void Clear();

